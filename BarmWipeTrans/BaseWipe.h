#pragma once

#include <d3dx9.h>
#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"

const D3DVERTEXELEMENT9 declWipe[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 		
	D3DDECL_END()
};

class CBaseWipe
{
public:
	CBaseWipe(void);
	virtual ~CBaseWipe(void);
public:
	virtual HRESULT InitMesh(CRenderEngine* pEngine);
	virtual HRESULT Draw(CVideoBuffer* pMask, void* pParamRaw,void* pParent) = 0;
    virtual void    Ready(CVideoBuffer* pMaskDef);
protected:	
	CRenderEngine*		m_pEngine;
	CBaseMesh*			m_pQuadMesh;
	CBaseFx*			m_pEffect;

public:
	D3DXMATRIXA16		m_matAspectProj;
	float				m_fAspect;
};


