#include "StdAfx.h"
#include "BrokenGlassRender.h"

#pragma warning(disable:4244)

#define _TRANS

CBrokenGlassRender::CBrokenGlassRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
, m_pRandomTexture(NULL)
, m_pRandomPixel(NULL)
{
}

CBrokenGlassRender::~CBrokenGlassRender(void)
{
	Uninit();
}

bool CBrokenGlassRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/BrokenGlassShader.fx"));
	ASSERT(m_pEffect);

	//m_resRandomTex = pResMan->CreateTexture(256,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&uuid_randomTex);
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	{
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 256, 1, 0, 0};
	m_pRandomTexture = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(m_pRandomTexture);
	LPDIRECT3DSURFACE9	pSuf = m_pRandomTexture->GetSurface();
	srand(1234);
	D3DLOCKED_RECT lockRect;
	pSuf->LockRect(&lockRect,NULL,D3DLOCK_DISCARD);
	BYTE* pBits = (BYTE*)(lockRect.pBits);
	for( int i=0; i<256; ++i )
	{
		pBits[i*4] = (int)(rand()/(float)(RAND_MAX)*0xff);				// movement random
		pBits[i*4+1] = (int)(rand()/(float)(RAND_MAX)*0xff);			// width random
	}
	pSuf->UnlockRect();
	//pSuf->Release();
	}

	//m_resRandomPixel = pResMan->CreateTexture(2048,2,D3DUSAGE_DYNAMIC,D3DFMT_R32F,D3DPOOL_DEFAULT,&uuid_randomPixelTex);
	{
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 2048, 2, 0, 0};
	m_pRandomPixel = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(m_pRandomPixel);
	LPDIRECT3DSURFACE9	pSuf = m_pRandomPixel->GetSurface();
	srand(4567);
	D3DLOCKED_RECT lockRect;
	pSuf->LockRect(&lockRect,NULL,D3DLOCK_DISCARD);
	float* pBits = (float*)(lockRect.pBits);
	float* pBits2 = (float*)((BYTE*)lockRect.pBits+lockRect.Pitch);
	for( int i=0; i<2048; ++i )
	{
		pBits[i] = (rand()/(float)(RAND_MAX));
		pBits2[i] = (rand()/(float)(RAND_MAX))-0.5f;
	}
	pSuf->UnlockRect();
	//pSuf->Release();
	}

	//return SUCCEEDED(hr);
	return true;
}

void CBrokenGlassRender::Uninit()
{
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	if(m_pRandomTexture)
	{
		pBufMgr->ReleaseVideoBuffer(m_pRandomTexture);
		m_pRandomTexture = NULL;
	}
	if(m_pRandomPixel)
	{
		pBufMgr->ReleaseVideoBuffer(m_pRandomPixel);
		m_pRandomPixel = NULL;
	}
}

