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
	m_pNegativeEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/fx_GNegative.fx"));
	return true;
}

bool CNegativeRender::Render( CVideoBuffer* pSrc, CVideoBuffer* pDest, NegativeFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	if(SetRenderTarget(pDest))
	{
		const VideoBufferInfo& srcBuffInfo = pSrc->GetVideoBufferInfo();
		D3DXMATRIX	matImage;
		D3DXMatrixIdentity(&matImage);
		float m_fu =  0.5f/(float)(srcBuffInfo.nWidth);
		float m_fv =  0.5f/(float)(srcBuffInfo.nHeight);

		D3DXMATRIX  *pMatWorld, *pMatView, *pMatProj;
		pResMan->GetQuadMatrix(&pMatWorld, &pMatView, &pMatProj);
		D3DXMATRIX matWVP = *pMatWorld * (*pMatView)* (*pMatProj);

		//目前需要copy一次，从memory到video memory的texture
		//这个应该在将来去掉，以提高性能
		LPDIRECT3DTEXTURE9 lpTex = CreateTexture(pDevice, pSrc);
		if(lpTex)
		{
			m_pNegativeEffect->SetTexture("g_txColor", lpTex);
			m_pNegativeEffect->SetMatrix("g_matWorldViewProj",&matWVP);
			m_pNegativeEffect->SetMatrix("g_matTexture",&matImage);
			m_pNegativeEffect->SetTechnique("Picture");	

			UINT cPass = 0,uPass = 0;
			switch(srcBuffInfo.format)
			{
			case FMT_RGBA32:uPass = 0;break;
			case FMT_YUVA32:uPass = 1;break;
			case FMT_YUYV:uPass = 2;break;
			}

			if ( SUCCEEDED(pDevice->BeginScene()) )
			{
				m_pNegativeEffect->Begin(&cPass, 0);
				m_pNegativeEffect->BeginPass(uPass);
				m_pQuadMesh->DrawMeshFx();
				m_pNegativeEffect->EndPass();
				m_pNegativeEffect->End();
				pDevice->EndScene();
			}
			lpTex->Release();
			lpTex = NULL;
			m_pNegativeEffect->SetTexture("g_txColor", lpTex);
		}
	}
	return true;
}

bool CNegativeRender::SetRenderTarget( CVideoBuffer* pDest )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	LPDIRECT3DSURFACE9 pRTSurface = NULL;
	pRTSurface = pDest->GetSurface();
	// set render target
	pDevice->SetRenderTarget(0, pRTSurface);
	SAFE_RELEASE(pRTSurface);

	const VideoBufferInfo& buffInfo = pDest->GetVideoBufferInfo();
	// set view port
	D3DVIEWPORT9 vp;
	vp.MaxZ = 1.0f;
	vp.MinZ = 0.0f;
		vp.X        = 0;
		vp.Y        = 0;
		vp.Width    = buffInfo.nWidth;
		vp.Height   = buffInfo.nHeight;
	pDevice->SetViewport(&vp);

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	return true;
}


