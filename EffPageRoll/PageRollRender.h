#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CPageRollRender
{
public:
	CPageRollRender(void);
	~CPageRollRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrc, FxParamBase* pParam);

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*		m_pQuadMesh;
};
