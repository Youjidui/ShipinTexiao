#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "../EffBarmWipeTrans/WipeBase.h"

class CMA_VOpenWipe : public CWipeBase
{
public:
	CMA_VOpenWipe(void);
	~CMA_VOpenWipe(void);

	virtual HRESULT Init(CRenderEngine* pEngine);
	virtual HRESULT Draw(CVideoBuffer* pMask, BasicWipeFxParam* pParamRaw);
	virtual void    Ready(CVideoBuffer* pMaskDef);

protected:
	HRESULT InitMesh(CRenderEngine* pEngine);
	CBaseMesh*	m_pMesh;
};
