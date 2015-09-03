#pragma once

#include "../VideoBuffer.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"

struct AmoebaWipeFxParam : FxParamBase
{
	float fProgress;
	float	fPhase;
	float	fBumpDensity;
	float	fSlant;
	float	fHeight;
	float	fSoftEdge;
	int		nRandomSeed;
	//
	float	fBrightness;
	float	fLightAngle;
	D3DCOLOR	crHColor;
	D3DCOLOR	crLColor;
	//
	float	fOffset;
};


class CAmoebaWipeRender
{
public:
	CAmoebaWipeRender(void);
	~CAmoebaWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, AmoebaWipeFxParam* pParam);

private:
	CRenderEngine*		m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseFx*			m_pEffect;
	CVideoBuffer*		m_pNoiseTexture;
	int m_nRandomSeed;
	float m_fPhase;
};
