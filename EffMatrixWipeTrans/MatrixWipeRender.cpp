#include "StdAfx.h"
#include "MatrixWipeRender.h"
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"

CMatrixWipeRender::CMatrixWipeRender(void)
{
}

CMatrixWipeRender::~CMatrixWipeRender(void)
{
	Uninit();
}

bool CMatrixWipeRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GMatrix_Mask.fx"));
	ASSERT(m_pEffect);

	return true;
}

void CMatrixWipeRender::Uninit()
{

}


bool CMatrixWipeRender::RenderMask( CVideoBuffer*pMaskDef, BasicWipeFxParam* pParam )
{
	ASSERT(m_privateData.m_pWipe);
	m_privateData.m_pWipe->Ready(pMaskDef);
	HRESULT hr = m_privateData.m_pWipe->Draw(pMaskDef, pParam);
	return SUCCEEDED(hr);
}

bool CMatrixWipeRender::Ready( CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam )
{
	if(INSIDE(pParam->structPattern.nPattern, 0, 15) && !INSIDE(m_privateData.m_nPattern, 0, 15))
	{
		m_privateData.m_nPattern = pParam->structPattern.nPattern;

		SAFE_DELETE(m_privateData.m_pWipe);
		m_privateData.m_pWipe = new CMatrixWipe;
		m_privateData.m_pWipe->Init(m_pEngine);
	}
	return NULL != m_privateData.m_pWipe;
}
