#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
//#include "../EffCommon/SonyMask.h"

class ChromaKeyRender// : public CSonyMaskRender
{
public:
	ChromaKeyRender(void);
	~ChromaKeyRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrc, ColorKeyParam* pParam);

private:
	virtual bool RenderArea(CVideoBuffer*pDstDef, CVideoBuffer *pSrcDef, void* pParam);

private:
	CPixelShader*	m_PS_CRK_RGB32;
	//CPixelShader*	m_PS_CRK_YUYV_NA;
	//CPixelShader*	m_PS_CRK_YUYV_AP;
	//CPixelShader*	m_PS_CRK_YUVA;
};
