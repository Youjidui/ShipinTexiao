#include "StdAfx.h"
#include "SonyBlurRender.h"

CSonyBlurRender::CSonyBlurRender(void)
{
}

CSonyBlurRender::~CSonyBlurRender(void)
{
}

bool CSonyBlurRender::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/SonyBlur.fx"));
	ASSERT(m_pEffect);
	return true;
}

void CSonyBlurRender::Uninit()
{

}

static int getSizeDownScale( float blurPixel )
{
	UINT baseCount = 32;
	int	 scale = 1;
	while( baseCount < 720 )		// 全屏幕的一半范围
	{
		if( blurPixel+1 <= baseCount )
			break;
		baseCount*=2;
		scale*=2;
	}
	return scale;
}

static void computeBlurPassCount( float blurPixel, UINT& passCount, float& endPassCoef )
{
	passCount = 0;
	endPassCoef = 1.f;

	int downScale = getSizeDownScale(blurPixel);

	if( downScale == 1 )
	{
		UINT thisPassCount = 0;
		for( int i=0; i<5; ++i )
		{
			passCount = i+1;
			thisPassCount = thisPassCount*2+1;
			if( blurPixel < thisPassCount )
				break;
		}
		endPassCoef = (1<<(passCount-1)) / (blurPixel+1);
	}
	else
	{
		passCount = 5;
		float scaleDownBlurPixel = (float)blurPixel / downScale;
		endPassCoef = 16.f / (scaleDownBlurPixel+1);
	}
}


bool CSonyBlurRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrc, SonyBlurFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	// 参数调整
	float blurPixelX = pParam->blurX;		// 1,2,3...31;   32,35,37...62 这个数字表示模糊的像素个数
	float blurPixelY = pParam->blurY;

	// loading和计算工作
	// 计算总共需要的pass次数
	UINT passCountX = 0;
	float endPassCoefX = 1.f;
	computeBlurPassCount(blurPixelX,passCountX,endPassCoefX);
	UINT passCountY = 0;
	float endPassCoefY = 1.f;
	computeBlurPassCount(blurPixelY,passCountY,endPassCoefY);


	bool bYUYV	= false;//(pSrcDef->IsYUV16Buffer());
	bool bAlpha = false;//(/*(!pSrcDef->bDiscardAlpha)&&*/(pSrcDef->pAlpha!=NULL));
	bool bYUYVA = bYUYV&&bAlpha;

	if( bYUYVA )
		m_pEffect->SetTechnique("RenderTWORT");
	else
		m_pEffect->SetTechnique("RenderONERT");

	// VS constant
	D3DXMATRIX	*matWorld,*matView,*matPorj;
	pResMan->GetQuadMatrix(&matWorld,&matView,&matPorj);
	D3DXMATRIX matCombined = (*matWorld)*(*matView)*(*matPorj);
	D3DXMATRIX matTextureSrc;
	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombined);

	if ( SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass = 0;
		m_pEffect->Begin(&cPass,0);

		for( int pass=0; pass<2; pass++ )				// X direction		// Y direction		
		{
			UINT &passCount = (pass==0)?passCountX:passCountY;
			float &endPassCoef = (pass==0)?endPassCoefX:endPassCoefY;

			int gap=1;

			m_pEffect->BeginPass(pass);
			for( UINT i=0; i<passCount; ++i )
			{
				// TEXTURE & stage state & sample state
				m_pEffect->SetTexture("g_txColor",pSrc->GetTexture());
				//if( bYUYVA )
				//	m_pEffect->SetTexture("g_txAlpha",((CBaseTexture*)(pSrcDef->pAlpha))->GetTexture());

				// PS constant
				bool bOK = GenerateMatrix(pSrc, &matTextureSrc, mat_Image);
				ASSERT(bOK);
				m_pEffect->SetMatrix("g_matTexture",&matTextureSrc);

				D3DXVECTOR4 g_texBound;				// left,right; top,bottom;
				const VideoBufferInfo& bi = pSrc->GetVideoBufferInfo();
				RECT rcImage = {0,0,bi.nWidth,bi.nHeight};
				g_texBound.x = (rcImage.left+0.5f) / bi.nAllocWidth;
				g_texBound.y = (rcImage.right-0.5f) / bi.nAllocWidth;
				g_texBound.z = (rcImage.top+0.5f) / bi.nAllocHeight;
				g_texBound.w = (rcImage.bottom-0.5f) / bi.nAllocHeight;
				m_pEffect->SetVector("g_texBound",&g_texBound);

				float pixelWidth = 1.f/bi.nAllocWidth;
				float pixelHeight = 1.f/bi.nAllocHeight;

				// PS constant
				D3DXVECTOR4	g_pixelOffset;			// 在外部计算不同pass所需要的采样间隔
				g_pixelOffset.x = gap * pixelWidth;
				g_pixelOffset.y = gap * pixelHeight;
				gap *= 2;
				m_pEffect->SetVector("g_pixelOffset",&g_pixelOffset);

				D3DXVECTOR4 g_coeff(0.25f,0.5f,0.25f,0.f);
				if( i==passCount-1 )
				{
					g_coeff.x = g_coeff.z = (1-endPassCoef)/2.f;
					g_coeff.y = endPassCoef;
				}
				m_pEffect->SetVector("g_coeff",&g_coeff);
				D3DXVECTOR4 vAlphaBlend(0,0,0,0);
				//if(m_bBlurEffect)
				//{
				//	if(pass == 0 && i == 0)
				//		vAlphaBlend.x = 1;
				//	else if(pass == 1 && i == passCount - 1)
				//		vAlphaBlend.y = 1;
				//}
				m_pEffect->SetVector("g_vAlphaBlend",&vAlphaBlend);

				// setRT
				m_pEngine->SetRenderTarget(pDest);

				m_pEffect->CommitChanges();

				m_pQuadMesh->DrawMeshFx();

				//   				m_pResMan->DumpResourceToFile(pDstDef->handle,L"C:/blurred.dds");
				//   				m_pResMan->DumpResourceToFile(pSrcDef->handle,L"C:/src.dds");
			}
			m_pEffect->EndPass();
		}

		m_pEffect->End();
		pDevice->EndScene();
		m_pEngine->SetRenderTarget(NULL);
		//m_pEffect->SetTexture("g_txColor", NULL);
	}
	return true;
}
