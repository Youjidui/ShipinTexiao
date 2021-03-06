#include "StdAfx.h"
#include "WipeRenderBase.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/mathmacros.h"

CWipeRenderBase::CWipeRenderBase(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pInstanceMesh(NULL)
, m_pEffect(NULL)
, m_pMaskEffect(NULL)
{
	m_privateData.m_nPattern = 10000; //outside of the normal range. for example [0 to 5] in BarmWipe
}

CWipeRenderBase::~CWipeRenderBase(void)
{
	Uninit();
}

bool CWipeRenderBase::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pInstanceMesh = pResMgr->CreateQuadInstanceMesh(pDevice);
	ASSERT(m_pInstanceMesh);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/BasicWipe.fx"));
	ASSERT(m_pEffect);

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

void CWipeRenderBase::Uninit()
{
	SAFE_DELETE(m_privateData.m_pWipe);

	if(m_privateData.m_pDivideHorTexture)
	{
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		pBufMgr->ReleaseVideoBuffer(m_privateData.m_pDivideHorTexture);
		m_privateData.m_pDivideHorTexture = NULL;
	}
	if(m_privateData.m_pDivideVertTexture)
	{
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		pBufMgr->ReleaseVideoBuffer(m_privateData.m_pDivideVertTexture);
		m_privateData.m_pDivideVertTexture = NULL;
	}
}

bool CWipeRenderBase::Render( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, BasicWipeFxParam* pParam )
{
	ASSERT(m_pEngine);
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	RESET_RENDER_TARGET(m_pEngine);
	//m_pEngine->SetDepthBuffer(true);
	SET_DEPTH_STENCIL(m_pEngine);

	if(pParam->structPattern.bInvert)
	{
		std::swap(pSrcA, pSrcB);
	}

	//pParam->structPattern.fOffset = CLAMP(pParam->structPattern.fOffset, 0.f, 1.f);
	//if((pParam->structPattern.fOffset <= 0.0f || pParam->structPattern.fOffset > 1.0f) && pParam->structPattern.fCenterX == 0.0f && pParam->structPattern.fCenterY == 0.0f)
	//{	
	//	{
	//		m_pEngine->EffectVideoCopy(pDest, pSrcA);
	//		//pDstDef->fAlphaValue *= pParam->structGeneral.fTransparency;
	//	}
	//	return TRUE;
	//}

	Ready(pSrcA, pParam);

	// generate mask
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	CVideoBuffer* pMask = m_pEngine->CreateRenderTargetBuffer();
	if(pMask)
	{
		//bool bOK = m_pEngine->SetRenderTarget(pMask);
		//ASSERT(bOK);
		//bOK = m_pEngine->SetDepthBuffer(true);
		//ASSERT(bOK);
		SET_DEPTH_STENCIL(m_pEngine);

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

			bool bOK = RenderMask(pMask, pParam);
			ASSERT(bOK);
			//D3DXSaveSurfaceToFile(_T("./BarmWipe_Mask.DDS"), D3DXIFF_DDS, pMask->GetSurface(), NULL, NULL);

			CVideoBuffer* pNewMask = RenderMultiDivide(pMask,pSrcB, pParam, bProcessMultiple,bProcessDivide);
			ASSERT(pNewMask);
			if(pNewMask != pMask)
			{
				pBufMgr->ReleaseVideoBuffer(pMask);
				pMask = pNewMask;
			}
			//D3DXSaveSurfaceToFile(_T("./BarmWipe_Mask_2.bmp"), D3DXIFF_BMP, pMask->GetSurface(), NULL, NULL);

			bOK = RenderDrawOut(pDest, pSrcA, pSrcB, pMask, pParam);
			ASSERT(bOK);
			//D3DXSaveSurfaceToFile(_T("./BarmWipe_.bmp"), D3DXIFF_BMP, pMask->GetSurface(), NULL, NULL);

			pDevice->EndScene();
			pDevice->SetTexture(0, NULL);
		}
		pDevice->SetRenderTarget(1, NULL);

		//bOK = m_pEngine->SetDepthBuffer(false);
		//ASSERT(bOK);
		BOOL bOK = pBufMgr->ReleaseVideoBuffer(pMask);
		ASSERT(bOK);
	}

	//pDstDef->fAlphaValue = pSrcDef->fAlphaValue * pParam->structGeneral.fTransparency;
	return true;
}

