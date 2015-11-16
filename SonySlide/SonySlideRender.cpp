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
	m_pSonySliderEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/SonySlide.fx"));
	return true;
}

void CSonySlideRender::Uninit()
{

}

bool CSonySlideRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1,  CVideoBuffer* pSrc2, SonySlideFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	RESET_RENDER_TARGET(m_pEngine);

	if (pParam->bReverse)
	{
		std::swap(pSrc1, pSrc2);
	}

	CVideoBuffer* pTempDef = m_pEngine->CreateRenderTargetBuffer();//CreateVideoBuffer(pDst->GetVideoBufferInfo());

	if(m_pEngine->SetRenderTarget(pTempDef))
	{
		//const Custom_Profile *pProfile = m_pEngine->GetCurProfile();

		D3DVIEWPORT9 vPort;
		pDevice->GetViewport(&vPort);
		//vPort.Width = pProfile->nEditWidth * (pSrcDef->IsYUV16Buffer()?0.5f:1.0f);
		//vPort.Height = pProfile->nEditHeight;
		//m_pDevice->SetViewport(&vPort);

		int nEditWidth, nEditHeight;
		m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
		const float pSrcDef0_OffsetX = 0.f, pSrcDef0_OffsetY = 0.f;

		D3DXMATRIX matScale, matTransition;
		const VideoBufferInfo& biSrc1 = pSrc1->GetVideoBufferInfo();
		float fxZoom = biSrc1.nWidth / (float)vPort.Width;
		float fyZoom = biSrc1.nHeight / (float)vPort.Height;

		float ofx = -0.5f + fxZoom*0.5f + pSrcDef0_OffsetX / nEditWidth;
		float ofy =  0.5f - fyZoom*0.5f - pSrcDef0_OffsetY / nEditHeight;

		D3DXMatrixScaling(&matScale, fxZoom, fyZoom , 1.0f);
		D3DXMatrixTranslation(&matTransition, ofx, ofy, 0.0f);

		D3DXMATRIX matCombined ,matWorld;

		D3DXMATRIX matTextureSrc;
		GenerateMatrix(pSrc1, &matTextureSrc, mat_Image);

		GetWorldMatrix(&matWorld, pParam->dwType, pParam->fTransition);
		LPD3DXMATRIX matView = NULL, matProj= NULL;

		pResMan->GetPerspectiveMatrix(&matView, &matProj);
		matCombined = matScale * matTransition * matWorld * *matView * *matProj;

		m_pSonySliderEffect->SetMatrix("g_matWorldViewProj",&matCombined);
		m_pSonySliderEffect->SetMatrix("g_matTexture",&matTextureSrc);	
		m_pSonySliderEffect->SetFloat("g_fAlpha",1.0f); // TODO: alpha

		m_pSonySliderEffect->SetTexture("g_txColor", pSrc1->GetTexture());
		if( /*pSrcDef->pAlpha*/ false) //TODO
		{
			m_pSonySliderEffect->SetTexture("g_txAlpha", pSrc1->GetTexture());
		}

		UINT cPass = 0, uPass = 0;
		switch(biSrc1.format)
		{
		case FMT_RGBA32:
		case FMT_YUVA32:
			uPass = 0;
			break;
		case FMT_YUYV:
			if(/*pDstDef->pAlpha*/false) // TODO
			{
				if(/*pSrcDef->pAlpha*/false)
					uPass = 2;
				else
					uPass = 3;
			}
			else
			{
				uPass = 1;
			}
			break;
		}

		m_pSonySliderEffect->SetTechnique("Picture");

		if ( SUCCEEDED(pDevice->BeginScene()))
		{
			//pDevice->ColorFill(pTempDef->GetSurface(), NULL, 0);
			m_pSonySliderEffect->Begin(&cPass,0);
			//	for(;uPass<cPass;uPass++)
			//	{
			m_pSonySliderEffect->BeginPass(uPass);
			m_pQuadMesh-> DrawMeshFx();
			m_pSonySliderEffect->EndPass();
			//	}
			m_pSonySliderEffect->End();

			pDevice->EndScene();
		}
		m_pSonySliderEffect->SetTexture("g_txColor", NULL);
		m_pSonySliderEffect->SetTexture("g_txAlpha", NULL);
	}
	//D3DXSaveSurfaceToFile(_T("./SonySlide_temp.dds"), D3DXIFF_DDS, pTempDef->GetSurface(), NULL, NULL);

	m_pEngine->BlendCompose(pDst, pSrc2, pTempDef);
	//BlendTwoBuffer(pSrc1, pSrc2, pDst);
	m_pEngine->GetVideoBufferManager()->ReleaseVideoBuffer(pTempDef);
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
