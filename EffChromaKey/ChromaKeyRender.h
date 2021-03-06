#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class ChromaKeyRender
{
public:
	ChromaKeyRender(void);
	~ChromaKeyRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrc, FxParamBase* pParam);
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, FxParamBase* pParam);

private:
	bool RenderArea(CVideoBuffer*pDstDef, CVideoBuffer *pSrcDef, FxParamBase* pParam);

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*		m_pQuadMesh;

	CVertexShader*	m_VS_DirectOut_2stages_V3;
	CPixelShader*	m_PS_CRK_RGB32;
	//CPixelShader*	m_PS_CRK_YUYV_NA;
	//CPixelShader*	m_PS_CRK_YUYV_AP;
	CPixelShader*	m_PS_CRK_YUVA;
};
