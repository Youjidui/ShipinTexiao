#include "StdAfx.h"
#include "RippleWipeRender.h"

#pragma warning(disable:4244)

CRippleWipeRender::CRippleWipeRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
}

CRippleWipeRender::~CRippleWipeRender(void)
{
	Uninit();
}

bool CRippleWipeRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/RippleWipe.fx"));
	ASSERT(m_pEffect);
	return (m_pEffect && m_pQuadMesh);
}

void CRippleWipeRender::Uninit()
{

}

bool CRippleWipeRender::Render( CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParamRaw )
{
	bool bOK = false;
	HRESULT hr = E_FAIL;

	RippleWipeFxParam* pParam = (RippleWipeFxParam*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();

	CVideoBuffer* pSrcDef[2];
	pSrcDef[0] = pSrcA;
	pSrcDef[1] = pSrcB;

	if(pParam->transParam.bReverse)
		std::swap(pSrcDef[0],pSrcDef[1]);

	//const Custom_Profile *pProfile = m_pEngine->GetCurProfile();
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	//Draw Full Src  Texture
	//handle_tpr hTemp[2] = {INVALID_RESID,INVALID_RESID};
	//TP_VBufferDef * pTempDef[2] = {pSrcDef[0],pSrcDef[1]};
	//for(int i = 0; i< 2;i ++)
	//{
	//	if(pTempDef[i]->BaseWidth != pProfile->nEditWidth || pTempDef[i]->BaseHeight != pProfile->nEditHeight
	//		|| pTempDef[i]->rcImage.left != CEIL(pTempDef[i]->OffsetX) || pTempDef[i]->rcImage.top != CEIL(pTempDef[i]->OffsetY))
	//	{
	//		hTemp[i] = NewRTBuffer(pTempDef[i]->OffsetX,pTempDef[i]->OffsetY,pProfile->nEditWidth,pProfile->nEditHeight);
	//		m_pEngine->EffectVideoCopy(hTemp[i], pSrcDef[i]->handle);
	//		pTempDef[i] = m_pResMan->GetBufferDef(hTemp[i]);
	//	}
	//}
	CVideoBuffer* pTempDef[2] = {pSrcDef[0],pSrcDef[1]};
	//for(int i = 0; i< 2;i ++)
	//{
	//	const VideoBufferInfo& bi = pTempDef[i]->GetVideoBufferInfo();
	//	if(bi.nAllocWidth != nEditWidth || bi.nAllocHeight != nEditHeight)
	//	{
	//		VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, nEditWidth, nEditHeight, 0, 0};
	//		pTempDef[i] = pBufMgr->CreateVideoBuffer(mediaBI);
	//		ASSERT(pTempDef[i]);
	//		bOK = m_pEngine->EffectVideoCopy(pTempDef[i], pSrcDef[i]);
	//		ASSERT(bOK);
	//	}
	//}

	//float fAspect = m_pResMan->GetAspect() * (pProfile->nEditWidth * (pSrcDef[0]->IsYUV16Buffer() ? 2.0f : 1.0f))  / (float)(pProfile->nEditHeight  * m_pResMan->GetAspectVerifyCoef());
	float fAspect = nEditWidth * 1.0f / nEditHeight;
	bOK = m_pEngine->SetRenderTarget(pDest);
	ASSERT(bOK);

	const VideoBufferInfo& vbi = pTempDef[0]->GetVideoBufferInfo();

	D3DXMATRIXA16 matCombine,matTex;		
	float scale_x   = pow( 10.0f, -pParam->fEllipticity );
	float scale_y   = 1.0f / pow( 10.0f, -pParam->fEllipticity );	

	float d = -pParam->fDistortion;
	D3DXVECTOR4 rot = D3DXVECTOR4( d  ,d  ,fAspect,0);

	float width  = pParam->fRipple_Width * 10.0f;
	float freq   = pParam->fNum_Wave * 2.0f;
	float amp    = pParam->fAmplitude  * 2.0f / pParam->fNum_Wave;
	float bright = pParam->fBrightness;

	float pos =  (width + sqrt(pow(1.0f * fAspect * scale_x  + fabs(pParam->fCenterX),2.0f) + pow(1.0f * scale_y + fabs(pParam->fCenterY),2.0f))) * pParam->transParam.progress;

	D3DXVECTOR4 wave( pos, amp, 1.0f / width, freq );

	float theta = -D3DXToRadian(pParam->fLight_Angle);

	float lx =  cos( theta ); 
	float ly =  sin( theta );
	float lz = 0;
	D3DXVECTOR4 light( -lx, -ly, lz, bright );

	D3DXMATRIX *matView = NULL, *matProj= NULL;
	pResMan->GetPerspectiveMatrix( &matView, &matProj);
	matCombine = (*matView) * (*matProj);
	D3DXMatrixIdentity(&matTex);
	matTex._31 = 0.5f / vbi.nAllocWidth;
	matTex._32 = 0.5f / vbi.nAllocHeight;

	hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matTex",&matTex);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTexture("g_txColor",pTempDef[0]->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTexture("g_txColor1",pTempDef[1]->GetTexture());
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetVector("g_vWave",&wave);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vLight",&light);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("rot",&rot);
	ASSERT(SUCCEEDED(hr));
	D3DXVECTOR4	vMisc(pParam->fCenterX * fAspect,-pParam->fCenterY,scale_x,scale_y);
	hr = m_pEffect->SetVector("g_vMisc",&vMisc);
	ASSERT(SUCCEEDED(hr));
	//D3DXVECTOR4 vAlpha(pSrcDef[0]->fAlphaValue,pSrcDef[1]->fAlphaValue,0,0);
	D3DXVECTOR4 vAlpha(1.0f, 1.0f, 0, 0);
	hr = m_pEffect->SetVector("g_vAlpha",&vAlpha);
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetMatrix("g_matTex",&matTex);
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetTechnique("RippleWipe");
	ASSERT(SUCCEEDED(hr));

	if(SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass;
		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->BeginPass(0);
		ASSERT(SUCCEEDED(hr));
		bOK = m_pQuadMesh->DrawMeshFx();
		ASSERT(bOK);
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->End();
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->EndScene();
		ASSERT(SUCCEEDED(hr));
	}	

	for(int i = 0; i < 2 ;i ++)
	{
		//FreeRTBuffer(hTemp[i]);
		if(pTempDef[i] != pSrcDef[i])
			pBufMgr->ReleaseVideoBuffer(pTempDef[i]);
	}

	return true;
}
