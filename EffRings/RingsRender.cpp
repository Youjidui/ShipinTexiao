#include "StdAfx.h"
#include "RingsRender.h"
#include "../Utility/SafeDelete.h"
//#include "../D3D9Render/BaseFx.h"

CRingsRender::CRingsRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pNegativeEffect(NULL)
{
}

CRingsRender::~CRingsRender(void)
{
}

bool CRingsRender::Init( CRenderEngine* pEngine)
{
	//m_pEngine = pEngine;
	//LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	//CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	//m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	//ASSERT(NULL != m_pQuadMesh);
	//m_pNegativeEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Negative.fx"));
	//ASSERT(NULL != m_pNegativeEffect);
	return true;
}

bool CRingsRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrc, FxParamBase* )
{
	
	return true;
}

