#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "../EffPageRoll/MipMapGenerator.h"

class CCubeTransRender
{
public:
	CCubeTransRender(void);
	~CCubeTransRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam);

protected:
	CRenderEngine*	m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseMesh*	m_pMesh;
	CBaseMesh*	m_pInstanceMesh;
	CBaseFx*		m_pEffect;
	CMipMapGenerator* m_pMipMapGenerator;

protected:
	HRESULT CreateMesh();
};
