#include "StdAfx.h"
#include "AmoebaWipeRender.h"
#include "../Utility/ColorConvertor.h"

#define		NOISE_BITMAP_SIZE	32


VOID WINAPI ColorFill (D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
					   const D3DXVECTOR2* pTexelSize, LPVOID pData)
{

	float	fPhase = *(float*)pData;
	float	fValue = (cosf((rand() % 10001) / 10000.0f * 2.0f * D3DX_PI + fPhase) + 1.0f) / 2.0f;

	*pOut = D3DXVECTOR4(fValue,fValue,fValue,fValue);

}


CAmoebaWipeRender::CAmoebaWipeRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
, m_pNoiseTexture(NULL)
, m_nRandomSeed(-1)
, m_fPhase(0.123456789f)
{
}

CAmoebaWipeRender::~CAmoebaWipeRender(void)
{
	Uninit();
}


bool CAmoebaWipeRender::Init( CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/AmoebaWipeTrans.fx"));
	ASSERT(m_pEffect);
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, nEditWidth, nEditHeight, 0, 0};
	m_pNoiseTexture = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(m_pNoiseTexture);
	m_blurRender.Init(pEngine);
	return true;
}


void CAmoebaWipeRender::Uninit()
{
	m_blurRender.Uninit();
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	pBufMgr->ReleaseVideoBuffer(m_pNoiseTexture);
	//CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	//pResMgr->ReleaseEffect();
}


bool CAmoebaWipeRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, AmoebaWipeFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	//float	fAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;

	bool bOK = CreateNoiseTexture(pParam);
	ASSERT(bOK);
	//D3DXSaveSurfaceToFile(_T("./AmoebaWipe_Noise.bmp"), D3DXIFF_BMP, m_pNoiseTexture->GetSurface(), NULL, NULL);

	//handle_tpr hKey[3] = {INVALID_RESID,INVALID_RESID};
	//TP_VBufferDef * pKeyDef[3];

	//for(int i = 0; i < 3 ;i ++)
	//{
	//	hKey[i] = NewRTBuffer(0,0,pProfile->nEditWidth,pProfile->nEditHeight);
	//	pKeyDef[i] = m_pResMan->GetBufferDef(hKey[i]);
	//}	

	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, nEditWidth, nEditHeight, 0, 0};
	CVideoBuffer* pMedia = pBufMgr->CreateVideoBuffer(mediaBI);
	Resize(pMedia, m_pNoiseTexture, pParam);

	//D3DXSaveTextureToFile(_T("./AmoebaWipe_Resize.bmp"), D3DXIFF_BMP, pMedia->GetTexture(), NULL);
	//D3DXSaveSurfaceToFile(_T("./AmoebaWipe_Resize.bmp"), D3DXIFF_BMP, pMedia->GetSurface(), NULL, NULL);

	CVideoBuffer* pMediaBlur = pBufMgr->CreateVideoBuffer(mediaBI);
	SonyBlurFxParam blurParam;
	blurParam.blurX = 200.0f / (4.0f + 28.0f * pParam->fBumpDensity);
	blurParam.blurY = 200.0f / (4.0f + 28.0f * pParam->fBumpDensity) / fAspect;
	m_blurRender.Render(pMediaBlur, pMedia, &blurParam);

	//D3DXSaveTextureToFile(_T("./AmoebaWipe_Blur.bmp"), D3DXIFF_BMP, pMediaBlur->GetTexture(), NULL);
	//D3DXSaveSurfaceToFile(_T("./AmoebaWipe_Blur.bmp"), D3DXIFF_BMP, pMediaBlur->GetSurface(), NULL, NULL);

	Light(pMedia, pMediaBlur, pParam);

	//D3DXSaveTextureToFile(_T("./AmoebaWipe_Light.bmp"), D3DXIFF_BMP, pMedia->GetTexture(), NULL);
	//D3DXSaveSurfaceToFile(_T("./AmoebaWipe_Light.bmp"), D3DXIFF_BMP, pMedia->GetSurface(), NULL, NULL);

	blurParam.blurX = 100.0f / (4.0f + 28.0f * pParam->fBumpDensity) / (pParam->fHeight + 1.0f);
	blurParam.blurY = 100.0f / (4.0f + 28.0f * pParam->fBumpDensity) / (pParam->fHeight + 1.0f);
	CVideoBuffer* pMediaBlur2 = pBufMgr->CreateVideoBuffer(mediaBI);
	m_blurRender.Render(pMediaBlur2, pMedia, &blurParam);

	//D3DXSaveTextureToFile(_T("./AmoebaWipe_Blur_Second.bmp"), D3DXIFF_BMP, pMediaBlur2->GetTexture(), NULL);

	Last(pDest, pSrcA, pSrcB, pMediaBlur, pMediaBlur2, pParam);

	//D3DXSaveSurfaceToFile(_T("./AmoebaWipe_Last.bmp"), D3DXIFF_BMP, pDest->GetSurface(), NULL, NULL);

	pBufMgr->ReleaseVideoBuffer(pMediaBlur2);
	pBufMgr->ReleaseVideoBuffer(pMediaBlur);
	pBufMgr->ReleaseVideoBuffer(pMedia);

	return true;
}


