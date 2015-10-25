#include "StdAfx.h"
#include "BarmWipeRender.h"
#include "../Utility/SafeDelete.h"

#pragma warning(disable:4244)

#define INSIDE(x,Min,Max) (x >= Min && x <= Max)


CBarmWipeRender::CBarmWipeRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
, m_pMaskEffect(NULL)
{
	m_privateData.m_nPattern = 10000; //outside from 0 to 5
}

CBarmWipeRender::~CBarmWipeRender(void)
{
	Uninit();
}

bool CBarmWipeRender::Init( CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/BasicWipe.fx"));
	ASSERT(m_pEffect);
	m_pMaskEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Barm_Mask.fx"));
	ASSERT(m_pMaskEffect);
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	int w, h;
	m_pEngine->GetTargetVideoSize(w, h);
	if(!m_privateData.m_pDivideHorTexture)
	{
		VideoBufferInfo bi = {D3DFMT_L8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, w, 1, w, 1};
		m_privateData.m_pDivideHorTexture = pBufMgr->CreateVideoBuffer(bi);
		ASSERT(m_privateData.m_pDivideHorTexture);
	}
	if(!m_privateData.m_pDivideVertTexture)
	{
		VideoBufferInfo bi = {D3DFMT_L8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, h, 1, h, 1};
		m_privateData.m_pDivideVertTexture = pBufMgr->CreateVideoBuffer(bi);
		ASSERT(m_privateData.m_pDivideVertTexture);
	}

	return true;
}


void CBarmWipeRender::Uninit()
{
	//m_blurRender.Uninit();
	//CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	//pBufMgr->ReleaseVideoBuffer(m_pNoiseTexture);
}

bool CBarmWipeRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, BarmWipeFxParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	pParam->structPattern.fOffset = min(pParam->structPattern.fOffset, 1.f);
	if((pParam->structPattern.fOffset <= 0.0f || pParam->structPattern.fOffset > 1.0f) && pParam->structPattern.fCenterX == 0.0f && pParam->structPattern.fCenterY == 0.0f)
	{	
		if(pParam->structPattern.bInvert)
		{
			m_pEngine->SetRenderTarget(pDest);
		}
		else
		{
			m_pEngine->EffectVideoCopy(pSrcA, pDest);
			//pDstDef->fAlphaValue *= pParam->structGeneral.fTransparency;
		}
		return TRUE;
	}

	Ready(pSrcA, pParam);	

	// generate mask
	CVideoBuffer* pMask = m_pEngine->CreateRenderTargetBuffer();
	m_pEngine->SetRenderTarget(pMask);

	//m_fGlobalAspect = pSrcDef->IsYUV16Buffer()?1.0f:2.0f;

	if( SUCCEEDED( pDevice->BeginScene() ) )
	{	

		BOOL bProcessMultiple = pParam->structModify.nMultipleType > 0 && (pParam->structModify.nMultipleNumberX > 1 || pParam->structModify.nMultipleNumberY > 1);
		BOOL bProcessDivide = pParam->structModify.nDivideType >0 && pParam->structModify.fDivideWidth > 0.0f;

		if(bProcessMultiple)
		{
			//pMaskDef->rcImage.right = CEIL(pMaskDef->GetImageWidth() / (float)pParam->structModify.nMultipleNumberX);
			//pMaskDef->rcImage.bottom = CEIL(pMaskDef->GetImageHeight() / (float)pParam->structModify.nMultipleNumberY);		

			//if(m_pParam->structModify.bFlip && m_pParam->structModify.nMultipleNumberY > 1)
			//{
			//	pMaskDef->rcImage.top += 0;
			//	pMaskDef->rcImage.bottom += 2;
			//}
		}

		pDevice->SetTexture(0,NULL);
		RenderMask(pMask, pParam);

		pMask = RenderMulitDivide(pMask,pSrcA,pParam, bProcessMultiple,bProcessDivide);

		RenderDrawOut(pSrcA, pSrcB, pMask, pDest, pParam);

		pDevice->EndScene();
	}

	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	pBufMgr->ReleaseVideoBuffer(pMask);

	pDevice->SetRenderTarget(0,NULL);
	//pDstDef->fAlphaValue = pSrcDef->fAlphaValue * pParam->structGeneral.fTransparency;
	return true;
}	

