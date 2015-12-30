#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CSonyPinpRender
{
public:
	CSonyPinpRender(void);
	~CSonyPinpRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, FxParamBase* pParam);

private:
	CRenderEngine* m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseMesh*          m_pMesh;
	CBaseMesh*          m_pBorderMesh;
	//CBaseMesh*			m_pDecayMesh;
	LPDIRECT3DVERTEXBUFFER9 m_pDecayMesh;
	CBaseFx* m_pPinPEffect;
	CBaseFx* m_pDirectOutEffect;
	D3DXMATRIXA16	m_matView, m_matProj;
	CVideoBuffer*		m_pNoiseTexture;

protected:
	bool CreateMesh();
	void _render_for_rgb32(CVideoBuffer* pDstDef, CVideoBuffer* pSrcDef, SonyPinPFxParam* pParam, BOOL bDecay, BOOL bAlphaBlend);
	void _do_render_scene(CVideoBuffer* pSrc, SonyPinPFxParam* pParam, BOOL bDecay, BOOL bAlphaBlend, 
		const RECT& rcDestImage, float offsetX,float offsetY, BOOL bSingleWord,float fAlpha);
	void CalcBox(IN D3DXMATRIXA16 * matWorld, OUT RECT * rcImage);
	HRESULT ClearCGBlend(CVideoBuffer* pDest, CVideoBuffer* pSrc);
	void GenerateWorld(const RECT * pSrcRcImage,const D3DXVECTOR2 * pOffset,const D3DXVECTOR2 * pDstSize,D3DXMATRIXA16 * pMatWorld);
};