bool CAmoebaWipeRender::CreateNoiseTexture( AmoebaWipeFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	//1.Create random bitmap
	if(pParam->nRandomSeed != m_nRandomSeed || pParam->fPhase != m_fPhase)
	{
		m_nRandomSeed = pParam->nRandomSeed;
		m_fPhase = pParam->fPhase;
		srand(m_nRandomSeed);

		float	fPhase = m_fPhase / 1800.0f * D3DX_PI * 2.0f;
		LPDIRECT3DTEXTURE9 pTex = m_pNoiseTexture->GetTexture();
		if (FAILED (D3DXFillTexture (pTex, ColorFill, &fPhase)))
		{
			return FALSE;
		}	
	}

	return true;
}


void CAmoebaWipeRender::Resize(CVideoBuffer* pDest, CVideoBuffer* pSrc, AmoebaWipeFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	//float	fAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;

	D3DXMATRIX* matWorld = NULL, *matView = NULL, *matProj= NULL;
	pResMan->GetQuadMatrix(&matWorld, &matView, &matProj);
	D3DXMATRIXA16 matCombine,matTex,matRotation,matTrans,matInvTrans,matAspect,matInvAspect;
	matCombine = *matView * *matProj;
	D3DXMatrixIdentity(&matTex);
	matTex._11 = (4.0f + 28.0f * pParam->fBumpDensity) / NOISE_BITMAP_SIZE;
	matTex._22 = (4.0f + 28.0f * pParam->fBumpDensity) / NOISE_BITMAP_SIZE;
	matTex._31 = 0.5f / NOISE_BITMAP_SIZE;
	matTex._32 = 0.5f / NOISE_BITMAP_SIZE;

	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matInvTrans);
	D3DXMatrixIdentity(&matAspect);
	D3DXMatrixIdentity(&matInvAspect);
	matTrans._31 = -0.5f;
	matTrans._32 = -0.5f;
	matInvTrans._31 = 0.5f;
	matInvTrans._32 = 0.5f;
	matAspect._11 *= fAspect;
	matInvAspect  /= 1.0f / fAspect;
	D3DXMatrixRotationZ(&matRotation,pParam->fSlant * D3DX_PI / 4.0f);
	matTex = matTrans * matAspect * matRotation * matInvAspect * matInvTrans * matTex;


	m_pEngine->SetRenderTarget(pDest);
	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	m_pEffect->SetMatrix("g_matTex",&matTex);
	m_pEffect->SetTexture("g_txColor",m_pNoiseTexture->GetTexture());
	m_pEffect->SetTechnique("Amoeba");	

	if(SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass;
		m_pEffect->Begin(&cPass,0);
		m_pEffect->BeginPass(0);
		m_pQuadMesh->DrawMeshFx();
		m_pEffect->EndPass();
		m_pEffect->End();
		pDevice->EndScene();
		m_pEngine->SetRenderTarget(NULL);
	}
	m_pEffect->SetTexture("g_txColor", NULL);
}

