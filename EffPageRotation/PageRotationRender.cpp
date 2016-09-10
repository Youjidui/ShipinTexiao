#include "StdAfx.h"
#include "PageRotationRender.h"

#pragma warning(disable:4244)

CPageRotationRender::CPageRotationRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
}

CPageRotationRender::~CPageRotationRender(void)
{
	Uninit();
}

bool CPageRotationRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/PageRotation.fx"));
	ASSERT(m_pEffect);

	//return SUCCEEDED(hr);
	return true;
}

void CPageRotationRender::Uninit()
{
}

bool CPageRotationRender::Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParamRaw)
{
	PageRotationFxParam* pParam = (PageRotationFxParam*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	if(pParam->structTrans.bReverse)
		std::swap(pSrcA, pSrcB);

	//CBaseTexture * pMipMap[2] = {NULL,NULL},*pYUVA = NULL;
	CVideoBuffer* pMipMap[2] = {NULL,NULL};
	//pMipMap[0] = m_pResMan->GetTemp_Video(0,TRUE);	
	//pMipMap[1] = m_pResMan->GetTemp_Video(1,TRUE);
	// not use mipmap
	//VideoBufferInfo mipMapInfo = pSrcA->GetVideoBufferInfo();
	//mipMapInfo.eType = VideoBufferInfo::VIDEO_MEM;
	//mipMapInfo.eUsage = VideoBufferInfo::_IN_OUT_WITH_MIPMAP;
	//pMipMap[0] = pVM->CreateVideoBuffer(mipMapInfo);
	//pMipMap[1] = pVM->CreateVideoBuffer(mipMapInfo);

	//GenerateMipMap(pSrcDef[0],pMipMap[0],TRUE);
	//GenerateMipMap(pSrcDef[1],pMipMap[1],TRUE);
	pMipMap[0] = pSrcA;
	pMipMap[1] = pSrcB;

	D3DXMATRIXA16 matProj,matWorld,matAspect,matCombine,matTex,matShift;
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float fAspect = nEditWidth  / (float)(nEditHeight);

	D3DXMatrixScaling(&matAspect,fAspect,1.0f,1.0f);
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 0.01f, 1000.0f );

	D3DXMatrixIdentity(&matTex);
	const VideoBufferInfo& biSrc = pMipMap[0]->GetVideoBufferInfo();
	//matTex._11 = nEditWidth * 1.0f / biSrc.nAllocWidth;
	//matTex._22 = nEditHeight * 1.0f / biSrc.nAllocHeight;
	matTex._11 = biSrc.nWidth * 1.0f / biSrc.nAllocWidth;
	matTex._22 = biSrc.nHeight * 1.0f / biSrc.nAllocHeight;
	matTex._31 = 0.5f / biSrc.nAllocWidth;
	//if(pParam->bOdd)
	//	matTex._32 = 0.5f / biSrc.nAllocHeight;
	if(pParam->nPattern == 0)
		D3DXMatrixRotationX(&matWorld,-D3DX_PI * pParam->structTrans.fTransition);
	else
		D3DXMatrixRotationY(&matWorld,-D3DX_PI * pParam->structTrans.fTransition);

	D3DXMatrixIdentity(&matShift);
	//if(!pParam->bOdd)
	//	D3DXMatrixTranslation(&matShift,0.0f,0.5f / nEditHeight,0.0f);

	D3DXMATRIX* pMatView, *pMatProj;
	pResMan->GetPerspectiveMatrix(&pMatView, &pMatProj);
	matCombine = matAspect * matWorld * matShift * (*pMatView) * matProj;

	float fAlphaValue = 1.f;
	D3DXVECTOR4 vMisc(fAlphaValue,fAlphaValue,0.0f,0.0f);

	m_pEffect->SetTechnique("PageRotation");
	m_pEffect->SetTexture("g_txColor",pMipMap[0]->GetTexture());
	m_pEffect->SetTexture("g_txColor1",pMipMap[1]->GetTexture());
	m_pEffect->SetInt("g_nPattern",pParam->nPattern);
	m_pEffect->SetMatrix("g_matTex",&matTex);
	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	m_pEffect->SetVector("g_vMisc",&vMisc);

	m_pEngine->SetRenderTarget(pDest);
	if( SUCCEEDED( pDevice->BeginScene()))
	{
		UINT cPass,uPass = 3;
		m_pEffect->Begin(&cPass,0);
		for(uPass=0;uPass<cPass;uPass++)
		{
			m_pEffect->BeginPass(uPass);
			if(uPass == 2)
			{
				matCombine = matAspect * matShift * (*pMatView) * matProj;
				m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
				m_pEffect->CommitChanges();
			}
			m_pQuadMesh->DrawMeshFx();

			m_pEffect->EndPass();
		}
		m_pEffect->End();

		// End the scene
		pDevice->EndScene();
	}

	return true;
}

