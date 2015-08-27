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
	bool Render(CVideoBuffer* pSrc, CVideoBuffer* pDest, NegativeFxParam* pParam);

private:
	bool SetRenderTarget(CVideoBuffer* pDest);

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseFx*			m_pNegativeEffect;
};