CVideoBuffer* CWipeRenderBase::RenderMultiple(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam)
{
	HRESULT hr = E_FAIL;
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

	VideoBufferInfo biInMask = pMask->GetVideoBufferInfo();

	{
		float fMaskHalfWidth = CEIL(float(biInMask.nWidth) / pParam->structModify.nMultipleNumberX / 2.0f);
		float fMaskHalfHeight = CEIL(float(biInMask.nHeight) / pParam->structModify.nMultipleNumberY / 2.0f);
		if(!pParam->structModify.bOverlap)
		{
			RECT rcImage;
			SetRect(&rcImage, biInMask.nWidth / 2.0f - fMaskHalfWidth,
				biInMask.nHeight / 2.0f - fMaskHalfHeight,
				biInMask.nWidth / 2.0f + fMaskHalfWidth,
				biInMask.nHeight / 2.0f + fMaskHalfHeight);
			biInMask.nWidth = rcImage.right - rcImage.left;
			biInMask.nHeight = rcImage.bottom - rcImage.top;
			::GenerateMatrix(biInMask, &matTex, mat_Image);
		}
		else
		{
			RECT rcImage;
			::GenerateMatrix(pMask, &matTex, mat_Image);
			SetRect(&rcImage, biInMask.nWidth / 2.0f - fMaskHalfWidth,
				biInMask.nHeight / 2.0f - fMaskHalfHeight,
				biInMask.nWidth / 2.0f + fMaskHalfWidth,
				biInMask.nHeight / 2.0f + fMaskHalfHeight);
			biInMask.nWidth = rcImage.right - rcImage.left;
			biInMask.nHeight = rcImage.bottom - rcImage.top;
		}
	}
	//else
	//{
	////if(pParam->structModify.bFlip && pParam->structModify.nMultipleNumberY > 1)
	////	pDest->rcImage.bottom -= 2;
	//::GenerateMatrix(pMask, &matTex, mat_Image);

	//if(pParam->structModify.bFlip && pParam->structModify.nMultipleNumberY > 1)
	//	matTex._32 += 1.0f / biInMask.nAllocHeight;
	//else
	//	matTex._32 -= 0.4f / biInMask.nAllocHeight;
	//}

	D3DXVECTOR4 vMultiple = D3DXVECTOR4(biInMask.nWidth / 1.0f / biMaskDest.nWidth,
		biInMask.nHeight / 1.0f / biMaskDest.nHeight,
		pParam->structModify.nMultipleNumberX * 1.f, pParam->structModify.nMultipleNumberY * 1.f);
	if(pParam->structModify.bOverlap || pParam->structModify.nMultipleType == 2)
		vMultiple.z += 1.0f;
	if(pParam->structModify.bOverlap || pParam->structModify.nMultipleType == 3)
		vMultiple.w += 1.0f;
	//else
	bool bOK = m_pEngine->SetRenderTarget(pMaskDest);
	ASSERT(bOK);

	D3DXMATRIX *matView = NULL, *matProj= NULL;
	pResMgr->GetPerspectiveMatrix( &matView, &matProj);
	D3DXMATRIX matCombine = *matView * *matProj;
	hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR4 vTexCoordFlip;
	//if(pSrcDef->OffsetY_Other == 1.0f && pSrcDef->bIsCGBuffer)
	//	vTexCoordFlip = D3DXVECTOR4 (1.0f / biDest.nAllocWidth / matTex._11,((bOdd ? 1.0f : 0.0f) + 1.0f) / biDest.nAllocHeight / matTex._22,0.0f,0.0f);
	//else
	vTexCoordFlip = D3DXVECTOR4 (1.0f / biInMask.nAllocWidth / matTex._11,((bOdd ? 0.0f : 1.0f) + 1.0f) / biInMask.nAllocHeight / matTex._22,0.0f,0.0f);

	hr = m_pEffect->SetTexture("g_txColor",pMask->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matTexture",&matTex);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vMultiple",&vMultiple);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vMultipleMisc",&vMultipleMisc);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vTexCoordFlip",&vTexCoordFlip);
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetTechnique("Multiple");
	ASSERT(SUCCEEDED(hr));

	UINT cPass = 0;
	UINT uPass = pParam->structModify.nMultipleType - 1;
	if(pParam->structModify.bOverlap)
		uPass += 3;

	hr = m_pEffect->Begin(&cPass,0);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->BeginPass(uPass);
	ASSERT(SUCCEEDED(hr));
	//m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4,0,2);
	bOK = !!m_pInstanceMesh->DrawInstance(int(vMultiple.z * vMultiple.w));
	ASSERT(bOK);

	hr = m_pEffect->EndPass();
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->End();
	ASSERT(SUCCEEDED(hr));

	m_pEffect->SetTexture("g_txColor", NULL);

	//TODO: remove debug output
	//TCHAR szFilename[MAX_PATH];
	//_stprintf(szFilename, _T("Multiple_Misc_x=%f_y=%f_z=%f_w=%f.dds"), vMultipleMisc.x, vMultipleMisc.y, vMultipleMisc.z, vMultipleMisc.w);
	//D3DXSaveSurfaceToFile(szFilename, D3DXIFF_DDS, pMaskDest->GetSurface(), NULL, NULL);

	return pMaskDest;