bool CBarmWipeRender::Ready(CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam)
{
	if(INSIDE(pParam->structPattern.nPattern, 0,5) && !INSIDE(m_privateData.m_nPattern, 0,5))
	{
		m_privateData.m_nPattern = pParam->structPattern.nPattern;

		SAFE_DELETE(m_privateData.m_pWipe);
		m_privateData.m_pWipe = new CBarmWipe;
		m_privateData.m_pWipe->InitMesh(m_pEngine);
	}

	return TRUE;
}

bool CBarmWipeRender::RenderMask(CVideoBuffer* pMaskDef, BarmWipeFxParam* pParam)
{
	bool bOK = false;
	if(!m_privateData.m_pWipe)
		Ready(pMaskDef, pParam);
	if(m_privateData.m_pWipe)
	{
		m_privateData.m_pWipe->Ready(pMaskDef);
		HRESULT hr = m_privateData.m_pWipe->Draw(pMaskDef, pParam, this);
		bOK = SUCCEEDED(hr);
	}
	return bOK;
}

CVideoBuffer* CBarmWipeRender::RenderMultiple(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	CVideoBuffer* pMaskDest = m_pEngine->CreateRenderTargetBuffer();
	const VideoBufferInfo& biMaskDest = pMaskDest->GetVideoBufferInfo();

	float fOffsetY_Other = 1.0f;
	D3DXVECTOR4 vMultipleMisc;
	//pParam->bOdd
	bool bOdd = false;
	if(fOffsetY_Other == 1.0f && 0)
		vMultipleMisc = D3DXVECTOR4(pParam->structModify.bFlip ? 1.0f : 0.0f, pParam->structModify.bOverlap ? 1.f : 0.f,
		1.0f / biMaskDest.nWidth * (bOdd ? 1.0f : 0.0f), 1.0f / biMaskDest.nHeight);
	else
		vMultipleMisc = D3DXVECTOR4(pParam->structModify.bFlip ? 1.0f : 0.0f, pParam->structModify.bOverlap  ? 1.0f : 0.0f,
		1.0f / biMaskDest.nWidth * (bOdd ? 0.0f : 1.0f), 1.0f / biMaskDest.nHeight);

	D3DXMATRIX matTex;
	//if(pParam->structModify.bFlip && pParam->structModify.nMultipleNumberY > 1)
	//	pDest->rcImage.bottom -= 2;

	const VideoBufferInfo& biInMask = pMask->GetVideoBufferInfo();
	::GenerateMatrix(pMask, &matTex, mat_Image);

	if(pParam->structModify.bFlip && pParam->structModify.nMultipleNumberY > 1)
		matTex._32 += 1.0f / biInMask.nAllocHeight;
	else
		matTex._32 -= 0.4f / biInMask.nAllocHeight;

	D3DXVECTOR4 vMultiple = D3DXVECTOR4(biInMask.nWidth / 1.0f / biInMask.nWidth,
		biInMask.nHeight / 1.0f / biMaskDest.nHeight,
		pParam->structModify.nMultipleNumberX * 1.f, pParam->structModify.nMultipleNumberY * 1.f);
	if(pParam->structModify.bOverlap || pParam->structModify.nMultipleType == 2)
		vMultiple.z += 1.0f;
	if(pParam->structModify.bOverlap || pParam->structModify.nMultipleType == 3)
		vMultiple.w += 1.0f;
	//else			
	m_pEngine->SetRenderTarget(pMaskDest);

	D3DXMATRIX* matWorld = NULL, *matView = NULL, *matProj= NULL;
	pResMgr->GetQuadMatrix(&matWorld, &matView, &matProj);
	D3DXMATRIX matCombine = *matView * *matProj;
	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);

	D3DXVECTOR4 vTexCoordFlip;
	//if(pSrcDef->OffsetY_Other == 1.0f && pSrcDef->bIsCGBuffer)
	//	vTexCoordFlip = D3DXVECTOR4 (1.0f / biDest.nAllocWidth / matTex._11,((bOdd ? 1.0f : 0.0f) + 1.0f) / biDest.nAllocHeight / matTex._22,0.0f,0.0f);
	//else
	vTexCoordFlip = D3DXVECTOR4 (1.0f / biInMask.nAllocWidth / matTex._11,((bOdd ? 0.0f : 1.0f) + 1.0f) / biInMask.nAllocHeight / matTex._22,0.0f,0.0f);

	m_pEffect->SetTexture("g_txColor",pMask->GetTexture());			
	m_pEffect->SetMatrix("g_matTexture",&matTex);
	m_pEffect->SetVector("g_vMultiple",&vMultiple);
	m_pEffect->SetVector("g_vMultipleMisc",&vMultipleMisc);
	m_pEffect->SetVector("g_vTexCoordFlip",&vTexCoordFlip);

	m_pEffect->SetTechnique("Multiple");			

	UINT cPass = 0;
	UINT uPass = pParam->structModify.nMultipleType - 1;
	m_pEffect->Begin(&cPass,0);

	m_pEffect->BeginPass(uPass);				
	//m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4,0,2);
	CBaseMesh* pMesh = pResMgr->CreateQuadInstanceMesh(pDevice);
	if(pMesh)	pMesh->DrawInstance(int(vMultiple.z * vMultiple.w));

	m_pEffect->EndPass();

	m_pEffect->End();	

	//pMaskDef = pMaskDef1;
	//hMask = hMask1;
	return pMaskDest;
