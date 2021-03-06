#pragma once

#include <d3dx9.h>
#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"
#include "WipeBase.h"

class CWipeRenderBase
{
public:
	CWipeRenderBase(void);
	~CWipeRenderBase(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, BasicWipeFxParam* pParam);

protected:
	virtual bool RenderMask(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam ) = 0;
	virtual bool Ready(CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam) = 0;

protected:
	CVideoBuffer* RenderMultiple(CVideoBuffer* pDest, CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam);
	CVideoBuffer* RenderDivide(CVideoBuffer* pDest, CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam);
	CVideoBuffer* RenderMultiDivide(CVideoBuffer* pDestDef, CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam, BOOL bProcessMultiple, BOOL bProcessDivide);
	void GenerateDivideTexture(BasicWipeFxParam* pParam);
	void GenerateDivideTexture(LPDIRECT3DTEXTURE9	pDivideTexture,int nBuffWidth, float fDivideWidth);
	bool RenderDrawOut(CVideoBuffer* pDstDef, CVideoBuffer* pSrcDefA, CVideoBuffer* pSrcDefB, CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam);

protected:
	CRenderEngine*		m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseMesh*			m_pInstanceMesh;
	CBaseFx*			m_pEffect;
	CBaseFx*			m_pMaskEffect;

	struct PrivateData 
	{
		CVideoBuffer*	m_pDivideHorTexture;
		CVideoBuffer*	m_pDivideVertTexture;
		float					m_fDivideHorValue;
		float					m_fDivideVertValue;

		int m_nPattern;
		CWipeBase* m_pWipe;

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

protected:
	template<class INSTANCE_TYPE>
	bool InitInstanceByPattern( int nPattern, int nMinRange, int nMaxRange )
	{
		if(INSIDE(nPattern, nMinRange, nMaxRange))
		{
			if( !INSIDE(m_privateData.m_nPattern, nMinRange, nMaxRange))
			{
				SAFE_DELETE(m_privateData.m_pWipe);
				m_privateData.m_pWipe = new INSTANCE_TYPE;
				HRESULT hr = m_privateData.m_pWipe->Init(m_pEngine);
				ASSERT(SUCCEEDED(hr));
				m_privateData.m_nPattern = nPattern;
			}
			return true;
		}
		return false;
	}
};