#ifdef _SHAPE_WIPE
	pMaskDef->rcImage = rcMask;
#endif
	//D3DXSaveTextureToFile(L"c:\\2.bmp",D3DXIFF_BMP,((CBaseTexture*)pMaskDef->pContainer)->GetTexture(),NULL);
}

CVideoBuffer* CWipeRenderBase::RenderDivide(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	GenerateDivideTexture(pParam);

	//handle_tpr hMask1 = NewRTBuffer(0.0f,0.0f,pMaskDef->GetImageWidth() ,pMaskDef->GetImageHeight(),pSrcDef->bIsCGBuffer);
	//TP_VBufferDef *pMaskDef1 = m_pResMan->GetBufferDef(hMask1);
	CVideoBuffer* pMaskSrc = pMask;
	const VideoBufferInfo& biMaskSrc = pMaskSrc->GetVideoBufferInfo();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
	VideoBufferInfo biMaskDest = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, biMaskSrc.nWidth, biMaskSrc.nHeight, 0, 0};
	CVideoBuffer* pMaskDest = pVM->CreateVideoBuffer(biMaskDest);
	ASSERT(pMaskDest);

	BOOL aPass[4] = {FALSE};
	switch(pParam->structModify.nDivideType)
	{
	case 1:aPass[0] = TRUE;break;
	case 2:aPass[1] = TRUE;break;
	case 3:aPass[2] = TRUE;break;
	case 4:aPass[3] = TRUE;break;
	case 5:aPass[0] = aPass[2]  =TRUE;break;
	}

	D3DXMATRIX matWorld, *matView = NULL, *matProj= NULL;
	D3DXMatrixIdentity(&matWorld);
	pResMgr->GetPerspectiveMatrix( &matView, &matProj);
	D3DXMATRIX matCombine = *matView * *matProj;
	HRESULT hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTechnique("Divide");
	ASSERT(SUCCEEDED(hr));

	bool bOK = m_pEngine->SetRenderTarget(pMaskDest);
	ASSERT(bOK);

	D3DXMATRIX matTex,matMask;
	bOK = GenerateMatrix(pMaskSrc, &matTex, mat_Image);
	ASSERT(bOK);
	hr = m_pEffect->SetTexture("g_txColor",pMaskSrc->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTexture("g_txDivide",(pParam->structModify.nDivideType < 3 || pParam->structModify.nDivideType == 5 ) ?
		m_privateData.m_pDivideVertTexture->GetTexture() : m_privateData.m_pDivideHorTexture->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matTexture",&matTex);
	ASSERT(SUCCEEDED(hr));

	D3DXMatrixIdentity(&matMask);
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	matMask._31 = 0.5f / nEditWidth;
	matMask._32 = 0.5f / nEditHeight;
	hr = m_pEffect->SetMatrix("g_matMask",&matMask);
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR4 vMisc;
	float OffsetY_Other = 1.0f;
	bool pParam_bOdd = false;
	if(OffsetY_Other == 1.0f )
		vMisc = D3DXVECTOR4(1.0f / biMaskSrc.nHeight * (pParam_bOdd ? 1.0f : 0.0f),1.0f - 1.0f / biMaskSrc.nWidth, 1.0f - 1.0f / biMaskSrc.nHeight, 0.0f);
	else
		vMisc = D3DXVECTOR4(1.0f / biMaskSrc.nHeight * (pParam_bOdd ? 0.0f : 1.0f),1.0f - 1.0f / biMaskSrc.nWidth, 1.0f - 1.0f / biMaskSrc.nHeight, 0.0f);

	hr = m_pEffect->SetVector("g_vDivideMisc",&vMisc);
	ASSERT(SUCCEEDED(hr));

	UINT cPass = 0;
	UINT uPass = 0;
	hr = m_pEffect->Begin(&cPass,0);
	ASSERT(SUCCEEDED(hr));
	for(uPass = 0; uPass < cPass ;uPass ++)
	{
		if(!aPass[uPass])
			continue;

		if(aPass[0] && uPass == 2)		//nDivideType = 5
		{				
			std::swap(pMaskSrc, pMaskDest);
			bOK = m_pEngine->SetRenderTarget(pMaskDest);
			ASSERT(bOK);
			bOK = GenerateMatrix(pMaskSrc, &matTex, mat_Image);
			ASSERT(bOK);
			hr = m_pEffect->SetTexture("g_txColor", pMaskSrc->GetTexture());
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->SetTexture("g_txDivide",uPass < 2 ? m_privateData.m_pDivideVertTexture->GetTexture() : m_privateData.m_pDivideHorTexture->GetTexture());
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->SetMatrix("g_matTexture", &matTex);
			ASSERT(SUCCEEDED(hr));
		}

		hr = m_pEffect->BeginPass(uPass);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->CommitChanges();
		ASSERT(SUCCEEDED(hr));
		bOK = m_pQuadMesh->DrawMeshFx();
		ASSERT(bOK);
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));

		//TODO: remove debug output
		//TCHAR szFilename[MAX_PATH];
		//_stprintf(szFilename, _T("Divide_pass_%d_Misc_x=%f_y=%f_z=%f.dds"), uPass, vMisc.x, vMisc.y, vMisc.z);
		//D3DXSaveSurfaceToFile(szFilename, D3DXIFF_DDS, pMaskDest->GetSurface(), NULL, NULL);
	}
	hr = m_pEffect->End();
	ASSERT(SUCCEEDED(hr));
	m_pEffect->SetTexture("g_txColor", NULL);
	m_pEffect->SetTexture("g_txDivide", NULL);

	return pMaskDest;
}

