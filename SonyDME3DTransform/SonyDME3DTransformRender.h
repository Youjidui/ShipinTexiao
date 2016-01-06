#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include <assert.h>

class CSetEffectParam
{
public:
	CSetEffectParam(LPD3DXEFFECT pEffect):_E(pEffect){ASSERT(_E);}
	template<class T>
	void operator()(const char * cParam, const T * pValue) const
	{ if(FAILED( _E->SetValue(cParam,(LPCVOID)pValue,sizeof(T)))) TRACE("set param error : %s\n", cParam);}    
	void operator()(const char * cTech) const
	{ if (FAILED(_E->SetTechnique(cTech)))TRACE("set tech error : %s\n", cTech);}

private:
	LPD3DXEFFECT _E;
};


class CSonyDME3DTransformRender
{
public:
	CSonyDME3DTransformRender(void);
	~CSonyDME3DTransformRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, FxParamBase* pParam);

protected:
	void RenderRGBA(CVideoBuffer* pDst, CVideoBuffer* pSrc);
	void RenderScene(CVideoBuffer* pDst, CVideoBuffer* pSrc);

	void GetWorldMatrix(D3DXMATRIX * matWorld);

private:
	CRenderEngine* m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseFx* m_pSonyDME3DEffect;
	
	BOOL m_bRotate;
	SonyDME3DTransfromFxPrarm* m_pParam;
	D3DXMATRIXA16	m_matView, m_matProj;
};