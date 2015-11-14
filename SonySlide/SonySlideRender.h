#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CSonySlideRender
{
public:
	CSonySlideRender(void);
	~CSonySlideRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1,  CVideoBuffer* pSrc2, SonySlideFxParam* pParam);

private:
	void GetWorldMatrix(D3DXMATRIX * mat,DWORD dwType,float fTranstion);
	
	//bool BlendTwoBuffer(CVideoBuffer* bufferUp, CVideoBuffer* bufferDown, CVideoBuffer* bufferDest);

private:
	CRenderEngine* m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseFx* m_pSonySliderEffect;
};
