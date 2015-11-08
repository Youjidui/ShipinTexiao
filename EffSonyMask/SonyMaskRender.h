#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CSonyMaskRender
{
public:
	CSonyMaskRender();  

	virtual bool Init(CRenderEngine* pEngine);
	virtual void Uninit();
	virtual bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer * pMaskDef /*pSrcB*/, FxParamBase* pParam);

private:
	void BeginSonyMask(CVideoBuffer * pSrcDef,CVideoBuffer * pDstDef, CVideoBuffer * pMaskDef, FxParamBase* pParam);
	void EndSonyMask(); 
	HRESULT ProcessMask(CVideoBuffer* hDest, CVideoBuffer* hSrc, CVideoBuffer* hEffectSrc, CVideoBuffer* hMask, FxParamBase* pParam);

	virtual bool RenderArea(CVideoBuffer *pSrcDef, CVideoBuffer *pDstDef, void* pParam) = 0;
	virtual void Ready(CVideoBuffer *pSrcDef,void* pParam){};

	//virtual HRESULT SetRenderTarget( const DWORD   rtbaseIndex,  CVideoBuffer* pTarget, D3DCOLOR  dwColor, D3DCOLOR dwAlpha = 0)
	//{
	//	return m_pEngine->SetRenderTarget(rtbaseIndex, pTarget, D3DCLEAR_TARGET, dwColor, dwAlpha, 1.0f, 0);
	//}

	virtual ~CSonyMaskRender()
	{        
	}

protected:

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*		m_pQuadMesh;
	CBaseFx*		m_pDirectOutEffect;
	CBaseFx*		m_pProcessMaskEffect;

public:
	int                 m_nOffsetX,m_nOffsetY;
	BOOL				m_bProcessMask;
	BOOL				m_bOptimize;
	BOOL				m_bRender;
};
