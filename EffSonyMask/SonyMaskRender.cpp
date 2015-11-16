#include "StdAfx.h"
#include "SonyMaskRender.h"
#include "../Utility/mathmacros.h"

#pragma warning(disable: 4244)

CSonyMaskRender::CSonyMaskRender()
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pDirectOutEffect(NULL)
, m_pProcessMaskEffect(NULL)
, m_nOffsetX(0)
, m_nOffsetY(0)
, m_bProcessMask(0)
, m_bOptimize(0)
, m_bRender(0)
{
}

bool CSonyMaskRender::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);

	m_pDirectOutEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/fx_GDirectOut.fx"));
	ASSERT(m_pDirectOutEffect);
	m_pProcessMaskEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GProcessMask.fx"));
	ASSERT(m_pProcessMaskEffect);

	return true;
}

void CSonyMaskRender::Uninit()
{
}

bool CSonyMaskRender::Render(CVideoBuffer *pDstDef, CVideoBuffer *pSrcDef, CVideoBuffer* pMaskDef /*or alpha*/, FxParamBase* pParam)
{
	CVideoBufferManager* pVBM = m_pEngine->GetVideoBufferManager();
	RESET_RENDER_TARGET(m_pEngine);

	m_bProcessMask	= !!pMaskDef;	// && pMaskDef->bufferStyle == VBUFFER_STYLE_SONYMASK;
	m_bOptimize		=	TRUE;
	m_bRender		=	TRUE;
	Ready(pSrcDef,pParam);

	if(m_bProcessMask)	
	{
		//CVideoBuffer* hTemp = NewRTBuffer(pSrcDef->OffsetX,pSrcDef->OffsetY,pSrcDef->GetImageWidth() * (pSrcDef->IsYUV16Buffer()?2:1),pSrcDef->GetImageHeight()/*,pSrcDef->bIsCGBuffer*/);
		//const VideoBufferInfo biSrc = pSrcDef->GetVideoBufferInfo();
		//VideoBufferInfo biTemp = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo._IN_OUT, biSrc.nWidth, biSrc.nHeight};
		CVideoBuffer* pTempDef = m_pEngine->CreateRenderTargetBuffer();
		if(m_bOptimize)
		{
			//CZBufferManager zbuffer(m_pEngine,DEPTHSTENCIL_GENERAL);
			//m_pEngine->SetDepthBuffer(true);
			SET_DEPTH_STENCIL(m_pEngine);
			BeginSonyMask(pSrcDef, pDstDef, pMaskDef, pParam);
			RenderArea(pSrcDef, pTempDef, pParam);
			EndSonyMask();
			//m_pEngine->SetDepthBuffer(false);
		}
		else
		{
			//CZBufferManager zbuffer(m_pEngine,DEPTHSTENCIL_NONE);
			//m_pEngine->SetDepthBuffer(false);
			RenderArea(pSrcDef,pTempDef,pParam);
		}
		ProcessMask(pSrcDef, pTempDef, pDstDef, pMaskDef, pParam);

		//FreeRTBuffer(hTemp);
		pVBM->ReleaseVideoBuffer(pTempDef);
	}
	else
	{
		RenderArea(pSrcDef, pDstDef, pParam);
	}

	return TRUE;
}