CVideoBuffer* CWipeRenderBase::RenderMultiDivide(CVideoBuffer* pMask, CVideoBuffer* pSrcDef, BasicWipeFxParam* pParam, BOOL bProcessMultiple, BOOL bProcessDivide )
{
	CVideoBuffer* pRet = pMask;
	CVideoBuffer* pMask4Multi = pMask;
	bool bNew = false;
	//Multiple
	if(bProcessMultiple)
	{
		pMask4Multi = RenderMultiple(pMask, pSrcDef, pParam);
		pRet = pMask4Multi;
		bNew = pMask4Multi != pMask;
	}
	// Divide

	CVideoBuffer* pMask4Divide = pMask4Multi;
	if(bProcessDivide)
	{
		pMask4Divide = RenderDivide(pMask4Multi, pSrcDef, pParam);
		pRet = pMask4Divide;
		//bNew = pMask4Divide != pMask4Multi;
		if(bNew)
		{
			CVideoBufferManager* pVBM = m_pEngine->GetVideoBufferManager();
			pVBM->ReleaseVideoBuffer(pMask4Multi);
		}
	}
	return pRet;
}


void CWipeRenderBase::GenerateDivideTexture(LPDIRECT3DTEXTURE9	pDivideTexture,int nBuffWidth, float fDivideWidth)
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
				nValue = 1 - nValue;
				nNumRow = 1;
			}		
			for(int j = 0; j < nNumRow && nIndex < nBuffWidth;j ++)
				pCr[nIndex++] = nValue;
		}
		pDivideTexture->UnlockRect(0);
	}
}

void CWipeRenderBase::GenerateDivideTexture(BasicWipeFxParam* pParam)
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
			GenerateDivideTexture(m_privateData.m_pDivideVertTexture->GetTexture(), nEditHeight, fWidth);
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