#ifdef _SHAPE_WIPE
	pMaskDef->rcImage = rcMask;
#endif
	//D3DXSaveTextureToFile(L"c:\\2.bmp",D3DXIFF_BMP,((CBaseTexture*)pMaskDef->pContainer)->GetTexture(),NULL);
}

CVideoBuffer* CBarmWipeRender::RenderDivide(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	GenerateDivideTexture(pParam);

	//handle_tpr hMask1 = NewRTBuffer(0.0f,0.0f,pMaskDef->GetImageWidth() ,pMaskDef->GetImageHeight(),pSrcDef->bIsCGBuffer);
	//TP_VBufferDef *pMaskDef1 = m_pResMan->GetBufferDef(hMask1);
	CVideoBuffer* pMaskDest = m_pEngine->CreateRenderTargetBuffer();
	const VideoBufferInfo& biMaskDest = pMaskDest->GetVideoBufferInfo();

	CVideoBuffer* pMaskSrc = pMask;
	const VideoBufferInfo& biMaskSrc = pMaskSrc->GetVideoBufferInfo();

	BOOL aPass[4] = {FALSE};
	switch(pParam->structModify.nDivideType)
	{
		case 1:aPass[0] = TRUE;break;
		case 2:aPass[1] = TRUE;break;
		case 3:aPass[2] = TRUE;break;
		case 4:aPass[3] = TRUE;break;
		case 5:aPass[0] = aPass[2]  =TRUE;break;
	}

	D3DXMATRIX* matWorld = NULL, *matView = NULL, *matProj= NULL;
	pResMgr->GetQuadMatrix(&matWorld, &matView, &matProj);
	D3DXMATRIX matCombine = *matView * *matProj;
	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);

	m_pEffect->SetTechnique("Divide");			

	m_pEngine->SetRenderTarget(pMaskDest);

	D3DXMATRIX matTex,matMask;
	GenerateMatrix(pMaskSrc, &matTex, mat_Image);
	m_pEffect->SetTexture("g_txColor",pMaskSrc->GetTexture());
	m_pEffect->SetTexture("g_txDivide",(pParam->structModify.nDivideType < 3 || pParam->structModify.nDivideType == 5 ) ?
		m_privateData.m_pDivideVertTexture->GetTexture() : m_privateData.m_pDivideHorTexture->GetTexture());
	m_pEffect->SetMatrix("g_matTexture",&matTex);
	D3DXMatrixIdentity(&matMask);
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	matMask._31 = 0.5f / nEditWidth;
	matMask._32 = 0.5f / nEditHeight;
	m_pEffect->SetMatrix("g_matMask",&matMask);

	D3DXVECTOR4 vMisc;
	float OffsetY_Other = 1.0f;
	bool pParam_bOdd = false;
	if(OffsetY_Other == 1.0f )
		vMisc = D3DXVECTOR4(1.0f / biMaskSrc.nHeight * (pParam_bOdd ? 1.0f : 0.0f),1.0f - 1.0f / biMaskSrc.nWidth, 1.0f - 1.0f / biMaskSrc.nHeight, 0.0f);
	else
		vMisc = D3DXVECTOR4(1.0f / biMaskSrc.nHeight * (pParam_bOdd ? 0.0f : 1.0f),1.0f - 1.0f / biMaskSrc.nWidth, 1.0f - 1.0f / biMaskSrc.nHeight, 0.0f);

	m_pEffect->SetVector("g_vDivideMisc",&vMisc);

	UINT cPass = 0;
	UINT uPass = 0;
	m_pEffect->Begin(&cPass,0);
	for(uPass = 0; uPass < cPass ;uPass ++)
	{
		if(!aPass[uPass])
			continue;

		if(aPass[0] && uPass == 2)
		{				
			std::swap(pMaskSrc, pMaskDest);

			m_pEngine->SetRenderTarget(pMaskDest);

			::GenerateMatrix(pMaskSrc, &matTex, mat_Image);
			m_pEffect->SetTexture("g_txColor", pMaskSrc->GetTexture());

			m_pEffect->SetTexture("g_txDivide",uPass < 2 ? m_privateData.m_pDivideVertTexture->GetTexture() : m_privateData.m_pDivideHorTexture->GetTexture());

			m_pEffect->SetMatrix("g_matTexture", &matTex);
		}

		m_pEffect->BeginPass(uPass);
		m_pEffect->CommitChanges();
		m_pQuadMesh->DrawMeshFx();	
		m_pEffect->EndPass();

		//m_pResMan->DumpResourceToFile(pDestMaskDef->handle,L"c:\\mask.dds");				
	}
	m_pEffect->End();

	return pMaskDest;
}

