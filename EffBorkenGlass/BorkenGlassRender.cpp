#include "StdAfx.h"
#include "BorkenGlassRender.h"

CBorkenGlassRender::CBorkenGlassRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
}

CBorkenGlassRender::~CBorkenGlassRender(void)
{
	Uninit();
}

bool CBorkenGlassRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GBorkenGlassShader.fx"));
	ASSERT(m_pEffect);

	//return SUCCEEDED(hr);
	return true;
}

void CBorkenGlassRender::Uninit()
{

}

bool CBorkenGlassRender::Render( CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam )
{
	return true;
}
