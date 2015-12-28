#include "StdAfx.h"
#include "DiffuseWipeRender.h"
#include "../Utility/SafeDelete.h"
//#include "../D3D9Render/BaseFx.h"

CDiffuseWipeRender::CDiffuseWipeRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pDiffuseWipeEffect(NULL)
{
}

CDiffuseWipeRender::~CDiffuseWipeRender(void)
{
}

bool CDiffuseWipeRender::Init( CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(NULL != m_pQuadMesh);
	m_pDiffuseWipeEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/GDiffuseWipe.fx"));
	ASSERT(NULL != m_pDiffuseWipeEffect);
	return true;
}

bool CDiffuseWipeRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrc, DiffuseWipeFxParam* pFxParam)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	RESET_RENDER_TARGET(m_pEngine);

	DiffuseWipeFxParam* pParam = (DiffuseWipeFxParam*)pFxParam;

	if(pParam->nBlockWidth > 0 && pParam->nBlockHeight > 0)
	{

	}

	return true;
}

