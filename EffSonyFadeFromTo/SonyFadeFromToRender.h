#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CSonyFadeFromToRender
{
public:
	CSonyFadeFromToRender(void);
	~CSonyFadeFromToRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, FxParamBase* pParam);
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam);

protected:
	CRenderEngine*	m_pEngine;
	CBaseMesh*		m_pQuadMesh;
	CBaseFx*		m_pEffect;
	bool			m_bFadeFromOrTo;	//FadeFrom = true, FadeTo = false
};


class CSonyFadeToRender : public CSonyFadeFromToRender
{
public:
	CSonyFadeToRender() {m_bFadeFromOrTo=(false); }
};

class CSonyFadeFromRender: public CSonyFadeFromToRender
{
public:
	CSonyFadeFromRender() {m_bFadeFromOrTo=(true); }
};