void CAmoebaWipeRender::Light( CVideoBuffer* pDest, CVideoBuffer* pSrc, AmoebaWipeFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	//float	fAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;

	D3DXMATRIX* matWorld = NULL, *matView = NULL, *matProj= NULL;
	pResMan->GetQuadMatrix(&matWorld, &matView, &matProj);
	D3DXMATRIXA16 matCombine,matTex,matRotation,matTrans,matInvTrans,matAspect,matInvAspect;
	matCombine = *matView * *matProj;

	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matInvTrans);
	D3DXMatrixIdentity(&matAspect);
	D3DXMatrixIdentity(&matInvAspect);
	matTrans._31 = -0.5f;
	matTrans._32 = -0.5f;
	matInvTrans._31 = 0.5f;
	matInvTrans._32 = 0.5f;
	matAspect._11 *= fAspect;
	matInvAspect  /= 1.0f / fAspect;
	D3DXMatrixRotationZ(&matRotation,pParam->fSlant * D3DX_PI / 4.0f);
	D3DXMatrixIdentity(&matTex);
	matTex._31 = 0.5f / nEditWidth;
	matTex._32 = 0.5f / nEditHeight;


	float fSoftEdge = pParam->fSoftEdge / 2.0f;
	float fHeight = pParam->fHeight / 10.0f;
	float	fCenter = pParam->fOffset * 2.0f - 1.0f;
	float	fSoftCenter = pParam->fOffset + fCenter * (fSoftEdge + fHeight);
	float	fHeightCenter = pParam->fOffset + fCenter * fHeight;

	D3DXVECTOR4 vMisc = D3DXVECTOR4(
		fSoftCenter - (fSoftEdge + fHeight),
		fSoftCenter + (fSoftEdge - fHeight),
		fHeightCenter - fHeight,
		fHeightCenter + fHeight);
	m_pEffect->SetVector("g_vMisc", &vMisc);

	const VideoBufferInfo& bi = pSrc->GetVideoBufferInfo();
	D3DXVECTOR4 vColorSize = D3DXVECTOR4(bi.nAllocWidth, bi.nAllocHeight, pParam->fBrightness * 20.0f, 0.0f);
	m_pEffect->SetVector("g_textureInfo",&vColorSize);
	float fLightAngle = D3DXToRadian(pParam->fLightAngle);
	D3DXVECTOR4 vLightDir = D3DXVECTOR4( sin(fLightAngle),-sin(fLightAngle),cos(fLightAngle),-cos(fLightAngle));	
	m_pEffect->SetVector("g_vLightDir",&vLightDir);		

	D3DXCOLOR vHighLight = D3DXCOLOR(pParam->crHColor);
	D3DXCOLOR vShadow = D3DXCOLOR(pParam->crLColor);

	//ColorConvertor::RGBA2(pSrcDef[0]->bufferFormat == FMT_RGBA32 ?  FMT_RGBA32 : FMT_YUVA32 ,&vHighLight,&vHighLight);
	//ColorConvertor::RGBA2(pSrcDef[0]->bufferFormat == FMT_RGBA32 ?  FMT_RGBA32 : FMT_YUVA32 ,&vShadow,&vShadow);

	m_pEffect->SetFloatArray("g_HighLight",(float*)&vHighLight,4);
	m_pEffect->SetFloatArray("g_Shadow",(float*)&vShadow,4);
	//m_pEffect->SetVector("g_HighLight",(D3DXVECTOR4*)&vHighLight);
	//m_pEffect->SetVector("g_Shadow",(D3DXVECTOR4*)&vShadow);

	m_pEngine->SetRenderTarget(pDest);
	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	m_pEffect->SetMatrix("g_matTex",&matTex);
	m_pEffect->SetTexture("g_txKey",pSrc->GetTexture());

	if(SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass;
		m_pEffect->Begin(&cPass,0);
		m_pEffect->BeginPass(1);
		m_pQuadMesh->DrawMeshFx();
		m_pEffect->EndPass();
		m_pEffect->End();
		pDevice->EndScene();
		m_pEngine->SetRenderTarget(NULL);
	}
	m_pEffect->SetTexture("g_txKey", NULL);
}

