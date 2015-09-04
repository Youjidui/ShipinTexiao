#pragma once

#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"


class CSonyBlurRender
{
public:
	CSonyBlurRender(void);
	~CSonyBlurRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrc, SonyBlurFxParam* pParam);

private:
	CRenderEngine*		m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseFx*			m_pEffect;
};
