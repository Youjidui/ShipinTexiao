#include "StdAfx.h"
#include "MatrixWipeRender.h"

CMatrixWipeRender::CMatrixWipeRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
	m_pWipe = NULL;
	m_nPattern = 10000;
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

bool CMatrixWipeRender::Render( CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam )
{

}

bool CMatrixWipeRender::RenderMask( CVideoBuffer*pMaskDef, BasicWipeFxParam* pParam )
{
	m_pWipe->Ready(pMaskDef);
	return m_pWipe->Draw(pMaskDef, pParam);
}

bool CMatrixWipeRender::Ready( CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam )
{
	if(INSIDE(pParam->structPattern.nPattern,0,15) && !INSIDE(m_nPattern,0,15))
	{
		m_nPattern = pParam->structPattern.nPattern;

		SAFE_DELETE(m_pWipe);
		m_pWipe = new CMatrixWipe(m_pEngine);
		m_pWipe->InitMesh(m_pEngine->GetDevice());
		//UpdateInstance();
	}
}