void CAmoebaWipeRender::Last( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, CVideoBuffer* pSrcC, CVideoBuffer* pSrcD, AmoebaWipeFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	//float	fAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;

	D3DXMATRIX* matWorld = NULL, *matView = NULL, *matProj= NULL;
	pResMan->GetQuadMatrix(&matWorld, &matView, &matProj);
	D3DXMATRIXA16 matCombine,matTex,matRotation,matTrans,matInvTrans,matAspect,matInvAspect;
	matCombine = *matView * *matProj;

	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matInvTrans);
	D3DXMatrixIdentity(&matAspect);
	D3DXMatrixIdentity(&matInvAspect);
	matTrans._31 = -0.5f;
	matTrans._32 = -0.5f;
	matInvTrans._31 = 0.5f;
	matInvTrans._32 = 0.5f;
	matAspect._11 *= fAspect;
	matInvAspect  /= 1.0f / fAspect;
	D3DXMatrixRotationZ(&matRotation,pParam->fSlant * D3DX_PI / 4.0f);
	D3DXMatrixIdentity(&matTex);
	matTex._31 = 0.5f / nEditWidth;
	matTex._32 = 0.5f / nEditHeight;

	//handle_tpr hTemp[2] = {INVALID_RESID,INVALID_RESID};
	//TP_VBufferDef * pTempDef[2] = {pSrcDef[0],pSrcDef[1]};
	//for(int i = 0; i< 2;i ++)
	//{
	//	if(pTempDef[i]->BaseWidth != pProfile->nEditWidth || pTempDef[i]->BaseHeight != pProfile->nEditHeight
	//		|| pTempDef[i]->rcImage.left != CEIL(pTempDef[i]->OffsetX) || pTempDef[i]->rcImage.top != CEIL(pTempDef[i]->OffsetY))
	//	{
	//		hTemp[i] = NewRTBuffer(pTempDef[i]->OffsetX,pTempDef[i]->OffsetY,pProfile->nEditWidth,pProfile->nEditHeight);
	//		m_pEngine->EffectVideoCopy(pSrcDef[i]->handle,hTemp[i]);
	//		pTempDef[i] = m_pResMan->GetBufferDef(hTemp[i]);
	//	}
	//}
	//Draw Out
	m_pEngine->SetRenderTarget(pDest);	

	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	m_pEffect->SetMatrix("g_matTex",&matTex);
	m_pEffect->SetTexture("g_txColor",pSrcA->GetTexture());
	m_pEffect->SetTexture("g_txColor1",pSrcB->GetTexture());
	m_pEffect->SetTexture("g_txKey",pSrcC->GetTexture());
	m_pEffect->SetTexture("g_txLight",pSrcD->GetTexture());
	//D3DXVECTOR4 vAlpha(pSrcDef[0]->fAlphaValue,pSrcDef[1]->fAlphaValue,0,0);
	D3DXVECTOR4 vAlpha(1.0f,1.0f,0,0);
	m_pEffect->SetVector("g_vAlpha",&vAlpha);	

	if(SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass;
		m_pEffect->Begin(&cPass,0);
		m_pEffect->BeginPass(2);
		m_pQuadMesh->DrawMeshFx();
		m_pEffect->EndPass();
		m_pEffect->End();
		pDevice->EndScene();
		m_pEngine->SetRenderTarget(NULL);
	}
	m_pEffect->SetTexture("g_txColor", NULL);
	m_pEffect->SetTexture("g_txColor1", NULL);
	m_pEffect->SetTexture("g_txKey", NULL);
	m_pEffect->SetTexture("g_txLight", NULL);
}

