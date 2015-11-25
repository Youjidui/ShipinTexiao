#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "MipMapGenerator.h"

class CPageRollRender
{
public:
	CPageRollRender(void);
	~CPageRollRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam);

private:
	CRenderEngine*	m_pEngine;
	CBaseMesh*		m_pQuadMesh;
	CBaseMesh*		m_pMesh;
	//LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecl;
	CBaseFx*		m_pEffect;
	CBaseFx*		m_pTrans_Draw_BG_Effect;
	D3DXMATRIXA16	m_matProject;
	CMipMapGenerator* m_pMipMapGenerator;

protected:
	HRESULT CreateMesh();
	bool Draw(PageRollFxParam* pParam, D3DXMATRIX*	matTex);
	void  Trans_Draw_BG(CVideoBuffer* pBGDef,const BOOL bForPageRoll);
	void GenerateWorld(int nSrcWidth, int nSrcHeight, const D3DXVECTOR2 * pOffset, const D3DXVECTOR2 * pDstSize, D3DXMATRIXA16 * pMatWorld);
};