void CSonyMaskRender::BeginSonyMask(CVideoBuffer * pSrcDef,CVideoBuffer * pDstDef, CVideoBuffer * pMaskDef, FxParamBase* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVBM = m_pEngine->GetVideoBufferManager();
	SonyMaskFxParam* pMaskParam = (SonyMaskFxParam*)pParam;

	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo();
	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	const VideoBufferInfo& biDst = pDstDef->GetVideoBufferInfo();
	const RECT pSrcDef_rcImage = {0, 0, biSrc.nWidth, biSrc.nHeight};
	const RECT pDstDef_rcImage = {0, 0, biDst.nWidth, biDst.nHeight};
	const float pSrcDef_OffsetX = 0.f;
	const float pSrcDef_OffsetY = 0.f;

	//if(pMaskDef->bufferStyle == VBUFFER_STYLE_SONYMASK)
	{  
		if(m_nOffsetX || m_nOffsetY)		//GSonyEmbossRender
		{
			int nNumClear = 0;
			RECT rcImage[3];
			D3DRECT rcClear[3];

			for(int i= 0; i < pMaskParam->nNumMask; i ++)
			{
				rcImage[i] = pMaskParam->rcMask[i];

				InflateRect(&rcImage[i],m_nOffsetX * (pMaskParam->bInvert ? -1.0f : 1.0f),m_nOffsetY * (pMaskParam->bInvert ? -1.0f : 1.0f));

				OffsetRect(&rcImage[i], pDstDef_rcImage.left  - CEIL(pSrcDef_OffsetX), pDstDef_rcImage.top - CEIL(pSrcDef_OffsetY));
				rcImage[i].left = __max(0,rcImage[i].left);
				rcImage[i].top = __max(0,rcImage[i].top);
				rcImage[i].right = __min(rcImage[i].right, pDstDef_rcImage.right);
				rcImage[i].bottom = __min(rcImage[i].bottom, pDstDef_rcImage.bottom);
				if(!IsRectEmpty(&rcImage[i]))
				{
					D3DRECT rcTemp = {rcImage[i].left,rcImage[i].top,rcImage[i].right,rcImage[i].bottom};
					rcClear[nNumClear ++] = rcTemp;
				}
			}
			if(nNumClear)
			{
				if(pMaskParam->bInvert)
				{
					pDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0x0,1.0f,0);
					pDevice->Clear(nNumClear,rcClear,D3DCLEAR_ZBUFFER,0x0,0.1f,0);
				}
				else
				{   
					pDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0x0,0.1f,0);
					pDevice->Clear(nNumClear,rcClear,D3DCLEAR_ZBUFFER,0x0,1.0f,0);
				}
				pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
				pDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			}
			else
				pDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
		}
		else
		{
			//CVideoBuffer* hTemp = NewRTBuffer(0,0,m_pEngine->GetCurProfile()->nEditWidth,m_pEngine->GetCurProfile()->nEditHeight);
			//CVideoBuffer* pTempDef = m_pResMan->GetBufferDef(hTemp);
			CVideoBuffer* pTempDef = m_pEngine->CreateRenderTargetBuffer();
			//pTempDef->bClearFullTarget = FALSE;
			//SetRenderTarget(0,hTemp,pSrcDef->COLOR_BLACK());
			m_pEngine->SetRenderTarget(pTempDef);

			if(pMaskParam->bInvert)
			{
				pDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0x0,1.0f,0);
			}
			else
			{   
				pDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0x0,0.1f,0);
			}

			m_pDirectOutEffect->SetTechnique("Generate_Mask_Zbuffer");
			m_pDirectOutEffect->SetTexture("g_txColor", pMaskDef->GetTexture());
			D3DXMATRIXA16 matWorld,matTex,matCombine;
			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixIdentity(&matTex);

			int w, h;
			m_pEngine->GetTargetVideoSize(w, h);
			D3DVIEWPORT9 vPort;
			pDevice->GetViewport(&vPort);
			vPort.X = 0;
			vPort.Y = 0;
			//vPort.Width = (m_pEngine->GetCurProfile()->nEditWidth) * (pSrcDef->IsYUV16Buffer() ? 0.5f : 1.0f);
			//vPort.Height = m_pEngine->GetCurProfile()->nEditHeight;
			vPort.Width = w;
			vPort.Height = h;
			pDevice->SetViewport(&vPort);

			RECT  rcImage;
			SetRect(&rcImage,0,0,vPort.Width,vPort.Height);
			OffsetRect(&rcImage, pDstDef_rcImage.left  - CEIL(pSrcDef_OffsetX), pDstDef_rcImage.top - CEIL(pSrcDef_OffsetY));

			//matWorld._41 =  -0.5f + matWorld._11 * 0.5f + rcImage.left * (pSrcDef->IsYUV16Buffer()? 0.5f : 1.0f) / float(vPort.Width) ;
			matWorld._41 =  -0.5f + matWorld._11 * 0.5f + rcImage.left * (1.0f) / float(vPort.Width) ;
			matWorld._42 =  0.5f - matWorld._22 * 0.5f - rcImage.top / float(vPort.Height);
			D3DXMATRIX *matView = NULL, *matProj= NULL;
			pResMan->GetPerspectiveMatrix( &matView, &matProj);
			matCombine = matWorld * *matView * *matProj;	

			const int pMaskDef_BaseWidth = biMask.nAllocWidth;
			const int pMaskDef_BaseHeight = biMask.nAllocHeight;
			matTex._11 = vPort.Width / (float)pMaskDef_BaseWidth;
			matTex._22 = vPort.Height / (float)pMaskDef_BaseHeight;
			matTex._31 = 0.5f / pMaskDef_BaseWidth;
			matTex._32 = 0.5f / pMaskDef_BaseHeight;

			m_pDirectOutEffect->SetMatrix("g_matWorldViewProj",&matCombine);
			m_pDirectOutEffect->SetMatrix("g_matTexture",&matTex);
			UINT cPass,uPass = pMaskParam->bInvert;
			if(SUCCEEDED(pDevice->BeginScene()))
			{
				m_pDirectOutEffect->Begin(&cPass,0);
				m_pDirectOutEffect->BeginPass(uPass);
				m_pQuadMesh->DrawMeshFx();
				m_pDirectOutEffect->EndPass();
				m_pDirectOutEffect->End();
				pDevice->EndScene();
			}

			pDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);

			m_pDirectOutEffect->SetTexture("g_txColor", NULL);
			//FreeRTBuffer(hTemp);
			pVBM->ReleaseVideoBuffer(pTempDef);
		}        
	}
	//else
	//{
	//	pDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	//}
}

