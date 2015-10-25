#include "stdafx.h"
#include ".\basewipe.h"


CBaseWipe::CBaseWipe(void)
{
	m_pEngine = NULL;
	m_pQuadMesh = NULL;
	m_pEffect = NULL;
}

CBaseWipe::~CBaseWipe(void)
{
    //if(m_pResMan && m_pMesh)
    //{
	   // m_pResMan->Release(m_pMesh->GetResID());
    //    m_pMesh = NULL;
    //}
}

HRESULT CBaseWipe::InitMesh(CRenderEngine* pEngine) 
{
	m_pEngine = pEngine;	
	m_pQuadMesh = pEngine->GetResourceManager()->CreateQuadMesh(pEngine->GetDevice());	

	return S_OK;
}
void CBaseWipe::Ready(CVideoBuffer* pMaskDef)
{
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;

	//m_fAspect = m_pResMan->GetAspect() * (pMaskDef->GetImageWidth() * (pMaskDef->IsYUV16Buffer() ? 2.0f : 1.0f))  / (float)(pMaskDef->GetImageHeight()  * m_pResMan->GetAspectVerifyCoef());    
	m_fAspect = fAspect;
    D3DXMatrixPerspectiveFovLH( &m_matAspectProj, D3DX_PI/4, m_fAspect, 0.5f, 1000.0f );
    m_matAspectProj._11 *= m_fAspect;
    m_matAspectProj._22 *= m_fAspect;
}