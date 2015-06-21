#pragma once

#include "../D3D9Render/ResourceManager.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CNegativeRender
{
public:
	CNegativeRender(void);
	~CNegativeRender(void);

	bool Init(LPDIRECT3DDEVICE9 pDevice, CResourceManager* pResMan);
	void Uninit();
	bool Render(CVideoBuffer* pSrc, CVideoBuffer* pDest, NegativeFxParam* pParam);

private:
	bool SetRenderTarget(CVideoBuffer* pDest);
	void GetQuadMatrix( D3DXMATRIX** ppMatWorld, D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj );

private:
	LPDIRECT3DDEVICE9	m_pDevice;
	CResourceManager*	m_pResMan;
	CBaseMesh*			m_pQuadMesh;
	//CBaseFx*			m_pProcessMaskEffect;
	//CBaseFx*			m_pDirectOutEffect;
	CBaseFx*			m_pNegativeEffect;

private:
	D3DXMATRIXA16 *m_matWorld, *m_matPrj, *m_matView;
};
