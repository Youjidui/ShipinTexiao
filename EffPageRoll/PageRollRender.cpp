#include "StdAfx.h"
#include "PageRollRender.h"

CPageRollRender::CPageRollRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
{
}

CPageRollRender::~CPageRollRender(void)
{
}

bool CPageRollRender::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);

	return true;
}

void CPageRollRender::Uninit()
{

}

bool CPageRollRender::Render(CVideoBuffer*pDstDef, CVideoBuffer *pSrcDef, FxParamBase* pParam)
{
	return false;
}


