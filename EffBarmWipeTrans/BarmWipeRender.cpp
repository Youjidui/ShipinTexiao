#include "StdAfx.h"
#include "BarmWipeRender.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/mathmacros.h"

#pragma warning(disable:4244)

CBarmWipeRender::CBarmWipeRender(void)
{
}

CBarmWipeRender::~CBarmWipeRender(void)
{
	Uninit();
}

bool CBarmWipeRender::Init( CRenderEngine* pEngine)
{
	bool bOK = CWipeRenderBase::Init(pEngine);
	ASSERT(bOK);

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pMaskEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Barm_Mask.fx"));
	ASSERT(m_pMaskEffect);
	return m_pMaskEffect && bOK;
}


void CBarmWipeRender::Uninit()
{
	CWipeRenderBase::Uninit();
}

//bool CBarmWipeRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, BarmWipeFxParam* pParam )
//{
//}	

bool CBarmWipeRender::Ready(CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam)
{
	if(INSIDE(pParam->structPattern.nPattern, 0, 5) && !INSIDE(m_privateData.m_nPattern, 0, 5))
	{
		m_privateData.m_nPattern = pParam->structPattern.nPattern;

		SAFE_DELETE(m_privateData.m_pWipe);
		m_privateData.m_pWipe = new CBarmWipe;
		m_privateData.m_pWipe->Init(m_pEngine);
	}

	return NULL != m_privateData.m_pWipe;
}

bool CBarmWipeRender::RenderMask(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	bool bOK = false;
	ASSERT(m_privateData.m_pWipe);
	//if(!m_privateData.m_pWipe)
	//	Ready(pMaskDef, pParam);
	if(m_privateData.m_pWipe)
	{
		m_privateData.m_pWipe->Ready(pMaskDef);
		HRESULT hr = m_privateData.m_pWipe->Draw(pMaskDef, pParam);
		bOK = SUCCEEDED(hr);
	}
	return bOK;
}

