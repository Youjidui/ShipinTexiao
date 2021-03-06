#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "../EffBarmWipeTrans/WipeBase.h"

class CSingleRevolvWipe : public CWipeBase
{
public:
	CSingleRevolvWipe(void);
	~CSingleRevolvWipe(void);

	virtual HRESULT Init(CRenderEngine* pEngine);
	virtual HRESULT Draw(CVideoBuffer* pMask, BasicWipeFxParam* pParamRaw);
	virtual void    Ready(CVideoBuffer* pMaskDef);

protected:
	HRESULT InitMesh(CRenderEngine* pEngine);
	CBaseMesh*	m_pMesh;
};