void CSonyMaskRender::EndSonyMask()
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	pDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
}

HRESULT CSonyMaskRender::ProcessMask(CVideoBuffer* pDestDef, CVideoBuffer* pSrcDef, CVideoBuffer* pEffectSrcDef, CVideoBuffer* pMaskDef, FxParamBase* pParam)
{
	HRESULT hr = S_OK;
	//if(pMaskDef->bufferStyle != VBUFFER_STYLE_SONYMASK)
	//	return S_OK;  

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVBM = m_pEngine->GetVideoBufferManager();
	SonyMaskFxParam* pMaskParam = (SonyMaskFxParam*)pParam;

	RECT rcImage;
	BOOL	bChangeRcImage = FALSE;
	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo();
	const VideoBufferInfo& biEffSrc = pEffectSrcDef->GetVideoBufferInfo();
	rcImage.left = 0;
	rcImage.top = 0;
	//if(pSrcDef->GetImageWidth() != pEffectSrcDef->GetImageWidth() || pSrcDef->GetImageHeight() != pEffectSrcDef->GetImageHeight())
	//{
	//	hTemp[0] = Pos(pSrcDef->handle);
	//	if(hTemp[0] != INVALID_RESID)
	//		pSrcDef = m_pResMan->GetBufferDef(hTemp[0]);

	//	hTemp[1] = Pos(pEffectSrcDef->handle);
	//	if(hTemp[1] != INVALID_RESID)
	//		pEffectSrcDef = m_pResMan->GetBufferDef(hTemp[1]);

	//	bChangeRcImage = TRUE;
	//	UnionRect(&rcImage,&pSrcDef->rcImage,&pEffectSrcDef->rcImage);
	//}
	rcImage.right = __max(biSrc.nWidth, biEffSrc.nWidth);
	rcImage.bottom = __max(biSrc.nHeight, biEffSrc.nHeight);

	//m_pResMan->DumpResourceToFile(hSrc,L"c:\\src.dds");
	//m_pResMan->DumpResourceToFile(hEffectSrc,L"c:\\Effectsrc.dds");
	//m_pResMan->DumpResourceToFile(hMask,L"c:\\mask.bmp",FALSE,FALSE,D3DXIFF_BMP);

	//const Custom_Profile * pProfile = m_pEngine->GetCurProfile();
	D3DXMATRIXA16 matWorld,matCombine,matSrcTex,matEffectTex,matMask;	

	::GenerateMatrix(pSrcDef, &matSrcTex, mat_Image);
	::GenerateMatrix(pEffectSrcDef, &matEffectTex, mat_Image);
	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	RECT pMaskDef_rcImage = {0, 0, biMask.nWidth, biMask.nHeight};
	int w, h;
	m_pEngine->GetTargetVideoSize(w, h);
	SetRect(&pMaskDef_rcImage, 0, 0, w, h);
	::GenerateMatrix(pMaskDef, &matMask, mat_Image);

	if(pMaskParam->bInvert)
	{
		std::swap(pSrcDef,pEffectSrcDef);	
		std::swap(matSrcTex,matEffectTex);

		pMaskParam->bInvert = FALSE;
		if(pMaskParam->nArea == 1)		
			pMaskParam->nArea = 2;
		else if(pMaskParam->nArea == 2)
			pMaskParam->nArea = 1;
	}     

	// xp changed on 2011年4月27日 星期三, 
	// bProcessAlpha永远为true
	BOOL bProcessAlpha = TRUE;
	//     BOOL bProcessAlpha = (pMaskDef->structSonyMakInfo.fAlpha < 1.0f && pMaskDef->structSonyMakInfo.nArea > 0) || pSrcDef->fAlphaValue != pEffectSrcDef->fAlphaValue;    
	//     bProcessAlpha = bProcessAlpha || (pEffectSrcDef->pAlpha && pSrcDef->pAlpha) /*|| m_bKey*/;
	// xp changed end
	UINT cPass,uPass;
	D3DXVECTOR4 vBorderColor;
	D3DXCOLOR crBorderColor = pMaskParam->crBorder;
	//D3DXColorScale(&crBorderColor,&crBorderColor,1.0f / pMaskDef->structSonyMakInfo.fAlpha);
	m_pProcessMaskEffect->SetTechnique("Picture");
	const Buffer_Color_Format pSrcDef_bufferFormat = FMT_RGBA32;
	switch(pSrcDef_bufferFormat) {
		case FMT_YUYV:
			//if(!bProcessAlpha)	
			//	uPass = 0;
			//else
			//{
			//	pDestDef->pAlpha = m_pEngine->AllocateVideoAlphaRT();

			//	uPass = 3;
			//	if(pEffectSrcDef->pAlpha && pSrcDef->pAlpha)
			//	{
			//		uPass = 5;
			//		m_pProcessMaskEffect->SetTexture("g_txSrcAlpha",((CBaseTexture*)pSrcDef->pAlpha)->GetTexture());
			//		m_pProcessMaskEffect->SetTexture("g_txEffectAlpha",((CBaseTexture*)pEffectSrcDef->pAlpha)->GetTexture());
			//	}
			//	else if(pEffectSrcDef->pAlpha)
			//	{
			//		uPass = 4;
			//		m_pProcessMaskEffect->SetTexture("g_txEffectAlpha",((CBaseTexture*)pEffectSrcDef->pAlpha)->GetTexture());
			//	}
			//}
			//m_pEngine->ColorConv_RGBA_YUVA(&crBorderColor,&crBorderColor);
			//vBorderColor = D3DXVECTOR4((float*)crBorderColor);
			break;
		case FMT_YUVA32:
			//uPass = 1;
			//m_pEngine->ColorConv_RGBA_YUVA(&crBorderColor,&crBorderColor);
			//vBorderColor = D3DXVECTOR4((float*)crBorderColor);
			break;
		case FMT_RGBA32:
			uPass = 2;
			vBorderColor = D3DXVECTOR4((float*)crBorderColor);
			break;
	}

	//pDestDef->bRenderToFullTarget = TRUE;
	//m_pEngine->SetRenderTarget(0,pDestDef->handle,pDestDef->COLOR_BLACK(),0x0);	
	m_pEngine->SetRenderTarget(pDestDef);
	D3DVIEWPORT9 vPort;
	pDevice->GetViewport(&vPort);
	vPort.Width = w;//pProfile->nEditWidth * (pSrcDef->IsYUV16Buffer()?0.5f : 1.0f);
	vPort.Height = h;//pProfile->nEditHeight;
	pDevice->SetViewport(&vPort);	

	D3DXMatrixIdentity(&matWorld);

	const bool pSrcDef_IsYUV16Buffer = false;
	const int pSrcDef_BaseWidth = biSrc.nAllocWidth;
	const int pSrcDef_BaseHeight = biSrc.nAllocHeight;
	const float pSrcDef_OffsetX = 0.f;
	const float pSrcDef_OffsetY = 0.f;
	if(bChangeRcImage)
	{
		matWorld._11 = RECTWIDTH(rcImage)  / float(vPort.Width);
		matWorld._22 = RECTHEIGHT(rcImage) / float(vPort.Height);

		matWorld._41 =  -0.5f + matWorld._11 * 0.5f + rcImage.left * (pSrcDef_IsYUV16Buffer ? 0.5f : 1.0f) / float(vPort.Width) ;
		matWorld._42 =  0.5f - matWorld._22 * 0.5f - rcImage.top / float(vPort.Height);

		D3DXMatrixIdentity(&matSrcTex);
		matSrcTex._11 = RECTWIDTH(rcImage) / (float)pSrcDef_BaseWidth;
		matSrcTex._22 = RECTHEIGHT(rcImage) / (float)pSrcDef_BaseHeight;

		matSrcTex._31 = (0.5f + rcImage.left) / (float)pSrcDef_BaseWidth;
		matSrcTex._32 = (0.5f + rcImage.top) / (float)pSrcDef_BaseHeight;
		matEffectTex = matSrcTex;
	}
	else
	{
		//pSrcDef->GetImageWidth() ==> biSrc.nWidth
		//pSrcDef->GetImageHeight() ==> biSrc.nHeight
		matWorld._11 = biSrc.nWidth  / float(vPort.Width);
		matWorld._22 = biSrc.nHeight / float(vPort.Height);

		matWorld._41 =  -0.5f + matWorld._11 * 0.5f + pSrcDef_OffsetX * (pSrcDef_IsYUV16Buffer ? 0.5f : 1.0f) / float(vPort.Width) ;
		matWorld._42 =  0.5f - matWorld._22 * 0.5f - pSrcDef_OffsetY / float(vPort.Height);
	}

	D3DXMATRIX *matView = NULL, *matProj= NULL;
	pResMan->GetPerspectiveMatrix( &matView, &matProj);
	//matCombine = matWorld * m_matView * m_matProj;
	matCombine = matWorld * *matView * *matProj;	

	vBorderColor.w = 1.0f;

	m_pProcessMaskEffect->SetTexture("g_txMask",pMaskDef->GetTexture());
	m_pProcessMaskEffect->SetTexture("g_txSrcColor",pSrcDef->GetTexture());
	m_pProcessMaskEffect->SetTexture("g_txEffectColor",pEffectSrcDef->GetTexture());


	m_pProcessMaskEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	m_pProcessMaskEffect->SetMatrix("g_matSrcTexture",&matSrcTex);
	m_pProcessMaskEffect->SetMatrix("g_matEffectTexture",&matEffectTex);
	m_pProcessMaskEffect->SetMatrix("g_matMask",&matMask);

	m_pProcessMaskEffect->SetVector("g_vBorderColor",&vBorderColor);
	D3DXVECTOR4 vMisc(pMaskParam->nMaskType, pMaskParam->bHasBorder, pMaskParam->fAlpha, pMaskParam->nArea);
	const float pSrcDef_fAlphaValue = 1.0f;
	const float pEffectSrcDef_fAlphaValue = 1.0f;
	const bool m_bKey = false;
	D3DXVECTOR4 vAlpha(pSrcDef_fAlphaValue, pEffectSrcDef_fAlphaValue, m_bKey, 0.0f);

	m_pProcessMaskEffect->SetVector("g_vMisc",&vMisc);

	m_pProcessMaskEffect->SetVector("g_vAlpha",&vAlpha);

	if( SUCCEEDED( pDevice->BeginScene() ) )
	{
		m_pProcessMaskEffect->Begin(&cPass,0);

		m_pProcessMaskEffect->BeginPass(uPass);		
		m_pQuadMesh->DrawMeshFx();	
		m_pProcessMaskEffect->EndPass();

		m_pProcessMaskEffect->End();

		pDevice->EndScene();
	}   

	//pDestDef->fAlphaValue = 1.0f;      
	//pDestDef->bContainedAlpha = true;
	pDevice->SetRenderTarget(1,NULL);
	m_pProcessMaskEffect->SetTexture("g_txMask", NULL);
	m_pProcessMaskEffect->SetTexture("g_txSrcColor", NULL);
	m_pProcessMaskEffect->SetTexture("g_txEffectColor", NULL);

	//CalcBox(&matWorld,&m_matView,&m_matProj,&pDestDef->rcImage);    

	//pDestDef->OffsetX = pDestDef->rcImage.left;
	//pDestDef->OffsetY = pDestDef->rcImage.top;

//	for(int i = 0; i < 2;i ++)
//		FreeRTBuffer(hTemp[i]);
	//m_pResMan->DumpResourceToFile(pDestDef->handle,L"c:\\dest.dds");
	return hr;
}

