#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "../EffBarmWipeTrans/WipeRenderBase.h"

class CMultiAxisRotaryWipeRender : public CWipeRenderBase
{
public:
	CMultiAxisRotaryWipeRender(void);
	~CMultiAxisRotaryWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();

protected:
	virtual bool RenderMask(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam );
	virtual bool Ready(CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam);

private:
};
