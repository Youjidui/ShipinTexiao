#include "StdAfx.h"
#include "MultiAxisRotaryWipeRender.h"
#include "CenterVOpenWipe.h"
#include "CenterVWipe.h"
#include "DiagonalWipe.h"
#include "DoubleRotatingWipe.h"
#include "EdgeVOpenWipe.h"
#include "MA_VOpenWipe.h"
#include "MARotatingWipe.h"

CMultiAxisRotaryWipeRender::CMultiAxisRotaryWipeRender(void)
{
}

CMultiAxisRotaryWipeRender::~CMultiAxisRotaryWipeRender(void)
{
	Uninit();
}

bool CMultiAxisRotaryWipeRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;

	bool bOK = CWipeRenderBase::Init(pEngine);
	ASSERT(bOK);

	return bOK;
}

void CMultiAxisRotaryWipeRender::Uninit()
{
	CWipeRenderBase::Uninit();
}


bool CMultiAxisRotaryWipeRender::RenderMask( CVideoBuffer*pMaskDef, BasicWipeFxParam* pParam )
{
	ASSERT(m_privateData.m_pWipe);
	m_privateData.m_pWipe->Ready(pMaskDef);
	HRESULT hr = m_privateData.m_pWipe->Draw(pMaskDef, pParam);
	return SUCCEEDED(hr);
}


bool CMultiAxisRotaryWipeRender::Ready( CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam )
{
	if(InitInstanceByPattern<CMARotatingWipe>(pParam->structPattern.nPattern, 0, 3))
		0;
	else if(InitInstanceByPattern<CDiagonalWipe>(pParam->structPattern.nPattern, 4, 7))
		0;
	else if(InitInstanceByPattern<CDoubleRotatingWipe>(pParam->structPattern.nPattern, 8, 9))
		0;
	else if(InitInstanceByPattern<CCenterVWipe>(pParam->structPattern.nPattern, 10, 10))
		0;
	else if(InitInstanceByPattern<CMA_VOpenWipe>(pParam->structPattern.nPattern, 11, 12))
		0;
	else if(InitInstanceByPattern<CCenterVOpenWipe>(pParam->structPattern.nPattern, 13, 13))
		0;
	else if(InitInstanceByPattern<CEdgeVOpenWipe>(pParam->structPattern.nPattern, 14, 14))
		0;

	return (NULL != m_privateData.m_pWipe);
}