CVideoBuffer* CBarmWipeRender::RenderMulitDivide(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BarmWipeFxParam* pParam, BOOL bProcessMultiple, BOOL bProcessDivide )
{
	CVideoBuffer* pStage2Mask = pMask;
	//Multiple
	if(bProcessMultiple)
	{
		pStage2Mask = RenderMultiple(pMask, pSrcDef, pParam);
	}
	// Divide

	if(bProcessDivide)
	{
		pStage2Mask = RenderDivide(pStage2Mask, pSrcDef, pParam);
	}
	return pStage2Mask;
}


void CBarmWipeRender::GenerateDivideTexture(LPDIRECT3DTEXTURE9	pDivideTexture,int nBuffWidth, float fDivideWidth)
{
	D3DLOCKED_RECT lr;
	HRESULT hr = pDivideTexture->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	if(SUCCEEDED(hr))
	{
		BYTE * pCr = (BYTE*) lr.pBits;
		int nIndex = 0;
		int nValue = 0;
		for(int i = 0;i < nBuffWidth && nIndex < nBuffWidth;i++)
		{
			int nNumRow = int(nBuffWidth * fDivideWidth * (i + 1)) - int(nBuffWidth * fDivideWidth * i);

			nValue = i%2;
			if(nNumRow == 0)
			{
				nValue = 1.0f - nValue;
				nNumRow = 1;
			}		
			for(int j = 0; j < nNumRow && nIndex < nBuffWidth;j ++)
				pCr[nIndex++] = nValue;		
		}
		pDivideTexture->UnlockRect(0);
	}
}

