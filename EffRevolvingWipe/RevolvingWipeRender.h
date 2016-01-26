#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "../EffBarmWipeTrans/WipeRenderBase.h"

class CRevolvingWipeRender : public CWipeRenderBase
{
public:
	CRevolvingWipeRender(void);
	~CRevolvingWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();

protected:
	virtual bool RenderMask(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam );
	virtual bool Ready(CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam);
};
