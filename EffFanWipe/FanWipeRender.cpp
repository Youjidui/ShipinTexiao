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
	if(InitInstanceByPattern<CRotatingWipe>(pParam->structPattern.nPattern, 0, 7))
	{
	}
	else if(InitInstanceByPattern<CVOpenWipe>(pParam->structPattern.nPattern, 8, 13))
	{
	}
	return NULL != m_privateData.m_pWipe;
}

