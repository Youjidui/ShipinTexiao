#include "StdAfx.h"
#include "RevolvingWipeRender.h"
#include "SingleRevolvWipe.h"
#include "DoubleRevolvWipe.h"
#include "QuadrantRevolvWipe.h"

CRevolvingWipeRender::CRevolvingWipeRender(void)
{
}

CRevolvingWipeRender::~CRevolvingWipeRender(void)
{
	Uninit();
}

bool CRevolvingWipeRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;

	bool bOK = CWipeRenderBase::Init(pEngine);
	ASSERT(bOK);

	return bOK;
}

void CRevolvingWipeRender::Uninit()
{
	CWipeRenderBase::Uninit();
}


bool CRevolvingWipeRender::RenderMask( CVideoBuffer*pMaskDef, BasicWipeFxParam* pParam )
{
	ASSERT(m_privateData.m_pWipe);
	m_privateData.m_pWipe->Ready(pMaskDef);
	HRESULT hr = m_privateData.m_pWipe->Draw(pMaskDef, pParam);
	return SUCCEEDED(hr);
}


bool CRevolvingWipeRender::Ready( CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam )
{
	if(InitInstanceByPattern<CSingleRevolvWipe>(pParam->structPattern.nPattern, 0, 3))
		0;
	else if(InitInstanceByPattern<CDoubleRevolvWipe>(pParam->structPattern.nPattern, 4, 5))
		0;
	else if(InitInstanceByPattern<CQuadrantRevolvWipe>(pParam->structPattern.nPattern, 6, 6))
		0;

	return (NULL != m_privateData.m_pWipe);
}
