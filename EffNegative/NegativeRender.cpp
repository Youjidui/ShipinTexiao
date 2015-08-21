#include "StdAfx.h"
#include "NegativeRender.h"
#include "../Utility/SafeDelete.h"
//#include "../D3D9Render/BaseFx.h"

CNegativeRender::CNegativeRender(void)
: m_pDevice(NULL)
, m_pResMan(NULL)
, m_pQuadMesh(NULL)
, m_pNegativeEffect(NULL)
, m_matWorld(NULL)
, m_matPrj(NULL)
, m_matView(NULL)
{
	m_matWorld    = new D3DXMATRIXA16;
	m_matView     = new D3DXMATRIXA16;
	m_matPrj      = new D3DXMATRIXA16;
}

CNegativeRender::~CNegativeRender(void)
{
	delete m_matPrj;
	delete m_matView;
	delete m_matWorld;
}

bool CNegativeRender::Init( LPDIRECT3DDEVICE9 pDevice, CResourceManager* pResMan)
{
	m_pDevice = pDevice;
	m_pResMan = pResMan;
	m_pQuadMesh = m_pResMan->CreateQuadMesh(pDevice);
	//m_pProcessMaskEffect = m_pResMan->CreateEffect(_T("GProcessMask.fxo"));
	//m_pDirectOutEffect = m_pResMan->CreateEffect(_T("fx_GDirectOut.fxo"));
	m_pNegativeEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/fx_GNegative.fx"));
	return true;
}

bool CNegativeRender::Render( CVideoBuffer* pSrc, CVideoBuffer* pDest, NegativeFxParam* pParam )
{
	if(SetRenderTarget(pDest))
	{
		const VideoBufferInfo& srcBuffInfo = pSrc->GetVideoBufferInfo();
		D3DXMATRIX	matImage;
		D3DXMatrixIdentity(&matImage);
		float m_fu =  0.5f/(float)(srcBuffInfo.nWidth);
		float m_fv =  0.5f/(float)(srcBuffInfo.nHeight);

		D3DXMATRIX  *pMatWorld, *pMatView, *pMatProj;
		GetQuadMatrix(&pMatWorld, &pMatView, &pMatProj);
		D3DXMATRIX matWVP = *pMatWorld * (*pMatView)* (*pMatProj);

		LPDIRECT3DTEXTURE9 lpTex = NULL;
		if(SUCCEEDED(m_pDevice->CreateTexture(srcBuffInfo.nWidth, srcBuffInfo.nHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &lpTex, NULL)))
		{
			LPDIRECT3DSURFACE9 lpTexSurf = NULL;
			if(SUCCEEDED(lpTex->GetSurfaceLevel(0, &lpTexSurf)))
			{
				if(SUCCEEDED(m_pDevice->UpdateSurface(pSrc->GetSurface(), NULL, lpTexSurf, NULL)))
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

					if ( SUCCEEDED(m_pDevice->BeginScene()) )
					{
						m_pNegativeEffect->Begin(&cPass, 0);
						m_pNegativeEffect->BeginPass(uPass);
						m_pQuadMesh->DrawMeshFx();
						m_pNegativeEffect->EndPass();
						m_pNegativeEffect->End();
						m_pDevice->EndScene();
					}
				}
				else
				{
					//error info
				}
			}
		}
	}
	return true;
}

bool CNegativeRender::SetRenderTarget( CVideoBuffer* pDest )
{
	LPDIRECT3DSURFACE9 pRTSurface = NULL;
	pRTSurface = pDest->GetSurface();
	// set render target
	m_pDevice->SetRenderTarget(0, pRTSurface);
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
	m_pDevice->SetViewport(&vp);

	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	return true;
}

void CNegativeRender::GetQuadMatrix( D3DXMATRIX** ppMatWorld, D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj )
{
	D3DXMatrixIdentity( m_matWorld );
	D3DXMatrixIdentity( m_matView  );
	D3DXMatrixOrthoLH( m_matPrj, 1.0, 1.0,0.1f, 100.0f );

	D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -100.0f); 
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
	D3DXMatrixLookAtLH( m_matView, &vFromPt, &vLookatPt, &vUpVec );

	*ppMatWorld = m_matWorld;
	*ppMatView = m_matView;
	*ppMatPrj = m_matPrj;
}
