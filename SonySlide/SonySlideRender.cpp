#include "StdAfx.h"
#include "SonySlideRender.h"

CSonySlideRender::CSonySlideRender(void)
{
}

CSonySlideRender::~CSonySlideRender(void)
{
}

bool CSonySlideRender::Init(CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pSonySliderEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Negative.fx"));
	return true;
}

void CSonySlideRender::Uninit()
{

}

bool CSonySlideRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1,  CVideoBuffer* pSrc2, SonySlideFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	if (pParam->bReverse)
	{
		std::swap(pSrc1, pSrc2);
	}

	CVideoBuffer* pTempDef = m_pEngine->CreateRenderTargetBuffer();//CreateVideoBuffer(pDst->GetVideoBufferInfo());

	if(m_pEngine->SetRenderTarget(pDst))
	{
		//const Custom_Profile *pProfile = m_pEngine->GetCurProfile();

		D3DVIEWPORT9 vPort;
		pDevice->GetViewport(&vPort);
		//vPort.Width = pProfile->nEditWidth * (pSrcDef->IsYUV16Buffer()?0.5f:1.0f);
		//vPort.Height = pProfile->nEditHeight;
		//m_pDevice->SetViewport(&vPort);

		D3DXMATRIX matScale, matTransition;
		const VideoBufferInfo& biSrc1 = pSrc1->GetVideoBufferInfo();
		float fxZoom = biSrc1.nWidth / (float)vPort.Width;
		float fyZoom = biSrc1.nHeight / (float)vPort.Height;

		

	}

	return true;
}


void CSonySlideRender::GetWorldMatrix(D3DXMATRIX * mat,DWORD dwType,float fTranstion)
{
	assert(mat);
	D3DXVECTOR2 vecDir(0,0);
	switch(dwType)
	{
	case Right2Left:  vecDir = D3DXVECTOR2(-1.f,0.f);
		break;
	case Left2Right:  vecDir = D3DXVECTOR2(1.f,0.f);
		break;
	case Bottom2Top:  vecDir = D3DXVECTOR2(0.f,1.f);
		break;  
	case Top2Bottom:  vecDir = D3DXVECTOR2(0.f,-1.f);
		break;
	case BottomRight: vecDir = D3DXVECTOR2(1.f,-1.f);
		break;
	case BottomLeft:  vecDir = D3DXVECTOR2(-1.f,-1.f);
		break;
	case TopLeft:     vecDir = D3DXVECTOR2(-1.f,1.f);
		break;
	case TopRight:    vecDir = D3DXVECTOR2(1.f,1.f);
		break;
	default:  
		return;
	}
	float fTransX = fTranstion * vecDir.x;
	float fTransY = fTranstion * vecDir.y;
	D3DXMatrixTranslation(mat,fTransX,fTransY,0.f);
}