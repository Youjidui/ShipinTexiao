#include "StdAfx.h"
#include "BufferColorConvertor.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/mathmacros.h"

CBufferColorConvertor::CBufferColorConvertor(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
}

CBufferColorConvertor::~CBufferColorConvertor(void)
{
	Uninit();
}

bool CBufferColorConvertor::Init( CRenderEngine* pEngine )
{
	HRESULT hr = S_OK;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/YUVA2RGBA.fx"));
	ASSERT(m_pEffect);
	hr = m_pEffect->SetYUVA2RGBAMatrix(1);
	ASSERT(SUCCEEDED(hr));

	return SUCCEEDED(hr);
}

void CBufferColorConvertor::Uninit()
{

}

bool CBufferColorConvertor::Render( CVideoBuffer* pDest, CVideoBuffer *pSrc, FxParamBase* pParamBase)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	RESET_RENDER_TARGET(m_pEngine);

	ColorConvertFxParam* pParam = (ColorConvertFxParam*)pParamBase;
	if(m_pEngine->SetRenderTarget(pDest))
	{
		//bool bOK = m_pEngine->SetDepthStencilBuffer(false);
		//ASSERT(bOK);
		const VideoBufferInfo& srcBuffInfo = pSrc->GetVideoBufferInfo();
		D3DXMATRIX	matImage;
		D3DXMatrixIdentity(&matImage);
		float m_fu =  0.5f/(float)(srcBuffInfo.nWidth);
		float m_fv =  0.5f/(float)(srcBuffInfo.nHeight);

		D3DXMATRIX  matWorld, *pMatView = NULL, *pMatProj = NULL;
		D3DXMatrixIdentity(&matWorld);
		pResMan->GetOrthoMatrix(&pMatView, &pMatProj);
		D3DXMATRIX matWVP = matWorld * (*pMatView)* (*pMatProj);

		LPDIRECT3DTEXTURE9 lpTex = pSrc->GetTexture();
		if(lpTex)
		{
			hr = m_pEffect->SetTexture("g_txColor", lpTex);
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matWVP);
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->SetMatrix("g_matTexture",&matImage);
			ASSERT(SUCCEEDED(hr));
			switch(pParam->convert_dir)
			{
			case ColorConvertFxParam::RGBA2YUVA:
				hr = m_pEffect->SetTechnique("RGBA2YUVA");
				break;
			case ColorConvertFxParam::YUVA2RGBA:
				hr = m_pEffect->SetTechnique("YUVA2RGBA");
				break;
			}
			ASSERT(SUCCEEDED(hr));

			UINT cPass = 0,uPass = 0;
			if ( SUCCEEDED(hr = pDevice->BeginScene()) )
			{
				hr = m_pEffect->Begin(&cPass, 0);
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->BeginPass(uPass);
				ASSERT(SUCCEEDED(hr));
				hr = m_pQuadMesh->DrawMeshFx();
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->EndPass();
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->End();
				ASSERT(SUCCEEDED(hr));
				hr = pDevice->EndScene();
				ASSERT(SUCCEEDED(hr));
			}
			hr = m_pEffect->SetTexture("g_txColor", NULL);
			ASSERT(SUCCEEDED(hr));
		}
	}
	else
	{
		ASSERT(false);
	}
	return true;
}
