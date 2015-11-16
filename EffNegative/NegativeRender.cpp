#include "StdAfx.h"
#include "NegativeRender.h"
#include "../Utility/SafeDelete.h"
//#include "../D3D9Render/BaseFx.h"

CNegativeRender::CNegativeRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pNegativeEffect(NULL)
{
}

CNegativeRender::~CNegativeRender(void)
{
}

bool CNegativeRender::Init( CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(NULL != m_pQuadMesh);
	m_pNegativeEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Negative.fx"));
	ASSERT(NULL != m_pNegativeEffect);
	return true;
}

bool CNegativeRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrc, NegativeFxParam* pParam )
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	RESET_RENDER_TARGET(m_pEngine);

	if(m_pEngine->SetRenderTarget(pDest))
	{
		const VideoBufferInfo& srcBuffInfo = pSrc->GetVideoBufferInfo();
		D3DXMATRIX	matImage;
		D3DXMatrixIdentity(&matImage);
		float m_fu =  0.5f/(float)(srcBuffInfo.nWidth);
		float m_fv =  0.5f/(float)(srcBuffInfo.nHeight);

		D3DXMATRIX  matWorld, *pMatView = NULL, *pMatProj = NULL;
		pResMan->GetOrthoMatrix(&pMatView, &pMatProj);
		D3DXMATRIX matWVP = matWorld * (*pMatView)* (*pMatProj);

		//目前需要copy一次，从memory到video memory的texture
		//这个应该在将来去掉，以提高性能
		//LPDIRECT3DTEXTURE9 lpTex = CreateTexture(pDevice, pSrc);
		LPDIRECT3DTEXTURE9 lpTex = pSrc->GetTexture();
		if(lpTex)
		{
			hr = m_pNegativeEffect->SetTexture("g_txColor", lpTex);
			ASSERT(SUCCEEDED(hr));
			hr = m_pNegativeEffect->SetMatrix("g_matWorldViewProj",&matWVP);
			ASSERT(SUCCEEDED(hr));
			hr = m_pNegativeEffect->SetMatrix("g_matTexture",&matImage);
			ASSERT(SUCCEEDED(hr));
			hr = m_pNegativeEffect->SetTechnique("Picture");	
			ASSERT(SUCCEEDED(hr));

			UINT cPass = 0,uPass = 0;
			switch(srcBuffInfo.format)
			{
			case FMT_RGBA32:uPass = 0;break;
			case FMT_YUVA32:uPass = 1;break;
			case FMT_YUYV:uPass = 2;break;
			}

			if ( SUCCEEDED(hr = pDevice->BeginScene()) )
			{
				hr = m_pNegativeEffect->Begin(&cPass, 0);
				ASSERT(SUCCEEDED(hr));
				hr = m_pNegativeEffect->BeginPass(uPass);
				ASSERT(SUCCEEDED(hr));
				hr = m_pQuadMesh->DrawMeshFx();
				ASSERT(SUCCEEDED(hr));
				hr = m_pNegativeEffect->EndPass();
				ASSERT(SUCCEEDED(hr));
				hr = m_pNegativeEffect->End();
				ASSERT(SUCCEEDED(hr));
				hr = pDevice->EndScene();
				ASSERT(SUCCEEDED(hr));
			}
			hr = m_pNegativeEffect->SetTexture("g_txColor", NULL);
			ASSERT(SUCCEEDED(hr));
			lpTex->Release();
		}
	}
	else
	{
		ASSERT(false);
	}
	return true;
}

