#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CSonyFilter
{
public:
	CSonyFilter(void);
	~CSonyFilter(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, FxParamBase* pParam);

private:
	CRenderEngine* m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseFx* m_pEffect;
	CBaseFx* m_pShowKeyEffect;
	CVideoBuffer* m_pSonyFilterCoefTex;

	void CalcBox(const D3DXMATRIX* matWorld,const D3DXMATRIX * matView, const D3DXMATRIX* matProj, RECT* rcImage);
	void CalcSonyFilterCoef(double dbScale,int nTaps,int nPattern/*0 other 1 PageRoll*/);
};