void CBarmWipeRender::GenerateDivideTexture(BarmWipeFxParam* pParam)
{
	float fWidth = pParam->structModify.fDivideWidth;
	if(pParam->structModify.nDivideType == 0 || fWidth == 0.0f)
		return;

	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);

	switch(pParam->structModify.nDivideType) {
	case 1:
	case 2:
	case 5:
		if(m_privateData.m_fDivideHorValue != fWidth)
		{
			m_privateData.m_fDivideHorValue = fWidth;
			GenerateDivideTexture(m_privateData.m_pDivideHorTexture->GetTexture(), nEditHeight, fWidth);
		}
		break;	
	}
	switch(pParam->structModify.nDivideType) {	
	case 3:
	case 4:
	case 5:
		if(m_privateData.m_fDivideVertValue != fWidth)
		{
			m_privateData.m_fDivideVertValue = fWidth;
			GenerateDivideTexture(m_privateData.m_pDivideHorTexture->GetTexture(), nEditWidth, fWidth);
		}
		break;
	}	
}

bool CBarmWipeRender::RenderDrawOut(CVideoBuffer* pSrcDefA, CVideoBuffer* pSrcDefB, CVideoBuffer* pMaskDef, CVideoBuffer* pDstDef, BarmWipeFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	m_pEngine->SetRenderTarget(pDstDef);

	D3DVIEWPORT9 vPort;
	pDevice->GetViewport(&vPort);
	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	const VideoBufferInfo& biDest = pDstDef->GetVideoBufferInfo();
	vPort.Width = min( biMask.nWidth, biDest.nAllocWidth);
	vPort.Height = min(biMask.nHeight, biDest.nAllocHeight);	
	pDevice->SetViewport(&vPort);

	D3DXMATRIX matWorld0;
	D3DXMatrixIdentity(&matWorld0);	

	D3DXMATRIX matTex,matMask;	
	GenerateMatrix(pMaskDef, &matMask, mat_Image);

#ifdef _TRANS	
	D3DXMatrixIdentity(&matTex);
	matTex._31 = 0.5f / pSrcDef->BaseWidth;
	matTex._32 = 0.5f / pSrcDef->BaseHeight;

	m_pBasicWipeEffect->SetTexture("g_txColor1",((CBaseTexture*)ppSrcDef[1]->pContainer)->GetTexture());

	D3DXVECTOR4 vTransMisc(ppSrcDef[0]->fAlphaValue,ppSrcDef[1]->fAlphaValue,ppSrcDef[0]->pAlpha != NULL,ppSrcDef[1]->pAlpha != NULL);
	m_pBasicWipeEffect->SetVector("g_vTransMisc",&vTransMisc);

	m_pBasicWipeEffect->SetTechnique("Trans");
#else
	const VideoBufferInfo& biSrcA = pSrcDefA->GetVideoBufferInfo();
	//if(!pSrcDef->bIsCGBuffer)	
	{
		matWorld0._11 = biSrcA.nWidth  / float(vPort.Width);
		matWorld0._22 = biSrcA.nHeight / float(vPort.Height);

		float pSrcDef_OffsetX = 0.0f, pSrcDef_OffsetY = 0.0f;
		bool pSrcDef_IsYUV16Buffer = false;
		matWorld0._41 =  -0.5f + matWorld0._11 * 0.5f + pSrcDef_OffsetX * (pSrcDef_IsYUV16Buffer ?0.5f:1.0f) / float(vPort.Width) ;
		matWorld0._42 =  0.5f - matWorld0._22 * 0.5f + (- pSrcDef_OffsetY /*+ (pParam->bOdd?0.0f:0.5f)*/) / float(vPort.Height);
	}
	::GenerateMatrix(pSrcDefA, &matTex, mat_Image);
	m_pEffect->SetTechnique("Picture");
#endif

	D3DXVECTOR4 vBorderColor;
	D3DXCOLOR crBorderColor = pParam->structPattern.crBorderColor;
	//m_pEngine->ColorConv_RGBA_YUVA(&crBorderColor,&crBorderColor);
	vBorderColor = D3DXVECTOR4(crBorderColor.r,crBorderColor.g,crBorderColor.b,crBorderColor.a);
	UINT uPass,cPass;
	//biSrcA.format
	int biSrcA_format = FMT_RGBA32;
	switch(biSrcA_format) {
		case FMT_YUYV:
			//if(pSrcDef->pAlpha)
			//{
			//	uPass = 0;
			//	m_pBasicWipeEffect->SetTexture("g_txAlpha",((CBaseTexture*)pSrcDef->pAlpha)->GetTexture());
			//}
			//else
				uPass = 1;
#ifdef _TRANS	
			if(ppSrcDef[1]->pAlpha)
			{
				uPass = 0;
				m_pBasicWipeEffect->SetTexture("g_txAlpha1",((CBaseTexture*)ppSrcDef[1]->pAlpha)->GetTexture());
			}
#endif
			break;
		case FMT_RGBA32:     
			crBorderColor = pParam->structPattern.crBorderColor;
			vBorderColor = D3DXVECTOR4(crBorderColor.r,crBorderColor.g,crBorderColor.b,crBorderColor.a);
			uPass = 2;
			if(pParam->structPattern.fBorderWidth > 0.0f)
				uPass = 3;
			break;
		case FMT_YUVA32:
			uPass = 2;
			if(pParam->structPattern.fBorderWidth > 0.0f)
				uPass = 3;
			break;				
	}
	m_pEffect->SetTexture("g_txMask", pMaskDef->GetTexture());
	m_pEffect->SetTexture("g_txColor",pSrcDefA->GetTexture());

	LPD3DXMATRIX matWorld = NULL, matView = NULL, matProj= NULL;
	pResMgr->GetQuadMatrix(&matWorld, &matView, &matProj);
	D3DXMATRIX matCombine = *matView * *matProj;
	m_pEffect->SetMatrix("g_matWorldViewProj", &matCombine);

	D3DXVECTOR4 vMisc;
	vMisc = D3DXVECTOR4(pParam->structPattern.fOffset, 
		pParam->structPattern.fBorderWidth * 0.5f,
		pParam->structPattern.fSoftWidth * 0.5f,
		pParam->structPattern.bInvert ? 1.0f : 0.0f);

	m_pEffect->SetMatrix("g_matTexture",&matTex);
	m_pEffect->SetMatrix("g_matMask",&matMask);
	m_pEffect->SetVector("g_vBorderColor",&vBorderColor);
	m_pEffect->SetVector("g_vMisc",&vMisc);

	m_pEffect->Begin(&cPass,0);

	m_pEffect->BeginPass(uPass);		
	m_pQuadMesh->DrawMeshFx();	
	m_pEffect->EndPass();

	m_pEffect->End();
#ifndef _TRANS
	//if(!pSrcDef->bIsCGBuffer)
	//{
	//	CalcBox(&m_matWorld,&m_matView,&m_matProj,&pDstDef->rcImage);
	//	pDstDef->OffsetX = pDstDef->rcImage.left;
	//	pDstDef->OffsetY = pDstDef->rcImage.top;
	//}
#endif
	//     m_pResMan->DumpResourceToFile(ppSrcDef[0]->handle,L"c:\\src1.dds");
	//     m_pResMan->DumpResourceToFile(ppSrcDef[1]->handle,L"c:\\src2.dds");
	//     m_pResMan->DumpResourceToFile(pDstDef->handle,L"c:\\dest.dds");
	return true;
}

