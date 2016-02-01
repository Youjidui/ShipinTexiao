#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

struct DipToColorTransFxParam : public FxParamBase
{
	float fColor[4];
	float	  fProcess;
	BOOL	bReverse;

	DipToColorTransFxParam()
	{
		//fColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		fColor[0] = fColor[1] = fColor[2] = 0.0f;
		fColor[3] = 1.0f;	//alpha
		fProcess    = 0.0f;
		bReverse = FALSE;
	}
};

class CDipToColorRender
{
public:
	CDipToColorRender(void);
	~CDipToColorRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam);

protected:
	CRenderEngine*	m_pEngine;
	CBaseMesh*		m_pQuadMesh;
	CPixelShader*	m_pEffect;
	CVertexShader* m_VS_DirectOut_V3;
};
