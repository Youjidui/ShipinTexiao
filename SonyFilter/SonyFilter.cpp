#include "StdAfx.h"
#include "SonyFilter.h"
#include "../Utility/ColorConvertor.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/mathmacros.h"

#pragma warning(disable:4244)

CSonyFilter::CSonyFilter(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
, m_pShowKeyEffect(NULL)
{
}

CSonyFilter::~CSonyFilter(void)
{
}

bool CSonyFilter::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/EN_SonyScaleFilter.fx"));
	ASSERT(m_pEffect);
	m_pShowKeyEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/EN_ShowKey.fx"));
	ASSERT(m_pShowKeyEffect);
	//resID = CreateTexture(16,2,D3DUSAGE_DYNAMIC,D3DFMT_A16B16G16R16F,D3DPOOL_DEFAULT,NULL);
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	VideoBufferInfo mediaBI = {D3DFMT_A16B16G16R16F, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 16, 2, 0, 0};
	m_pSonyFilterCoefTex = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(m_pSonyFilterCoefTex);

	return true;
}

void CSonyFilter::Uninit()
{
	if(m_pSonyFilterCoefTex)
	{
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		pBufMgr->ReleaseVideoBuffer(m_pSonyFilterCoefTex);
		m_pSonyFilterCoefTex = NULL;
	}
}

