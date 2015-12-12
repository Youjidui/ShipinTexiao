#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CMatrixWipe
{
public:
	CMatrixWipe(CRenderEngine* pEngine);
	~CMatrixWipe(void);

	virtual HRESULT InitMesh(LPDIRECT3DDEVICE9 pDevice);
	virtual HRESULT Draw(CVideoBuffer *pMaskDef, MatrixWipeFxParam* pParam);
	void    Ready(CVideoBuffer* pMaskDef);

	void GenerateSequence(MatrixWipeFxParam* pParam);
	void TopHorizontal();
	void BottomHorizontal();
	void LeftVertical();
	void RightVertical();
	void DiagonalTopLeft();
	void TopLeftSpiral();
	void BottomRightSpiral();
	void TopLeftWaterFall();
	void LeftWaterFall();
	void Random();

protected:
	CRenderEngine*		m_pEngine;
	CBaseFx*			m_pEffect;
	CBaseMesh*			m_pQuadMesh;
	CVideoBuffer*		m_pTexture;

public:
	D3DXMATRIXA16		m_matWorld,m_matView,m_matProj;
	float				m_fAspect;

private:
	int		m_nTitleSize;
	int		m_nPattern;
	int		m_nOffset;
	int		m_nSequence;
	DWORD	m_nLastFrame;

	static int sm_nTitleSize;
	static int sm_nPattern;
	static int sm_nSequence;
};
