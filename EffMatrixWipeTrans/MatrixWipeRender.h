#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "MatrixWipe.h"

class CMatrixWipeRender
{
public:
	CMatrixWipeRender(void);
	~CMatrixWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam);

protected:
	CRenderEngine*	m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseFx*		m_pEffect;
	CMatrixWipe* m_pWipe;
	int			m_nPattern;

protected:
	virtual bool RenderMask(CVideoBuffer*pMaskDef, FxParamBase* pParam);
	virtual bool Ready(CVideoBuffer* pSrcDef, FxParamBase* pParam);
};
