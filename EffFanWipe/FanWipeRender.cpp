#include "StdAfx.h"
#include "FanWipeRender.h"
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"
#include "RotatingWipe.h"
#include "VOpenWipe.h"

CFanWipeRender::CFanWipeRender(void)
{
}

CFanWipeRender::~CFanWipeRender(void)
{
	Uninit();
}

bool CFanWipeRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;

	bool bOK = CWipeRenderBase::Init(pEngine);
	ASSERT(bOK);

	return bOK;
}

void CFanWipeRender::Uninit()
{
	CWipeRenderBase::Uninit();
}


bool CFanWipeRender::RenderMask( CVideoBuffer*pMaskDef, BasicWipeFxParam* pParam )
{
	ASSERT(m_privateData.m_pWipe);
	m_privateData.m_pWipe->Ready(pMaskDef);
	HRESULT hr = m_privateData.m_pWipe->Draw(pMaskDef, pParam);
	return SUCCEEDED(hr);
}

bool CFanWipeRender::Ready( CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam )
{
	if(INSIDE(pParam->structPattern.nPattern, 0, 7) && !INSIDE(m_privateData.m_nPattern, 0, 7))
	{
		m_privateData.m_nPattern = pParam->structPattern.nPattern;

		SAFE_DELETE(m_privateData.m_pWipe);
		m_privateData.m_pWipe = new CRotatingWipe;
		m_privateData.m_pWipe->Init(m_pEngine);
	}
	else if(INSIDE(pParam->structPattern.nPattern, 8, 13) && !INSIDE(m_privateData.m_nPattern, 8, 13))
	{
		m_privateData.m_nPattern = pParam->structPattern.nPattern;

		SAFE_DELETE(m_privateData.m_pWipe);
		m_privateData.m_pWipe = new CVOpenWipe;
		m_privateData.m_pWipe->Init(m_pEngine);
	}
	return NULL != m_privateData.m_pWipe;
}

