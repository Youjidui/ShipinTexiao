#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

struct NVVertex
{
	D3DXVECTOR3 _Pos;
	DWORD	_diffuse;
};


#define MIN_LENGTH      0.0f
#define MAX_LENGTH      1.0f
#define LENGTH_SCALE    2.0f
#define DEF_LENGTH      0.45f

#define YUV_ANGLE_ERROR (D3DX_PI/12.0f)
#define DEF_ANGLE       (D3DX_PI/12.0f)

struct ColorKeyParam : public FxParamBase
{
	DWORD version;
	float fAngle1, fAngle2, fLength1, fLength2;
	float fSoftness, fBlurness, fFineTune;
	float fGrayMin,fGrayMax;
	BOOL bIsReverse;
	ColorKeyParam()
	{        
		ResetAllParam();
	}

	void ResetAllParam()
	{
		version = 0x0100;
		fAngle1 = D3DX_PI/2.0f - D3DX_PI/12.0f + YUV_ANGLE_ERROR;
		fAngle2 = D3DX_PI/2.0f + D3DX_PI/12.0f + YUV_ANGLE_ERROR;
		fLength1 = MIN_LENGTH / LENGTH_SCALE;
		fLength2 = MAX_LENGTH / LENGTH_SCALE;
		fSoftness = 0.0f;
		fBlurness = 0.0f;
		fFineTune = 0.0f;
		fGrayMin=0.0f;
		fGrayMax=1.0f;
		bIsReverse=FALSE;
	}	

};

class ColorKeyRender
{
public:
	ColorKeyRender(void);
	~ColorKeyRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pSrc, CVideoBuffer* pDest, ColorKeyParam* pParam);

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
