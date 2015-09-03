#include "StdAfx.h"
#include "AmoebaWipeRender.h"



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
, m_nRandomSeed(0)
, m_fPhase(0.0f)
{
}

CAmoebaWipeRender::~CAmoebaWipeRender(void)
{
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	pBufMgr->ReleaseVideoBuffer(m_pNoiseTexture);
	//CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	//pResMgr->ReleaseEffect();
}


bool CAmoebaWipeRender::Init( CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/AmoebaWipeTrans.fx"));
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 1920, 1080, 0, 0};
	m_pNoiseTexture = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(m_pNoiseTexture);
	return true;
}

bool CAmoebaWipeRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, AmoebaWipeFxParam* pParam )
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
