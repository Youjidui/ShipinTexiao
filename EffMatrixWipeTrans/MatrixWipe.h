#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "../BarmWipeTrans/WipeBase.h"

class CMatrixWipe : public CWipeBase
{
public:
	CMatrixWipe();
	~CMatrixWipe(void);

	virtual HRESULT Init(CRenderEngine* pEngine);
	virtual HRESULT Draw(CVideoBuffer* pMask, BasicWipeFxParam* pParamRaw);
	virtual void    Ready(CVideoBuffer* pMaskDef);

protected:
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
	CVideoBuffer*		m_pTexture;

public:
	D3DXMATRIXA16		m_matWorld,m_matView,m_matProj;

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
