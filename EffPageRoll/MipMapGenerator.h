#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CMipMapGenerator
{
public:
	CMipMapGenerator(void);
	~CMipMapGenerator(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrc, FxParamBase* pParam);

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*		m_pMesh[3];
	CBaseFx*		m_pEffect;

protected:
	HRESULT GenerateMipMap(CVideoBuffer* pSrcDef, CVideoBuffer* pMipMap);
};