/*
πÊ‘Ú£∫
pRcBound:Render Bound,default (0,0,EditWidth,EditHeigh)
*/
bool CSonyFilter::Render( CVideoBuffer* pDstDef, CVideoBuffer* pSrcDef, FxParamBase* pParamRaw )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	SonyFilterFxParam* pParam = (SonyFilterFxParam*)pParamRaw;
	double dbSacleX = pParam->fSacleX;
	double dbSacleY = pParam->fSacleY;
	int nTaps = pParam->nTaps;
	RECT * pRcBound = &pParam->rcBound;

	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo(); 
	if(int(biSrc.nWidth * dbSacleX + 0.5f) == 0 || int(biSrc.nHeight * dbSacleY + 0.5f) == 0)
	{
		m_pEngine->SetRenderTarget(pDstDef);
		return TRUE;
	}

	RECT rcImage = {0, 0, biSrc.nWidth, biSrc.nHeight};
	float pSrcDef_OffsetX = 0.f, pSrcDef_OffsetY = 0.f;
	//D3DXVECTOR4 vvDst(pParam->vector4[0], pParam->vector4[1], pParam->vector4[2], pParam->vector4[3]);
	D3DXVECTOR4& vvDst = pParam->vector4;
	//srcDef.pAppend = (void*)&vvDst;
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float fPixelAspect = nEditWidth * 1.0f / nEditHeight;	

	RECT rcBorder;
	if(pRcBound == NULL)
		SetRect(&rcBorder, 0, 0, nEditWidth, nEditHeight);
	else
		memcpy(&rcBorder,pRcBound,sizeof(RECT));

	LPD3DXEFFECT pEffect = m_pEffect->GetFxPtr();
	pEffect->SetTexture("g_txCoef",m_pSonyFilterCoefTex->GetTexture());

	//RECT rcResult = pSrcDef->rcImage;
	RECT rcResult = {0, 0, biSrc.nWidth, biSrc.nHeight};

	BOOL bRenderX = int((dbSacleX - 1.0) * 1e4) != 0;
	BOOL bRenderY = int((dbSacleY - 1.0) * 1e4) != 0;

	//D3DXCOLOR crClear = pDstDef->COLOR_BLACK();
	D3DXCOLOR crClear(0, 0, 0, 0);
	D3DXMATRIXA16 matWorld,matCombine,matTex;	
	D3DXMATRIX *matView = NULL, *matProj= NULL;
	pResMgr->GetPerspectiveMatrix( &matView, &matProj);

	//X
	//CBaseTexture* pYUVATexture = (CBaseTexture*)pSrcDef->pContainer;
	CVideoBuffer* pYUVATexture = pSrcDef;
	if(bRenderX)
	{
		if(bRenderY)
		{
			//pYUVATexture = m_pResManager->GetTemp_Video(0,FALSE);
			CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
			VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, nEditWidth, nEditHeight, 0, 0};
			pYUVATexture = pBufMgr->CreateVideoBuffer(mediaBI);
			ASSERT(pYUVATexture);
		}
		else
		{
			//pYUVATexture = (CBaseTexture*)pDstDef->pContainer;
			pYUVATexture = pDstDef;
		}

		//LPDIRECT3DSURFACE9 pSurf = NULL;
		//pYUVATexture->GetTexture()->GetSurfaceLevel(0,&pSurf);
		//pDevice->SetRenderTarget(0,pSurf);
		//SAFE_RELEASE(pSurf);
		//pDevice->Clear(0,NULL,D3DCLEAR_TARGET,crClear,0,1);
		m_pEngine->SetRenderTarget(pYUVATexture);
		D3DVIEWPORT9 vPort;
		pDevice->GetViewport(&vPort);
		vPort.X = rcBorder.left;		
		vPort.Width = rcBorder.right - rcBorder.left;
		if(!bRenderY)
		{
			vPort.Y = rcBorder.top;
			vPort.Height = rcBorder.bottom - rcBorder.top;
		}
		pDevice->SetViewport(&vPort);

		CalcSonyFilterCoef(dbSacleX,nTaps,0);	

		D3DXMatrixIdentity(&matWorld);
		if(pParam->emPosDefMode == SonyFilterFxParam::POS_SONY_PINP)
		{
			//D3DXVECTOR4 * vDst = (D3DXVECTOR4*)pSrcDef->pAppend;
			D3DXVECTOR4 * vDst = &vvDst;
			matWorld._11 = (vDst->z - vDst->x) * dbSacleX / (float)vPort.Width;

			if(bRenderY)
				matWorld._22 = biSrc.nHeight / (float)vPort.Height;
			else
				matWorld._22 = (vDst->w - vDst->y) / (float)vPort.Height;
			}	
		else
		{
			matWorld._11 = biSrc.nWidth * dbSacleX / (float)vPort.Width;
			matWorld._22 = biSrc.nHeight / (float)vPort.Height;
		}

		if(pParam->emPosDefMode == SonyFilterFxParam::POS_CUSTOM)
		{
			matWorld._41 = (matWorld._11 - 1.0f) / 2.0f + pSrcDef_OffsetX;
			if(!bRenderY)
				matWorld._42 = - ((matWorld._22 - 1.0f) / 2.0f + pSrcDef_OffsetY );
		}
		else
		{
			matWorld._41 = pSrcDef_OffsetX - 0.5f;
			if(!bRenderY)
				matWorld._42 = 0.5f - pSrcDef_OffsetY;
		}

		//matCombine = matWorld * *m_matView * *m_matPrj;
		D3DXMATRIXA16 matCombine = matWorld * (*matView) * (*matProj);

		pEffect->SetTechnique(nTaps == 4 ? "P4" : "P8");
		pEffect->SetMatrix("g_matWVP", &matCombine);

		D3DXMatrixIdentity(&matTex);	
		if(pParam->emPosDefMode == SonyFilterFxParam::POS_SONY_PINP)
		{
			//D3DXVECTOR4 * vDst = (D3DXVECTOR4*)pSrcDef->pAppend;
			D3DXVECTOR4 * vDst = &vvDst;
			matTex._11 = (vDst->z - vDst->x) / (float)biSrc.nAllocWidth;
			matTex._31 = vDst->x / (float)biSrc.nAllocWidth;
			matTex._41 = (0.5f + vDst->x) / biSrc.nAllocWidth;
			matTex._43 = (vDst->z - 0.5f) / biSrc.nAllocWidth;
			
			if(bRenderY)
			{
				matTex._22 = biSrc.nHeight / (float)biSrc.nAllocHeight;			
				matTex._32 = rcImage.top / (float)biSrc.nAllocHeight;
				matTex._42 = (0.5f + rcImage.top) / biSrc.nAllocHeight;
				matTex._44 = (rcImage.bottom - 0.5f) / biSrc.nAllocHeight;
			}
			else
			{
				matTex._22 = (vDst->w - vDst->y) / (float)biSrc.nAllocHeight;			
				matTex._32 = vDst->y / (float)biSrc.nAllocHeight;		
				matTex._42 = (0.5f + vDst->y) / biSrc.nAllocHeight;			
				matTex._44 = (vDst->w - 0.5f) / biSrc.nAllocHeight;
			}
		}
		else
		{
			matTex._11 = biSrc.nWidth / (float)biSrc.nAllocWidth;
			matTex._22 = biSrc.nHeight / (float)biSrc.nAllocHeight;
			matTex._31 = rcImage.left / (float)biSrc.nAllocWidth;
			matTex._32 = rcImage.top / (float)biSrc.nAllocHeight;

			matTex._41 = (0.5f + rcImage.left) / biSrc.nAllocWidth;
			matTex._42 = (0.5f + rcImage.top) / biSrc.nAllocHeight;
			matTex._43 = (rcImage.right - 0.5f) / biSrc.nAllocWidth;
			matTex._44 = (rcImage.bottom - 0.5f) / biSrc.nAllocHeight;
		}

		pEffect->SetMatrix("g_matTex",&matTex);
		D3DXVECTOR4 vTexSize(biSrc.nAllocWidth, biSrc.nAllocHeight, 0.0f, 0.0f);
		vTexSize.z = 1.0f / vTexSize.x;
		vTexSize.w = 1.0f / vTexSize.y;
		pEffect->SetVector("g_txSize",&vTexSize);

		pEffect->SetTexture("g_txColor",pSrcDef->GetTexture());

		if(SUCCEEDED(pDevice->BeginScene()))
		{
			DWORD bufferFormat = FMT_RGBA32;
			UINT cPass;
			pEffect->Begin(&cPass,0);
			pEffect->BeginPass(bufferFormat == FMT_YUYV ? 1 : 0);
			m_pQuadMesh->DrawMeshFx();
			pEffect->EndPass();
			pEffect->End();
			pDevice->EndScene();
		}

		CalcBox(&matWorld, matView, matProj, &rcResult);
	}
	//D3DXSaveTextureToFile(L"C:\\AAbX.bmp",D3DXIFF_BMP,pYUVATexture->GetTexture() ,NULL);	
	//Y	
	if(bRenderY)
	{
		// added by wxb 2011/11/3 ±ﬂ‘µøπ…¡À∏
		if(dbSacleY < 1.0f && pParam->emPosDefMode == SonyFilterFxParam::POS_SONY_PINP)
		{    
			//D3DXVECTOR4 * vDst = (D3DXVECTOR4*)pSrcDef->pAppend;
			D3DXVECTOR4 * vDst = &vvDst;
			vDst->y --;
			vDst->w ++;
			dbSacleY += 2.0f / (vDst->w - vDst->y);

			pSrcDef_OffsetY -= 1.0f / nEditHeight;
		}
		// end ±ﬂ‘µøπ…¡À∏

		//LPDIRECT3DSURFACE9 pSurf = NULL;
		//((CBaseTexture*)pDstDef->pContainer)->GetTexture()->GetSurfaceLevel(0,&pSurf);
		//pDevice->SetRenderTarget(0,pSurf);
		//SAFE_RELEASE(pSurf);
		//pDevice->Clear(0,NULL,D3DCLEAR_TARGET,crClear,0,1);
		m_pEngine->SetRenderTarget(pDstDef);
		D3DVIEWPORT9 vPort;
		pDevice->GetViewport(&vPort);
		vPort.X = rcBorder.left;		
		vPort.Width = rcBorder.right - rcBorder.left;		
		vPort.Y = rcBorder.top;
		vPort.Height = rcBorder.bottom - rcBorder.top;

		pDevice->SetViewport(&vPort);

		if(nTaps == 4)
			CalcSonyFilterCoef(dbSacleY,nTaps,0);	
		else
		{
			if(dbSacleY > 1.0f / 1.5f)
				CalcSonyFilterCoef(1.0,nTaps,0);
			else
				CalcSonyFilterCoef(dbSacleY * 1.5,nTaps,0);
		}

		D3DXMatrixIdentity(&matWorld);	

		float fOffset = (0.25f / dbSacleY - 0.25f);
		//if(pSrcDef->bFirstField)
		//	fOffset *= -1;
		//if(!m_pProfile->bInterval)
		//	fOffset = 0.0f;
		
		if(pParam->emPosDefMode == SonyFilterFxParam::POS_SONY_PINP)
		{
			//D3DXVECTOR4 * vDst = (D3DXVECTOR4*)pSrcDef->pAppend;
			D3DXVECTOR4 * vDst = &vvDst;
			if(bRenderX)
				matWorld._11 = (rcResult.right - rcResult.left) / (float)vPort.Width;
			else
				matWorld._11 = (vDst->z - vDst->x) / (float)vPort.Width;
			matWorld._22 = (vDst->w - vDst->y) * dbSacleY / (float)vPort.Height;	
		}
		else
		{
			matWorld._11 = (rcResult.right - rcResult.left) / (float)vPort.Width;
			matWorld._22 = (rcResult.bottom - rcResult.top) * dbSacleY / (float)vPort.Height;	
		}

		if(pParam->emPosDefMode == SonyFilterFxParam::POS_CUSTOM)
		{
			if(bRenderX)
				matWorld._41 = (matWorld._11 - 1.0f) / 2.0f + rcResult.left / (float)vPort.Width;	
			else
				matWorld._41 = (matWorld._11 - 1.0f) / 2.0f + pSrcDef_OffsetX ;				

			matWorld._42 = - ((matWorld._22 - 1.0f) / 2.0f + pSrcDef_OffsetY);
		}
		else
		{
			if(bRenderX)
				matWorld._41 = (matWorld._11 - 1.0f) / 2.0f + rcResult.left / (float)vPort.Width;	
			else
				matWorld._41 = pSrcDef_OffsetX - 0.5f;			

			matWorld._42 = 0.5f - pSrcDef_OffsetY;
		}

		matCombine = matWorld * (*matView) * (*matProj);

		pEffect->SetFloat("g_fOffset",fOffset);

		pEffect->SetMatrix("g_matWVP",&matCombine);

		const VideoBufferInfo& biMedia = pYUVATexture->GetVideoBufferInfo();
		D3DXMatrixIdentity(&matTex);
		if(pParam->emPosDefMode == SonyFilterFxParam::POS_SONY_PINP)
		{	
			//D3DXVECTOR4 * vDst = (D3DXVECTOR4*)pSrcDef->pAppend;
			D3DXVECTOR4 * vDst = &vvDst;
			if(bRenderX)
			{
				matTex._11 = (rcResult.right - rcResult.left) / (float)biMedia.nAllocWidth;				
				matTex._31 = rcResult.left / (float)biMedia.nAllocWidth;
				matTex._41 = (0.5f + rcResult.left) / biMedia.nAllocWidth;
				matTex._43 = (rcResult.right - 0.5f) / biMedia.nAllocWidth;
			}
			else
			{
				matTex._11 = (vDst->z - vDst->x) / (float)biMedia.nAllocWidth;				
				matTex._31 = vDst->x / (float)biMedia.nAllocWidth;
				matTex._41 = (0.5f + vDst->x) / biMedia.nAllocWidth;
				matTex._43 = (vDst->z - 0.5f) / biMedia.nAllocWidth;
			}

			float fYOffset = rcResult.top - rcImage.top;
			matTex._22 = (vDst->w - vDst->y) / (float)biMedia.nAllocHeight;
			matTex._32 = (vDst->y + fYOffset) / (float)biMedia.nAllocHeight;			
			matTex._42 = (0.5f + vDst->y + fYOffset) / biMedia.nAllocHeight;
			matTex._44 = (vDst->w + fYOffset - 0.5f) / biMedia.nAllocHeight;
		}
		else
		{
			matTex._11 = (rcResult.right - rcResult.left) / (float)biMedia.nAllocWidth;
			matTex._22 = (rcResult.bottom - rcResult.top) / (float)biMedia.nAllocHeight;
			matTex._31 = rcResult.left / (float)biMedia.nAllocWidth;
			matTex._32 = rcResult.top / (float)biMedia.nAllocHeight;

			matTex._41 = (0.5f + rcResult.left) / biMedia.nAllocWidth;
			matTex._42 = (0.5f + rcResult.top) / biMedia.nAllocHeight;
			matTex._43 = (rcResult.right - 0.5f) / biMedia.nAllocWidth;
			matTex._44 = (rcResult.bottom - 0.5f) / biMedia.nAllocHeight;
		}
		pEffect->SetTechnique(nTaps == 4 ? "P4" : "P8");
		pEffect->SetMatrix("g_matTex",&matTex);
		pEffect->SetTexture("g_txColor",pYUVATexture->GetTexture());

		D3DXVECTOR4 vTexSize = D3DXVECTOR4(biMedia.nAllocWidth, biMedia.nAllocHeight, 0.0f, 0.0f);
		vTexSize.z = 1.0f / vTexSize.x;
		vTexSize.w = 1.0f / vTexSize.y;
		pEffect->SetVector("g_txSize",&vTexSize);

		if(SUCCEEDED(pDevice->BeginScene()))
		{
			UINT cPass;
			pEffect->Begin(&cPass,0);
			pEffect->BeginPass(2);
			m_pQuadMesh->DrawMeshFx();
			pEffect->EndPass();
			pEffect->End();
			pDevice->EndScene();
		}
		CalcBox(&matWorld, matView, matProj, &rcResult);
	}
	if(!bRenderX && !bRenderY)
	{	
		//LPDIRECT3DSURFACE9 pSurf = NULL;
		//((CBaseTexture*)pDstDef->pContainer)->GetTexture()->GetSurfaceLevel(0,&pSurf);
		//pDevice->SetRenderTarget(0,pSurf);
		//SAFE_RELEASE(pSurf);
		//pDevice->Clear(0,NULL,D3DCLEAR_TARGET,crClear,0,1);
		m_pEngine->SetRenderTarget(pDstDef);

		D3DVIEWPORT9 vPort;
		pDevice->GetViewport(&vPort);
		vPort.X = rcBorder.left;		
		vPort.Width = rcBorder.right - rcBorder.left;		
		vPort.Y = rcBorder.top;
		vPort.Height = rcBorder.bottom - rcBorder.top;
		pDevice->SetViewport(&vPort);

		D3DXMATRIXA16 matWorld,matCombine;	
		D3DXMatrixIdentity(&matWorld);
		matWorld._11 = biSrc.nWidth * dbSacleX / (float)vPort.Width;
		matWorld._22 = biSrc.nHeight / (float)vPort.Height;

		if(pParam->emPosDefMode == SonyFilterFxParam::POS_CUSTOM)
		{
			matWorld._41 = (matWorld._11 - 1.0f) / 2.0f + pSrcDef_OffsetX;		
			matWorld._42 = - ((matWorld._22 - 1.0f) / 2.0f + pSrcDef_OffsetY);
		}
		else
		{
			matWorld._41 = pSrcDef_OffsetX - 0.5f;
			matWorld._42 = 0.5f - pSrcDef_OffsetY;
		}

		matCombine = matWorld * (*matView) * (*matProj);

		D3DXMATRIXA16 matTex;
		D3DXMatrixIdentity(&matTex);	
		matTex._11 = biSrc.nWidth / (float)biSrc.nAllocWidth;
		matTex._22 = biSrc.nHeight / (float)biSrc.nAllocHeight;
		matTex._31 = (rcImage.left + 0.5f) / (float)biSrc.nAllocWidth;
		matTex._32 = (rcImage.top + 0.5f) / (float)biSrc.nAllocHeight;

		LPD3DXEFFECT pEffect = m_pShowKeyEffect->GetFxPtr();
		pEffect->SetTechnique("DirectOut");
		pEffect->SetMatrix("g_matWVP",&matCombine);
		
		pEffect->SetMatrix("g_matTex",&matTex);

		pEffect->SetTexture("g_txColor",pSrcDef->GetTexture());

		if(SUCCEEDED(pDevice->BeginScene()))
		{
			UINT cPass;
			pEffect->Begin(&cPass,0);
			pEffect->BeginPass(0);
			m_pQuadMesh->DrawMeshFx();
			pEffect->EndPass();
			pEffect->End();
			pDevice->EndScene();
		}
		CalcBox(&matWorld, matView, matProj, &rcResult);
	}
	//pDstDef->rcImage = rcResult;
	//pDstDef->OffsetX = pDstDef->rcImage.left;
	//pDstDef->OffsetY = pDstDef->rcImage.top;

	if(pYUVATexture != pDstDef)
	{
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		pBufMgr->ReleaseVideoBuffer(pYUVATexture);
		pYUVATexture = NULL;
	}

	//D3DXSaveTextureToFile(L"C:\\AAbY.bmp",D3DXIFF_BMP,((CBaseTexture*)pDstDef->pContainer)->GetTexture() ,NULL);	
	return TRUE;
}

