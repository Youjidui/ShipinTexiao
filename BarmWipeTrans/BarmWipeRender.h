#pragma once

#include <d3dx9.h>
#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../SonyBlur/SonyBlurRender.h"
#include "BarmWipe.h"

class CBarmWipeRender
{
public:
	CBarmWipeRender(void);
	~CBarmWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, BarmWipeFxParam* pParam);

protected:
	bool RenderMask(CVideoBuffer* pMaskDef, BarmWipeFxParam* pParam );
	CVideoBuffer* RenderMultiple(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam);
	CVideoBuffer* RenderDivide(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam);
	CVideoBuffer* RenderMulitDivide(CVideoBuffer* pDestDef, CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam, BOOL bProcessMultiple, BOOL bProcessDivide);
	void GenerateDivideTexture(BarmWipeFxParam* pParam);
	void GenerateDivideTexture(LPDIRECT3DTEXTURE9	pDivideTexture,int nBuffWidth, float fDivideWidth);
	bool RenderDrawOut(CVideoBuffer* pSrcDefA, CVideoBuffer* pSrcDefB, CVideoBuffer* pMaskDef, CVideoBuffer* pDstDef, BarmWipeFxParam* pParam);
	bool Ready(CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam);

private:
	CRenderEngine*		m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseMesh*			m_pInstanceMesh;
	CBaseFx*			m_pEffect;
	CBaseFx*			m_pMaskEffect;
	//CVideoBuffer*		m_pNoiseTexture;

	struct PrivateData 
	{
		CVideoBuffer*	m_pDivideHorTexture;
		CVideoBuffer*	m_pDivideVertTexture;
		float					m_fDivideHorValue;
		float					m_fDivideVertValue;

		int m_nPattern;
		CBarmWipe* m_pWipe;

		PrivateData()
		{
			m_pDivideHorTexture = NULL;
			m_pDivideVertTexture = NULL;
			m_fDivideHorValue = 0.0f;
			m_fDivideVertValue = 0.0f;
			m_nPattern = 0;
			m_pWipe = NULL;
		};
	};
	PrivateData m_privateData;
};