bool CWipeRenderBase::RenderDrawOut(CVideoBuffer* pDstDef, CVideoBuffer* pSrcDefA, CVideoBuffer* pSrcDefB, CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	bool bOK = m_pEngine->SetRenderTarget(pDstDef);
	ASSERT(bOK);

	D3DVIEWPORT9 vPort;
	HRESULT hr = pDevice->GetViewport(&vPort);
	ASSERT(SUCCEEDED(hr));
	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	const VideoBufferInfo& biDest = pDstDef->GetVideoBufferInfo();
	vPort.Width = min( biMask.nWidth, biDest.nAllocWidth);
	vPort.Height = min(biMask.nHeight, biDest.nAllocHeight);	
	hr = pDevice->SetViewport(&vPort);
	ASSERT(SUCCEEDED(hr));

	D3DXMATRIX matWorld0;
	D3DXMatrixIdentity(&matWorld0);	

	D3DXMATRIX matTex,matMask;	
	bOK = GenerateMatrix(pMaskDef, &matMask, mat_Image);
	ASSERT(bOK);

	const VideoBufferInfo& biSrcA = pSrcDefA->GetVideoBufferInfo();
#ifdef _TRANS
	D3DXMatrixIdentity(&matTex);
	matTex._31 = 0.5f / biSrcA.nAllocWidth;
	matTex._32 = 0.5f / biSrcA.nAllocHeight;

	//m_pEffect->SetTexture("g_txColor1", pSrcDefB->GetTexture());

	//D3DXVECTOR4 vTransMisc(ppSrcDef[0]->fAlphaValue,ppSrcDef[1]->fAlphaValue,ppSrcDef[0]->pAlpha != NULL,ppSrcDef[1]->pAlpha != NULL);
	float fAlphaValue4SrcA = 1.0f, fAlphaValue4SrcB = 1.0f;
	D3DXVECTOR4 vTransMisc(fAlphaValue4SrcA, fAlphaValue4SrcB, false, false);
	hr = m_pEffect->SetVector("g_vTransMisc",&vTransMisc);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTechnique("Trans");
	ASSERT(SUCCEEDED(hr));
#else
	//if(!pSrcDef->bIsCGBuffer)	
	{
		matWorld0._11 = biSrcA.nWidth  / float(vPort.Width);
		matWorld0._22 = biSrcA.nHeight / float(vPort.Height);

		float pSrcDef_OffsetX = 0.0f, pSrcDef_OffsetY = 0.0f;
		bool pSrcDef_IsYUV16Buffer = false;
		matWorld0._41 =  -0.5f + matWorld0._11 * 0.5f + pSrcDef_OffsetX * (pSrcDef_IsYUV16Buffer ?0.5f:1.0f) / float(vPort.Width) ;
		matWorld0._42 =  0.5f - matWorld0._22 * 0.5f + (- pSrcDef_OffsetY /*+ (pParam->bOdd?0.0f:0.5f)*/) / float(vPort.Height);
	}
	bOK = GenerateMatrix(pSrcDefA, &matTex, mat_Image);
	ASSERT(bOK);
	hr = m_pEffect->SetTechnique("Picture");
	ASSERT(SUCCEEDED(hr));
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
			//	m_pEffect->SetTexture("g_txAlpha",((CBaseTexture*)pSrcDef->pAlpha)->GetTexture());
			//}
			//else
			uPass = 1;
#ifdef _TRANS
			//if(pSrcDefB)
			//{
			//	uPass = 0;
			//	m_pEffect->SetTexture("g_txAlpha1", pSrcDefB->GetTexture());
			//}
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
	hr = m_pEffect->SetTexture("g_txMask", pMaskDef->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTexture("g_txColor",pSrcDefA->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTexture("g_txColor1",pSrcDefB->GetTexture());
	ASSERT(SUCCEEDED(hr));

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	LPD3DXMATRIX matView = NULL, matProj= NULL;
	pResMgr->GetPerspectiveMatrix(&matView, &matProj);
	D3DXMATRIX matCombine = *matView * *matProj;
	hr = m_pEffect->SetMatrix("g_matWorldViewProj", &matCombine);
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR4 vMisc;
	vMisc = D3DXVECTOR4(pParam->structPattern.fOffset, 
		pParam->structPattern.fBorderWidth * 0.5f,
		pParam->structPattern.fSoftWidth * 0.5f,
		pParam->structPattern.bInvert ? 1.0f : 0.0f);

	hr = m_pEffect->SetMatrix("g_matTexture",&matTex);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matMask",&matMask);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vBorderColor",&vBorderColor);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vMisc",&vMisc);
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->Begin(&cPass,0);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->BeginPass(uPass);
	ASSERT(SUCCEEDED(hr));
	bOK = m_pQuadMesh->DrawMeshFx();
	ASSERT(bOK);
	hr = m_pEffect->EndPass();
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->End();
	ASSERT(SUCCEEDED(hr));

	m_pEffect->SetTexture("g_txMask", NULL);
	m_pEffect->SetTexture("g_txColor",NULL);
	m_pEffect->SetTexture("g_txColor1",NULL);

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