void CSonyFilter::CalcBox(const D3DXMATRIX* matWorld,const D3DXMATRIX* matView,const D3DXMATRIX* matProj,RECT * rcImage)
{
	D3DXVECTOR3 vMesh[4]=
	{
		D3DXVECTOR3(-0.5f,-0.5f,0.0f),
			D3DXVECTOR3(-0.5f,0.5f,0.0f),
			D3DXVECTOR3(0.5f,0.5f,0.0f),
			D3DXVECTOR3(0.5f,-0.5f,0.0f),
	};
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	D3DVIEWPORT9 vPort;
	pDevice->GetViewport(&vPort);
	D3DXVec3ProjectArray(vMesh,sizeof(D3DXVECTOR3),vMesh,sizeof(D3DXVECTOR3),&vPort,matProj,matView,matWorld,4);
	D3DXVECTOR3 vMin,vMax;
	D3DXComputeBoundingBox(vMesh,4,sizeof(D3DXVECTOR3),&vMin,&vMax);
	//RECT rcImage;
	SetRect(rcImage,CEIL(vMin.x),CEIL(vMin.y),CEIL(vMax.x),CEIL(vMax.y));
	rcImage->left = CLAMP(rcImage->left,0,vPort.Width);
	rcImage->top = CLAMP(rcImage->top,0,vPort.Height);
	rcImage->right = CLAMP(rcImage->right,0,vPort.Width);
	rcImage->bottom = CLAMP(rcImage->bottom,0,vPort.Height);
}

