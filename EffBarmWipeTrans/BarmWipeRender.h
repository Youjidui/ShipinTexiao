#pragma once

#include <d3dx9.h>
#include "../VideoBuffer.h"
#include "../FxParam.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../SonyBlur/SonyBlurRender.h"
#include "BarmWipe.h"
#include "WipeRenderBase.h"

class CBarmWipeRender : public CWipeRenderBase
{
public:
	CBarmWipeRender(void);
	~CBarmWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	//bool Render(CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, BarmWipeFxParam* pParam);

protected:
	virtual bool RenderMask(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam );
	virtual bool Ready(CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam);

private:
};