bool CBrokenGlassRender::Render( CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParamRaw )
{
	BrokenGlassFxParam* pParam = (BrokenGlassFxParam*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	// 如果不需要特技处理
#ifdef _TRANS
	if(pParam->prm_bReverse)
		std::swap(pSrcA, pSrcB);
	CVideoBuffer* pSrcDef = pSrcA;
	CVideoBuffer*pDst0 = NULL, *pDstReal = pDest;
	//handle_tpr hDst0 = NewRTBuffer(pSrcDef->OffsetX,pSrcDef->OffsetY,pSrcDef->GetImageWidth(),pSrcDef->GetImageHeight());
	//pDst0 = m_pResMan->GetBufferDef(hDst0);
	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo();
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, biSrc.nWidth, biSrc.nHeight, 0, 0};
	pDst0 = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(pDst0);
	CVideoBuffer* pDstDef = pDst0;
#endif	

	// 参数调整
	float	blockAngle	= max(pParam->prm_width,0.0001f);	// 每一个块的角度
	float	movement	= pParam->prm_movement;
	float	angleBlock	= pParam->prm_angleBlock;
	float	rotateAngle = pParam->prm_rotate;
	float	aspect		= pParam->prm_aspect;	// [-1,1]
	float	minAspect = 0.2f;
	float	maxAspect = 5.f;
	if( aspect<=0 )	aspect=(aspect+1)*(1-minAspect)+minAspect;
	else			aspect=(aspect)*(maxAspect-1)+1;

	// loading和计算工作


	// 对CG和VIDEO分情况考虑
	bool bYUYV	= false;//(pSrcDef->IsYUV16Buffer());
	bool bAlpha = false;//(/*(!pSrcDef->bDiscardAlpha)&&*/(pSrcDef->pAlpha!=NULL));
	bool bYUYVA = bYUYV&&bAlpha;


	//////////////////////////////////////////////////////////////////////////
	//渲染pass
	//////////////////////////////////////////////////////////////////////////

	// setRT
	bool bOK = m_pEngine->SetRenderTarget(pDstDef);
	ASSERT(bOK);

	// VS constant

	// 如果是改变了Dst大小的特技，需要重新定位
	D3DXMATRIXA16	matWorldTemp;
	D3DXMatrixIdentity(&matWorldTemp);
	D3DXMATRIX*matView,*matPorj;
	//pResMan->GetOrthoMatrix(&matView,&matPorj);
	pResMan->GetPerspectiveMatrix(&matView, &matPorj);
	const VideoBufferInfo& biDest = pDstDef->GetVideoBufferInfo();
	//matWorldTemp._42 += 0.5f / pDstDef->GetImageHeight() * (pParam->bOdd?0.0f:1.0f);
	matWorldTemp._42 += (0.5f / biDest.nHeight * (0.0f));
	D3DXMATRIXA16 matCombined = matWorldTemp*(*matView)*(*matPorj);

	D3DXMATRIXA16 matTextureSrc;
	bOK = GenerateMatrix(pSrcDef, &matTextureSrc, mat_Image);
	ASSERT(bOK);

	HRESULT hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombined);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matTexture",&matTextureSrc);	
	ASSERT(SUCCEEDED(hr));

	// PS constant
	D3DXVECTOR4 params(blockAngle,movement,angleBlock,rotateAngle);
	hr = m_pEffect->SetVector("g_param",&params);
	ASSERT(SUCCEEDED(hr));
	D3DXVECTOR4 params2(pParam->prm_centerX,pParam->prm_centerY,aspect,D3DX_PI*2/blockAngle);
	hr = m_pEffect->SetVector("g_param2",&params2);
	ASSERT(SUCCEEDED(hr));
	D3DXVECTOR4	randomDegree(pParam->prm_widthRandom,pParam->prm_movementRandom,pParam->prm_randomPix,0.f);
	hr = m_pEffect->SetVector("g_randomDegree",&randomDegree);
	ASSERT(SUCCEEDED(hr));

	//D3DXVECTOR4 vImage ((pSrcDef->rcImage.left+0.5f)/pSrcDef->BaseWidth,
	//	(pSrcDef->rcImage.top+0.5f)/pSrcDef->BaseHeight,
	//	pSrcDef->GetImageWidth()/(float)(pSrcDef->BaseWidth),
	//	pSrcDef->GetImageHeight()/(float)(pSrcDef->BaseHeight));
	RECT rcImage = {0, 0, biSrc.nWidth, biSrc.nHeight};
	float pSrcDef_BaseWidth = biSrc.nAllocWidth;
	float pSrcDef_BaseHeight = biSrc.nAllocHeight;
	D3DXVECTOR4 vImage ((rcImage.left+0.5f)/pSrcDef_BaseWidth,
		(rcImage.top+0.5f)/pSrcDef_BaseHeight,
		biSrc.nWidth/(pSrcDef_BaseWidth),
		biSrc.nHeight/(pSrcDef_BaseHeight));
	hr = m_pEffect->SetVector("g_texRc",&vImage);
	ASSERT(SUCCEEDED(hr));

	// TEXTURE & stage state & sample state
	hr = m_pEffect->SetTexture("g_txColor",pSrcDef->GetTexture());
	ASSERT(SUCCEEDED(hr));
	//if( bYUYVA )
	//{
	//	m_pEffect->SetTexture("g_txAlpha",((CBaseTexture*)pSrcDef->pAlpha)->GetTexture());
	//}

	hr = m_pEffect->SetTexture("g_txRandom",m_pRandomTexture->GetTexture() );
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTexture("g_txRandomPixel",m_pRandomPixel->GetTexture());
	ASSERT(SUCCEEDED(hr));

	// render state

	// shader
	if( bYUYVA )
		hr = m_pEffect->SetTechnique("RenderYUYV_AP");
	else if(bYUYV)
		hr = m_pEffect->SetTechnique("RenderYUYV_NA");
	else
		hr = m_pEffect->SetTechnique("RenderRGBA");
	ASSERT(SUCCEEDED(hr));

	// begin&end scene
	if ( SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass;
		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		DWORD pSrcDef_bufferFormat = FMT_RGBA32;
		hr = m_pEffect->BeginPass(pSrcDef_bufferFormat == FMT_YUVA32 ? 1 : 0);
		ASSERT(SUCCEEDED(hr));
		bOK = m_pQuadMesh-> DrawMeshFx();
		ASSERT(bOK);
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->End();
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->EndScene();
		ASSERT(SUCCEEDED(hr));
	}

	//D3DXSaveSurfaceToFile(_T("./BrokenGlass_Render_RGBA.dds"), D3DXIFF_DDS, pDstDef->GetSurface(), NULL, NULL);

#ifdef _TRANS
	//BlendTwoBuffer(pDst0,ppSrcDef[1],pDstReal);
	bOK = m_pEngine->BlendCompose(pDstReal, pSrcB, pDst0);
	ASSERT(bOK);
	//FreeRTBuffer(pDst0->handle);
	if(pDst0)
	{
		pBufMgr->ReleaseVideoBuffer(pDst0);
		pDst0 = NULL;
	}
	pDstDef = pDstReal;
#endif

	// 恢复设备状态
	pDevice->SetVertexShader(NULL);
	pDevice->SetPixelShader(NULL);
	pDevice->SetRenderTarget(1,NULL);
	pDevice->SetTexture(0,NULL);
	pDevice->SetTexture(1,NULL);

	// 释放临时buffer

	//////////////////////////////////////////////////////////////////////////
	//渲染pass结束
	//////////////////////////////////////////////////////////////////////////

	return true;
}