float BesselFunc( float b )
{
	float Ib = 1.f;
	float rFactorial = 1.f;
	for ( int r=1; r<=10; ++r )	// I(b) = 1 + sum[r = 1,10](((b / 2)^r / r!)^2)
	{
		rFactorial *= r;
		float fTemp = powf(b/2.f,r) / rFactorial;
		Ib += fTemp * fTemp;
	}
	return Ib;
}

void CSonyFilter::CalcSonyFilterCoef(double dbScale,int nTaps,int nPattern/*0 other 1 PageRoll*/)
{
	float		coef[16][8];
	double		kaiser[8];	

	if(nPattern == 0)
	{
		int i,j,k;
		double d,w,o;
		d = 3.14159 * dbScale;
		if(d > 3.14159) d = 3.14159;

		if(nTaps== 4)
		{
			kaiser[0] = 0.60997 * 2048.0;
			kaiser[1] = 0.95074 * 2048.0;
			kaiser[2] = 0.95074 * 2048.0;
			kaiser[3] = 0.60997 * 2048.0;
			//
			for(j = 0;j < 16;j++)
			{
				o = double(j) / 16.0;
				for(i = 0;i < 4;i++){
					w = (double(i - 1) - o) * d;
					if(w == 0.0) coef[j][i] = 2048;
					else coef[j][i] = int(sin(w) / w * kaiser[i]);
				}
				k = 0;
				for(i = 0;i < 4;i++){
					k += coef[j][i];
				}
				for(i = 0;i < 4;i++){
					coef[j][i] = coef[j][i] * 2048 / k;	// normalize
				}
				coef[j][1] = 2048 - coef[j][0] - coef[j][2] - coef[j][3];
			}
			for(int j = 0;j < 16 ;j ++)
			{
				for(int i = 0; i < 4; i ++)
				{
					coef[j][i] /= 2048.0f;
				}
			}
		}
		else if(nTaps== 8)
		{

			kaiser[0] = 0.251731 * 2048.0;
			kaiser[1] = 0.534026 * 2048.0;
			kaiser[2] = 0.808136 * 2048.0;
			kaiser[3] = 0.977194 * 2048.0;
			kaiser[4] = 0.977194 * 2048.0;
			kaiser[5] = 0.808136 * 2048.0;
			kaiser[6] = 0.534026 * 2048.0;
			kaiser[7] = 0.251731 * 2048.0;
			/*
			const int	  N2 = 8;
			//b = 3.5f;
			float b = 4.3f;

			float Ib = BesselFunc( b );
			for ( i=0; i<N2; ++i )
			{
			float n = -3.5f + i;
			float bn = b * sqrtf( 1 - (2*n/N2)*(2*n/N2) );
			kaiser[i] = BesselFunc(bn) / Ib * 2048.0;
			}
			*/
			//
			for(j = 0;j < 16;j++)
			{
				o = double(j) / 16.0;
				for(i = 0;i < 8;i++)
				{
					w = (double(i - 3) - o) * d;
					if(w == 0.0) coef[j][i] = 2048;
					else coef[j][i] = int(sin(w) / w * kaiser[i]);
				}
				k = 0;
				for(i = 0;i < 8;i++)
				{
					k += coef[j][i];
				}
				for(i = 0;i < 8;i++)
				{
					coef[j][i] = coef[j][i] * 2048 / k;	// normalize
				}
				coef[j][3] = 2048 - coef[j][0] - coef[j][1] - coef[j][2] - coef[j][4] - coef[j][5] - coef[j][6] - coef[j][7];				
			}
		}	

	}
	else //Pattern == 1
	{
		if(nTaps == 4)
		{
			float fixcoef[16][8] = 
			{   // Cutoff = 1/2  K = 3.5
// 				276,1121,650,0,0,0,0,0,
// 				267,1053,712,14,0,0,0,0,
// 				245,1032,741,29,0,0,0,0,
// 				223,1010,768,44,0,0,0,0,
// 				203,988,795,60,0,0,0,0,
// 				183,966,821,76,0,0,0,0,
// 				163,943,847,92,0,0,0,0,
// 				145,920,872,109,0,0,0,0,
// 				127,896,896,127,0,0,0,0,
// 				109,872,920,145,0,0,0,0,
// 				92,847,943,163,0,0,0,0,
// 				76,821,966,183,0,0,0,0,
// 				60,795,988,203,0,0,0,0,
// 				44,768,1010,223,0,0,0,0,
// 				29,741,1032,245,0,0,0,0,
// 				14,712,1053,267,0,0,0,0,
				// lanczos a = 2 cut = 0.5
				{0.267043f,		0.465914f,	0.267043f,	0},
				{0.244556f,		0.460979f,	0.284701f,	0.00976424f},
				{0.222628f,		0.454824f,	0.302024f,	0.020524f},
				{0.201314f,		0.447476f,	0.318928f,	0.0322827f},
				{0.180667f,		0.438968f,	0.335327f,	0.0450376f},
				{0.160741f,		0.429341f,	0.351139f,	0.05878f},
				{0.141583f,		0.418641f,	0.366281f,	0.0734951f},
				{0.123241f,		0.406923f,	0.380675f,	0.0891622f},
				{0.105755f,		0.394245f,	0.394245f,	0.105755f},
				{0.0891622f,	0.380675f,	0.406923f,	0.123241f},
				{0.0734951f,	0.366281f,	0.418641f,	0.141583f},
				{0.05878f,		0.351139f,	0.429341f,	0.160741f},
				{0.0450376f,	0.335327f,	0.438968f,	0.180667f},
				{0.0322827f,	0.318928f,	0.447476f,	0.201314f},
				{0.020524f,		0.302024f,	0.454824f,	0.222628f},
				{0.00976424f,	0.284701f,	0.460979f,	0.244556f},
			};
			memcpy(coef,fixcoef,sizeof(float) * 16 * 8);
		}
		else if(nTaps == 8)
		{
			float fixcoef[16][8] = 
			{		// Cutoff = 1/2  K = 3.5
				-55,0,534,1039,646,0,-117,0,
					-54,-16,504,1021,691,27,-120,-4,
					-52,-32,467,1012,726,55,-120,-8,
					-49,-46,430,1000,761,85,-120,-12,
					-47,-59,393,986,793,116,-118,-16,
					-44,-71,357,969,824,148,-115,-21,
					-40,-82,320,950,853,181,-111,-25,
					-37,-91,285,929,881,215,-105,-29,
					-33,-99,249,906,906,249,-99,-33,
					-29,-105,215,881,929,285,-91,-37,
					-25,-111,181,853,950,320,-82,-40,
					-21,-115,148,824,969,357,-71,-44,
					-16,-118,116,793,986,393,-59,-47,
					-12,-120,85,761,1000,430,-46,-49,
					-8,-120,55,726,1012,467,-32,-52,
					-4,-120,27,691,1021,504,-16,-54
			};			
			memcpy(coef,fixcoef,sizeof(float) * 16 * 8);
		}		
	}
	LPDIRECT3DTEXTURE9 pCoefTex = m_pSonyFilterCoefTex->GetTexture();

	D3DLOCKED_RECT lr;
	pCoefTex->LockRect(0,&lr,NULL,0);

	if(nTaps== 4)
	{
// 		for(int j = 0;j < 16 ;j ++)
// 		{
// 			for(int i = 0; i < 4; i ++)
// 			{
// 				coef[j][i] /= 2048.0f;
// 			}
// 		}
		D3DXVECTOR4_16F * pData = (D3DXVECTOR4_16F*)((byte*)lr.pBits);
		for(int i = 0;i < 16;i ++)
			pData[i] = (float*)&coef[i][0];		
	}
	else if(nTaps == 8)
	{
		for(int j = 0;j < 16;j ++)
		{
			for(int i = 0; i < 8; i ++)
			{
				coef[j][i] /= 2048.0f;
			}
		}
		
		for(int j = 0;j < 2;j ++)
		{
			D3DXVECTOR4_16F * pData = (D3DXVECTOR4_16F*)((byte*)lr.pBits + lr.Pitch * j);
			for(int i = 0;i < 16;i ++)
				pData[i] = (float*)&coef[i][j * 4];

		}		
	}
	pCoefTex->UnlockRect(0);
}
