#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CSonyBarnSlideRender
{
public:
	CSonyBarnSlideRender(void);
	~CSonyBarnSlideRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrcF, CVideoBuffer* pSrcB, FxParamBase* pParam);

private:
	CRenderEngine* m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseMesh*          m_pMesh;
	CBaseFx* m_pEffect;
};
