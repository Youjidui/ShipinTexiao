#include "StdAfx.h"
#include "PushRender.h"
#include "../Utility/SafeDelete.h"

CPushRender::CPushRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pPushEffect(NULL)
{
}

CPushRender::~CPushRender(void)
{
}

bool CPushRender::Init(CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pPushEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Push.fx"));
	return true;
}

void CPushRender::Uninit()
{
}

bool CPushRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1,  CVideoBuffer* pSrc2, PushFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	RESET_RENDER_TARGET(m_pEngine);

	if (pParam->bReverse)
	{
		std::swap(pSrc1, pSrc2);
	}

	if(m_pEngine->SetRenderTarget(pDst))
	{
		//D3DVIEWPORT9 vPort;
		//pDevice->GetViewport(&vPort);

		int nEditWidth, nEditHeight;
		m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
		const VideoBufferInfo& biSrc1 = pSrc1->GetVideoBufferInfo();
		const float pSrcDef0_OffsetX = 0.f, pSrcDef0_OffsetY = 0.f;
		//非满屏定位开始
		//D3DXMATRIX matScale, matTransition;
		//float fxZoom = biSrc1.nWidth / (float)vPort.Width;
		//float fyZoom = biSrc1.nHeight / (float)vPort.Height;
		//float ofx = -0.5f + fxZoom*0.5f + pSrcDef0_OffsetX / nEditWidth;
		//float ofy =  0.5f - fyZoom*0.5f - pSrcDef0_OffsetY / nEditHeight;
		//D3DXMatrixScaling(&matScale, fxZoom, fyZoom , 1.0f);
		//D3DXMatrixTranslation(&matTransition, ofx, ofy, 0.0f);	
		//非满屏定位结束
		
		D3DXMATRIXA16 matCombined,matWorld,matWorld1;
		D3DXMATRIXA16 matTextureSrc;
		D3DXMatrixIdentity(&matTextureSrc);
		GenerateMatrix(pSrc1, &matTextureSrc,mat_Image);
		GetWorldMatrix(&matWorld,&matWorld1,pParam->dwType,pParam->fTransition);
		LPD3DXMATRIX matView = NULL, matProj= NULL;
		pResMan->GetPerspectiveMatrix(&matView, &matProj);

		//matCombined = matScale * matTransition * matWorld * *matView * *matProj;
		matCombined = matWorld * *matView * *matProj;
		
		D3DXCOLOR FirstColor;
		FirstColor.b = (pParam->dwFirstColor & 0xFF)/255.0f;
		FirstColor.g = ((pParam->dwFirstColor>>8) & 0xFF)/255.0f;
		FirstColor.r = ((pParam->dwFirstColor>>16) & 0xFF)/255.0f;
		FirstColor.a = ((pParam->dwFirstColor>>24) & 0xFF)/255.0f;
		D3DXCOLOR SecondColor;
		SecondColor.b = (pParam->dwSecondColor & 0xFF)/255.0f;
		SecondColor.g = ((pParam->dwSecondColor>>8) & 0xFF)/255.0f;
		SecondColor.r = ((pParam->dwSecondColor>>16) & 0xFF)/255.0f;
		SecondColor.a = ((pParam->dwSecondColor>>24) & 0xFF)/255.0f;
		//D3DXCOLOR vFirstColor;
		//D3DXCOLOR vSecondColor;
		//m_pEngine->ColorConv_RGBA_YUVA(&FirstColor,  &vFirstColor);
		//m_pEngine->ColorConv_RGBA_YUVA(&SecondColor, &vSecondColor);


		//add by szm 2011/12/1 软边宽高比不对
		float fAspect = nEditWidth*1.0f / nEditHeight;
		const int nGetAspectVerifyCoef = 1;	//m_pProfile->bInterval ? 2 : 1;
		fAspect = fAspect * biSrc1.nWidth / (float) (biSrc1.nHeight * nGetAspectVerifyCoef); // 获得屏幕宽高比
		fAspect = 1.f / fAspect;
		m_pPushEffect->SetFloat("g_fAspect", fAspect);
		//end by szm 2011/12/1

		m_pPushEffect->SetFloat("g_fAlpha",/*ppSrcDef[0]->fAlphaValue*/ 1.0f);	//TODO: alpha
		m_pPushEffect->SetMatrix("g_matTexture",&matTextureSrc);
		m_pPushEffect->SetTechnique("Picture");

		m_pPushEffect->SetMatrix("g_matWorldViewProj",&matCombined);
		m_pPushEffect->SetTexture("g_txColor",pSrc1->GetTexture());
		m_pPushEffect->SetFloat("fSoftness",pParam->fFirstSoftness);
		m_pPushEffect->SetVector("vColor",(D3DXVECTOR4*)&FirstColor);

		// begin&end scene
		UINT cPass;
		if ( SUCCEEDED(pDevice->BeginScene()))
		{

			m_pPushEffect->Begin(&cPass,0);
			m_pPushEffect->BeginPass(0);
			m_pQuadMesh-> DrawMeshFx();
			m_pPushEffect->EndPass();
			m_pPushEffect->End();
			pDevice->EndScene();
		}

		const VideoBufferInfo& biSrc2 = pSrc2->GetVideoBufferInfo();

		//非满屏定位开始
		//fxZoom = biSrc2.nWidth / (float)vPort.Width;
		//fyZoom = biSrc2.nHeight / (float)vPort.Height;
		//ofx = -0.5f + fxZoom*0.5f + pSrcDef0_OffsetX/ (float)nEditWidth;
		//ofy =  0.5f - fyZoom*0.5f - pSrcDef0_OffsetY / (float)nEditHeight;
		//D3DXMatrixScaling(&matScale, fxZoom, fyZoom , 1.0f);
		//D3DXMatrixTranslation(&matTransition, ofx, ofy, 0.0f);	
		//非满屏定位结束

		GenerateMatrix(pSrc1, &matTextureSrc,mat_Image);
		//matCombined= matScale * matTransition * matWorld1*matWorld * *matView * *matProj;
		matCombined= matWorld1*matWorld * *matView * *matProj;

		m_pPushEffect->SetMatrix("g_matWorldViewProj",&matCombined);
		m_pPushEffect->SetTexture("g_txColor",pSrc2->GetTexture());
		m_pPushEffect->SetFloat("fSoftness",pParam->fSecondSoftness);
		m_pPushEffect->SetVector("vColor",(D3DXVECTOR4*)&SecondColor);	
		m_pPushEffect->SetMatrix("g_matTexture",&matTextureSrc);

		//add by szm 2011/12/1 软边宽高比不对
		fAspect = nEditWidth*1.0f / nEditHeight;
		//fAspect = m_pResMan->GetAspect() * pSrcDef1->GetImageWidth() / (float) (pSrcDef1->GetImageHeight() * m_pResMan->GetAspectVerifyCoef()); // 获得屏幕宽高比
		fAspect = fAspect * biSrc2.nWidth / (float) (biSrc2.nHeight * nGetAspectVerifyCoef); // 获得屏幕宽高比
		fAspect = 1.f / fAspect;
		m_pPushEffect->SetFloat("g_fAspect",fAspect);
		//end by szm 2011/12/1

		// begin&end scene
		if ( SUCCEEDED(pDevice->BeginScene()))
		{

			m_pPushEffect->Begin(&cPass,0);
			m_pPushEffect->BeginPass(0);
			m_pQuadMesh-> DrawMeshFx();
			m_pPushEffect->EndPass();
			m_pPushEffect->End();
			pDevice->EndScene();
		}
		// dest标志位设定
		//pDstDef->bContainedAlpha = true;
		m_pPushEffect->SetTexture("g_txColor",NULL);
	}

	return true;
}

void CPushRender::GetWorldMatrix(D3DXMATRIX * pmat0, D3DXMATRIX *pmat1, DWORD dwType, float fTranstion)
{
	assert(pmat0);
	assert(pmat1);
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
	D3DXMatrixTranslation(pmat0,fTransX,fTransY,0.f);
	float fTransX1 = -vecDir.x;
	float fTransY1 = -vecDir.y;
	D3DXMatrixTranslation(pmat1,fTransX1,fTransY1,0.f);
}