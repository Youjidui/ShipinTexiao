#include "StdAfx.h"
#include "DissolveRender.h"
#include "../Utility/ColorConvertor.h"

#pragma warning(disable:4244)

CDissolveRender::CDissolveRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
}

CDissolveRender::~CDissolveRender(void)
{
	Uninit();
}

bool CDissolveRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/Dissolve.fx"));
	ASSERT(m_pEffect);

	//return SUCCEEDED(hr);
	return true;
}

void CDissolveRender::Uninit()
{
}

bool CDissolveRender::Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParamRaw)
{
	DissolveFxParam* pParam = (DissolveFxParam*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	if( pParam->prm_bReverse == TRUE )
		std::swap(pSrcA, pSrcB);

	CVideoBuffer* pSrcFullDef0 = pSrcA;
	CVideoBuffer* pSrcFullDef1 = pSrcB;
	CVideoBuffer* pDstDef = pDest;

	// 得到帧信息
	float fTime = pParam->prm_process;
	if(fTime < 1)
		fTime = fTime / 10;
	else
		fTime = fTime / 100;
	m_pEffect->SetFloat("g_ftime",fTime);

	// 对CG和VIDEO分情况考虑
	UINT pSrcFullDef0_bufferFormat = FMT_RGBA32;
	bool bYUVA	= (pSrcFullDef0_bufferFormat == FMT_YUVA32);
	bool bYUYV	= (false);
	bool bAlpha0 = (false);
	bool bYUYVA0 = bYUYV&&bAlpha0;
	bool bAlpha1 = (false);
	bool bYUYVA1 = bYUYV&&bAlpha1;
	bool bYUYVDst = false;

	//////////////////////////////////////////////////////////////////////////
	//渲染pass
	//////////////////////////////////////////////////////////////////////////

	// setRT
	m_pEngine->SetRenderTarget(pDstDef);

	// VS constant

	// 如果是改变了Dst大小的特技，需要重新定位
	D3DXMATRIX *matView,*matPorj;
	pResMan->GetOrthoMatrix(&matView,&matPorj);
	D3DXMATRIX matCombined = (*matView)*(*matPorj);

	D3DXMATRIX matTextureSrc;
	D3DXMatrixIdentity(&matTextureSrc);
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	const VideoBufferInfo& biSrc = pSrcFullDef0->GetVideoBufferInfo();
	matTextureSrc._11 = nEditWidth * (false ? 0.5f : 1.0f) / biSrc.nAllocWidth;
	matTextureSrc._22 = nEditHeight * 1.0f / biSrc.nAllocHeight;
	matTextureSrc._31 = 0.5f / biSrc.nAllocWidth;
	matTextureSrc._32 = 0.5f / biSrc.nAllocHeight;

	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombined);
	m_pEffect->SetMatrix("g_matTexture",&matTextureSrc);	

	// PS constant
	D3DXVECTOR4	colorMix(0.f,0.f,0.f,0.f);
	if(pParam->prm_mixType == 0)	//mix
	{
		colorMix.x = 1-pParam->prm_process;
		colorMix.y = pParam->prm_process;
	}
	else	//super mix && non-additive mix
	{
		if(pParam->prm_process <= 0.5f)
		{
			colorMix.x = 1.f;
			colorMix.y = pParam->prm_process/0.5f;
		}
		else
		{
			colorMix.x = (1-pParam->prm_process)/0.5f;
			colorMix.y = 1.f;
		}
	}
	switch(pParam->prm_mixType)
	{
	case 1:
		colorMix.z = 1.f;
		break;
	case 3:
	case 4:
		colorMix.z = pParam->prm_process;
		break;
	default:
		break;
	}

	float subtrcontr = pParam->prm_subtractr / 2.0f + 64.f/255.f;
	m_pEffect->SetVector("g_mix",&colorMix);
	m_pEffect->SetFloat("g_subtrcontr",subtrcontr);

	D3DXVECTOR4 fAlphaAB(0.f,0.f,0.f,0.f);
	float pSrcFullDef0_fAlphaValue = 1.0f, pSrcFullDef1_fAlphaValue = 1.0f;
	fAlphaAB.x = pSrcFullDef0_fAlphaValue;
	fAlphaAB.y = pSrcFullDef1_fAlphaValue;
	m_pEffect->SetVector("g_fAlphaBuffer",&fAlphaAB);

	D3DXCOLOR colorBlack(0.f,0.f,0.f,0.f);
	if( bYUVA)
		ColorConvertor::RGBA2YUVA(&colorBlack,&colorBlack);
	pDevice->SetPixelShaderConstantF(2,(float*)&colorBlack,4);


	// TEXTURE & stage state & sample state
	m_pEffect->SetTexture("g_txColor0",pSrcFullDef0->GetTexture());
	m_pEffect->SetTexture("g_txColor1",pSrcFullDef1->GetTexture());

	// render state
	pDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	// shader
	if(pParam->prm_mixType <= 2)
	{
		if( bYUYVDst)
			m_pEffect->SetTechnique("RenderYUYV_AP");
		else if(bYUYV)
			m_pEffect->SetTechnique("RenderYUYV_NA");	
		else if(bYUVA)
			m_pEffect->SetTechnique("RenderYUVA");
		else
			m_pEffect->SetTechnique("RenderRGBA");
	}
	else if(pParam->prm_mixType == 3)
	{
		m_pEffect->SetTechnique("RenderSubtraction");
	}
	else
	{
		m_pEffect->SetTechnique("RenderSandStorm");
	}

	// begin&end scene
	if ( SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass,uPass;
		m_pEffect->Begin(&cPass,0);
		for(uPass=0;uPass<cPass;uPass++)
		{
			m_pEffect->BeginPass(uPass);
			m_pQuadMesh-> DrawMeshFx();
			m_pEffect->EndPass();
		}
		m_pEffect->End();

		pDevice->EndScene();
	}

	// 恢复设备状态
	pDevice->SetRenderTarget(1,NULL);

	//////////////////////////////////////////////////////////////////////////
	//渲染pass结束
	//////////////////////////////////////////////////////////////////////////


	return true;
}

