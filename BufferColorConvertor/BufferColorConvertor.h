#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CBufferColorConvertor
{
public:
	CBufferColorConvertor(void);
	~CBufferColorConvertor(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrc, FxParamBase* pParam);

protected:
	CRenderEngine*	m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseFx*		m_pEffect;
};
