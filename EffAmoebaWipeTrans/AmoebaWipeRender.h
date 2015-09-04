#pragma once

#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../SonyBlur/SonyBlurRender.h"


class CAmoebaWipeRender
{
public:
	CAmoebaWipeRender(void);
	~CAmoebaWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, AmoebaWipeFxParam* pParam);

protected:
	bool CreateNoiseTexture(AmoebaWipeFxParam* pParam);
	void Resize(CVideoBuffer* pDest, CVideoBuffer* pSrc, AmoebaWipeFxParam* pParam );
	void Light(CVideoBuffer* pDest, CVideoBuffer* pSrc, AmoebaWipeFxParam* pParam );
	void Last(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, CVideoBuffer* pSrcC, CVideoBuffer* pSrcD, AmoebaWipeFxParam* pParam );

private:
	CRenderEngine*		m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseFx*			m_pEffect;
	CVideoBuffer*		m_pNoiseTexture;
	int m_nRandomSeed;
	float m_fPhase;
	CSonyBlurRender		m_blurRender;
};
