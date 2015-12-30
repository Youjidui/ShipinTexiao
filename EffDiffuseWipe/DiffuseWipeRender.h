#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CDiffuseWipeRender
{
public:
	CDiffuseWipeRender(void);
	~CDiffuseWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, DiffuseWipeFxParam* pParam);

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseFx*			m_pDiffuseWipeEffect;
};
