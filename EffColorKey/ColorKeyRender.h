#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

struct NVVertex
{
	D3DXVECTOR3 _Pos;
	DWORD	_diffuse;
};


class ColorKeyRender
{
public:
	ColorKeyRender(void);
	~ColorKeyRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrc, ColorKeyParam* pParam);

private:
	HRESULT SetSourceTextureWithTrans( const DWORD dwSampler, CVideoBuffer* pTexDef, MAT_STYLE matStyle );

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*			m_pQuadMesh;

private:
	D3DXMATRIXA16 *m_matWorld, *m_matPrj, *m_matView;

	NVVertex*			m_pBuffer;

private:
	CVertexShader* m_VS_DirectOut;
	CVertexShader* m_VS_DirectOut_V3;

	CPixelShader*	m_PS_ColorKey_RGB32_NOMASK;
	CPixelShader*	m_PS_ColorKey_RGB32_MASK;
	CPixelShader*	m_PS_ColorKey_YUV16_NA_YUYV_NOMASK;
	CPixelShader*	m_PS_ColorKey_YUV16_NA_UYVY_NOMASK;
	CPixelShader*	m_PS_ColorKey_YUV16_AP_YUYV_NOMASK;
	CPixelShader*	m_PS_ColorKey_YUV16_AP_UYVY_NOMASK;
	CPixelShader*	m_PS_ColorKey_YUV16_NA_YUYV_MASK;
	CPixelShader*	m_PS_ColorKey_YUV16_NA_UYVY_MASK;
	CPixelShader*	m_PS_ColorKey_YUV16_AP_YUYV_MASK;
	CPixelShader*	m_PS_ColorKey_YUV16_AP_UYVY_MASK;
	CPixelShader*	m_PS_Blur;
	CPixelShader*	m_PS_FineTune_RGB32_EXPAND;
	CPixelShader*	m_PS_FineTune_YUV16_EXPAND;
	CPixelShader*	m_PS_FineTune_RGB32_SHRINK;
	CPixelShader*	m_PS_FineTune_YUV16_SHRINK;
	CPixelShader*	m_PS_ColorKey_YUVA_NOMASK;
	CPixelShader*	m_PS_ColorKey_YUVA_MASK;

	typedef void (ColorKeyRender::*PASS_FUNCTOR_T)(CVideoBuffer *pSrcDef, CVideoBuffer* pDstDef, FxParamBase* pParam);
	void _colorkey_pass(CVideoBuffer *pSrcDef, CVideoBuffer* pDstDef, FxParamBase* pParam);
	void _fine_tune_pass(CVideoBuffer *pSrcDef, CVideoBuffer* pDstDef, FxParamBase* pParam);
	void _blur_pass(CVideoBuffer *pSrcDef, CVideoBuffer* pDstDef, FxParamBase* pParam);
	PASS_FUNCTOR_T m_render_pass[3];

};
