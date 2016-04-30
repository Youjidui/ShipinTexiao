#include "StdAfx.h"
#include "MeltRender.h"
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"

CMeltRender::CMeltRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
}

CMeltRender::~CMeltRender(void)
{
}

bool CMeltRender::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Melt.fx"));
	return true;
}

void CMeltRender::Uninit()
{

}

bool CMeltRender::Render( CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam )
{

}

bool	CMeltRender::RenderMelt(TPERParamMelt* pParam, const TPImageData* pSrcImage, TPImageData* pDstImage)
{
	// 获取引擎以及公共资源
	CBaseMesh*		pQuadMesh = m_pQuadMesh;
	CBaseFx*		pBaseFx = m_pEffect;
	CBaseTexture*	pRandomTex = TPGetEffectRenderEngine()->GetTexture(m_uResRandomPixel);
	assert(pBaseFx);

	TP_VBufferDef* pSrcDef = TPGetEffectRenderEngine()->GetVideoMemBuffer(pSrcImage);
	TP_VBufferDef* pDstDef = TPGetEffectRenderEngine()->GetVideoMemBuffer(pDstImage);

	if( pBaseFx == NULL || pSrcDef == NULL || pDstDef == NULL )
		return false;
	if( !TPGetEffectRenderEngine()->BeVideoDataValid(pSrcDef) 
		|| !TPGetEffectRenderEngine()->BeVideoDataValid(pDstDef) )
		return false;


	TPEffectAutoLock l(TPGetEffectRenderEngine());

	SetEffectParamImage(pBaseFx,pSrcDef,pDstDef,1.f,1.f,0.f,0.f,1.f,1.f,pParam->fRotateRadian,pParam->bSecondeFiled);

	pBaseFx->SetTexture("g_txRandomPixel",pRandomTex);

	D3DXVECTOR4 vTexSize(pSrcDef->GetImageWidth()*pSrcDef->GetImageHeight()*2/pSrcDef->GetImageWidth(),
		pSrcDef->GetImageHeight(),0.f,0.f);
	D3DXVECTOR4 vParamNoise(pParam->paramSmoothNoise.fAmplitude,
		pParam->paramSmoothNoise.fFrequency * 0.4f,
		pParam->paramSmoothNoise.fPhase,
		pParam->paramSmoothNoise.uInterpolationType);
	D3DXVECTOR4 vParamMelt(pParam->fPosition,
		pParam->fAmplitude,
		1.f,
		tanf(pParam->fSlantRadian));
	D3DXVECTOR4 vParamDensity(pParam->fDensity*10.f,pParam->fDensityPos,0.f,0.f);
	D3DXVECTOR4 vParamRandom(0.1f,0.1f,pParam->fRandomPix,0.f);

	pBaseFx->SetVector("g_texSize",&vTexSize);
	pBaseFx->SetVector("paramNoise",&vParamNoise);
	pBaseFx->SetVector("paramMelt",&vParamMelt);
	pBaseFx->SetVector("paramDensity",&vParamDensity);
	pBaseFx->SetVector("paramRandom",&vParamRandom);


	// setRT
	//pDstDef->bClearFullTarget = true;		// 基类处理，尤其是逐字情况
	pDstDef->bDiscardAlpha = false;
	pDstDef->bRenderToFullTarget = false;
	TPGetEffectRenderEngine()->SetRenderTarget0(pDstDef,pDstDef->COLOR_BLACK(),0x0);

	// render
	pBaseFx->SetTechnique("MeltEffect");
	if ( SUCCEEDED(TPGetEffectRenderEngine()->BeginScene()))
	{		
		UINT cPass = 0;
		pBaseFx->Begin(&cPass,0);

		pBaseFx->BeginPass(0);
		pQuadMesh-> DrawMeshFx();
		pBaseFx->EndPass();

		pBaseFx->End();

		TPGetEffectRenderEngine()->EndScene();
	}



	TPGetEffectRenderEngine()->ProcessDestAfterRender(pSrcDef,pDstDef);

	return true;
}

