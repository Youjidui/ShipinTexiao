#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CNegativeRender
{
public:
	CNegativeRender(void);
	~CNegativeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, FxParamBase* pParam);

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseFx*			m_pNegativeEffect;
};
