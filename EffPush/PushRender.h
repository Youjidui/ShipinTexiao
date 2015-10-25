#pragma once

#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"

class CPushRender
{
public:
	CPushRender(void);
	~CPushRender(void);

	bool Init(CRenderEngine* pEngine);
	void Uninit();
	bool Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1,  CVideoBuffer* pSrc2, PushFxParam* pParam);

private:
	void GetWorldMatrix(D3DXMATRIX * pmat0,D3DXMATRIX *pmat1,DWORD dwType,float fTranstion);

private:
	CRenderEngine* m_pEngine;
	CBaseMesh* m_pQuadMesh;
	CBaseFx* m_pPushEffect;
};