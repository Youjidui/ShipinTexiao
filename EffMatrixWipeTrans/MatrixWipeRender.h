#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "MatrixWipe.h"
#include "../EffBarmWipeTrans/WipeRenderBase.h"

class CMatrixWipeRender : public CWipeRenderBase
{
public:
	CMatrixWipeRender(void);
	~CMatrixWipeRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	//bool Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParam);

protected:
	virtual bool RenderMask(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam );
	virtual bool Ready(CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam);
};
