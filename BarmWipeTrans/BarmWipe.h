#pragma once

#include <math.h>
#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../D3D9Render/BaseMesh.h"
#include "basewipe.h"

#define LERP(a,b,s)(a * ( 1 - s ) + b * s)

//extern const GUID UUID_FX;
class CBarmWipe:public CBaseWipe
{
public:
	CBarmWipe();
	~CBarmWipe();
	virtual HRESULT InitMesh(CRenderEngine* pEngine);
	virtual HRESULT Draw(CVideoBuffer* pMaskDef, void* pParamRaw);
	HRESULT Draw(CVideoBuffer* pMaskDef, void* pParamRaw, int nPattern, float fOffset);
	float	CalcOffset(void* pParamRaw, int nPattern);
protected:
private:
	CBaseMesh* m_pBarmMesh;
};
