#pragma once

#include <math.h>
#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../D3D9Render/BaseMesh.h"
#include "WipeBase.h"
#include "../Utility/mathmacros.h"

//extern const GUID UUID_FX;
class CBarmWipe : public CWipeBase
{
public:
	CBarmWipe();
	~CBarmWipe();
	virtual HRESULT Init(CRenderEngine* pEngine);
	virtual HRESULT Draw(CVideoBuffer* pMask, BasicWipeFxParam* pParam);
	HRESULT Draw(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam, int nPattern, float fOffset);
	float	CalcOffset(BasicWipeFxParam* pParam, int nPattern);
protected:
	HRESULT CreateMesh();

	CBaseMesh* m_pBarmMesh;
};
