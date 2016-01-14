#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class Sony3DBrokenGlassRender
{
public:
	Sony3DBrokenGlassRender(void);
	~Sony3DBrokenGlassRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1, CVideoBuffer* pSrc2, Sony3DBrokenGlassEffectParam* pParam);

private:
	CRenderEngine* m_pEngine;
	//CBaseMesh* m_pQuadMesh;
	CBaseFx* m_pSony3DBrokenGlassEffect;
	CBaseMesh* m_pMesh;

	CVideoBuffer* m_pRandomARGBTexture;
	CVideoBuffer* m_pRandom1024x3Texture;

	D3DXMATRIXA16	m_matWorld,m_matView, m_matProj;
};
