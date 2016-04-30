#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CMeltRender
{
public:
	CMeltRender(void);
	~CMeltRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam);

protected:
	CRenderEngine* m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseFx* m_pEffect;
};
