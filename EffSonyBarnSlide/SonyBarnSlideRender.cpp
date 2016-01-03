#include "StdAfx.h"
#include "SonyBarnSlideRender.h"

CSonyBarnSlideRender::CSonyBarnSlideRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pMesh(NULL)
, m_pEffect(NULL)
{
}

CSonyBarnSlideRender::~CSonyBarnSlideRender(void)
{
	Uninit();
}

bool CSonyBarnSlideRender::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/BarnSlide.fx"));
	ASSERT(m_pEffect);
	return true;
}

void CSonyBarnSlideRender::Uninit()
{
}

bool CSonyBarnSlideRender::Render( CVideoBuffer* pDst, CVideoBuffer* pSrcFg, CVideoBuffer* pSrcBg, FxParamBase* pParamRaw )
{
	ASSERT(m_pEngine);
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	SonyBarnSlideFxParam* pParam = (SonyBarnSlideFxParam*)pParamRaw;
	CBaseFx* pBaseFx = m_pEffect;

	//D3DXMATRIX matWorld;
	D3DXMATRIX *matView = NULL, *matProj= NULL;
	//D3DXMatrixIdentity(&matWorld);
	pResMgr->GetOrthoMatrix(&matView, &matProj);
	D3DXMATRIX matWVP = *matView * *matProj;

	const VideoBufferInfo& biSrc = pSrcFg->GetVideoBufferInfo();
	D3DXMATRIXA16 matTex;
	D3DXMatrixIdentity(&matTex);
	matTex._31 = 0.5f / biSrc.nAllocWidth;
	matTex._32 = 0.5f / biSrc.nAllocHeight;

	m_pEngine->SetRenderTarget(pDst);

	pBaseFx->SetTexture("g_txColor",pSrcFg->GetTexture());
	pBaseFx->SetTexture("g_txBG",pSrcBg->GetTexture());

	pBaseFx->SetMatrix("g_matMVP",&matWVP);
	pBaseFx->SetMatrix("g_matTex",&matTex);
	float pSrcFg_fAlphaValue = 1.f, pSrcBg_fAlphaValue = 1.f;
	D3DXVECTOR4 vAlpha(pSrcFg_fAlphaValue,pSrcBg_fAlphaValue,0.0f,0.0f);
	pBaseFx->SetVector("g_vAlpha",&vAlpha);

	//float fAspect = TPGetEffectRenderEngine()->GetTPFrameSize(0,0,NULL);
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;
	float fRotateRad = pParam->fSlant /180 * D3DX_PI;

	D3DXVECTOR4 vResizeTex;
	vResizeTex.x = -1.f/fAspect; // float(pSrcDef->BaseWidth) / pSrcDef->BaseHeight;
	vResizeTex.y = 1.f;
	vResizeTex.z = 0.5f;
	vResizeTex.w = 0.5f;
	pBaseFx->SetVector("g_aspect",&vResizeTex);

	D3DXMATRIX matRot;

	D3DXMatrixRotationZ(&matRot,fRotateRad);


	D3DXMATRIX matAspect;

	D3DXMatrixIdentity(&matAspect);

	matAspect._11 = fAspect;

	matRot = matAspect * matRot;

	pBaseFx->SetMatrix("g_matRot",&matRot);

	float w = 0.5f * fAspect;
	float h = 0.5f;

	float trans_x = 0.0f;
	float trans_y = 0.0f;

	float cos_rotate = cosf(fRotateRad);
	float sin_rotate = sinf(fRotateRad);


	D3DXVECTOR4 vecRot = D3DXVECTOR4(  cos_rotate /fAspect
		, -sin_rotate/fAspect
		, -1.0f      *  sin_rotate
		, -1.0f      *  cos_rotate
		);

	pBaseFx->SetVector("g_rot",&vecRot );

	switch ( pParam->nType )
	{
	case 0:
		trans_x = pParam->fTrans * ( cos_rotate * w + fabs( sin_rotate ) * h );
		break;

	case 1:
		trans_y = pParam->fTrans * ( cos_rotate * h + fabs( sin_rotate ) * w );
		break;

	case 2:
		{
			float unit  = cos_rotate * h + fabs( sin_rotate ) * w;
			float unit2 = cos_rotate * w - fabs( sin_rotate ) * h;

			if ( unit > unit2 )
			{
				unit = w / ( cos_rotate + fabs( sin_rotate ) );
			}

			trans_x = pParam->fTrans * unit;
			trans_y = pParam->fTrans * unit;
		}
	}

	pBaseFx->SetVector("g_trans",&D3DXVECTOR4(trans_x,trans_y,0,0) );


	pBaseFx->SetTechnique("BarnSlide");
	// render

	if ( SUCCEEDED(pDevice->BeginScene()))
	{		
		UINT cPass = 0;
		pBaseFx->Begin(&cPass,0);

		pBaseFx->BeginPass(0);
		m_pQuadMesh-> DrawMeshFx();
		pBaseFx->EndPass();

		pBaseFx->End();

		pDevice->EndScene();
	}

	return true;
}

