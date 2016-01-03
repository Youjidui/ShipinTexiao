#include "StdAfx.h"
#include "SonyPinpRender.h"
#include "../Utility/ColorConvertor.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/mathmacros.h"
#include "../SonyFilter/SonyFilter.h"

#pragma warning(disable:4244)

static
VOID WINAPI ColorFill (D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
					   const D3DXVECTOR2* pTexelSize, LPVOID pData)
{

	*pOut = D3DXVECTOR4((rand()%10001)/10001.0f, (rand()%10001)/10001.0f, (rand()%10001)/10001.0f,(rand()%10001)/10001.0f);
	if(pData)
	{
		D3DXCOLOR src = D3DXCOLOR(pOut->x,pOut->y,pOut->z,pOut->w),dest;
		//CRenderEngine* pEngine = (CRenderEngine*)pData;
		//pEngine->ColorConv_RGBA_YUVA(&src,&dest);
		ColorConvertor::RGBA2YUVA(&src,&dest);
		*pOut = D3DXVECTOR4(dest.r,dest.g,dest.b,dest.a);
	}

}


CSonyPinpRender::CSonyPinpRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pMesh(NULL)
, m_pBorderMesh(NULL)
, m_pDecayMesh(NULL)
, m_pPinPEffect(NULL)
, m_pDirectOutEffect(NULL)
, m_pNoiseTexture(NULL)
{
}

CSonyPinpRender::~CSonyPinpRender(void)
{
	Uninit();
}

bool CSonyPinpRender::Init( CRenderEngine* pEngine )
{
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tanf(D3DX_PI/8) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );	
	D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 1.0f, 0.1f, 1000.0f );

	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pPinPEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/SonyPinP.fx"));
	ASSERT(m_pPinPEffect);
	m_pDirectOutEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/fx_GDirectOut.fx"));
	ASSERT(m_pDirectOutEffect);
#ifndef EFFECT_PRLOAD_RESOURCE
	srand(1000);
	//m_uResID_Noise = m_pResMan->CreateTexture(512,256,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&UUID_TEXTURE_NOISE);
	//LPDIRECT3DTEXTURE9 pNoiseTex = ((CBaseTexture*)m_pResMan->GetResource(m_uResID_Noise))->GetTexture();
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 512, 256, 0, 0};
	m_pNoiseTexture = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(m_pNoiseTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pNoiseTexture->GetTexture();
	ASSERT(pTex);
	BOOL bRGBA2YUVA = false;
	if (FAILED (D3DXFillTexture (pTex, &ColorFill, (LPVOID)bRGBA2YUVA)))
	{
		return FALSE;
	}
#endif
	return true;
}

void CSonyPinpRender::Uninit()
{
	SAFE_RELEASE(m_pDecayMesh);
	if(m_pNoiseTexture)
	{
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		pBufMgr->ReleaseVideoBuffer(m_pNoiseTexture);
		m_pNoiseTexture = NULL;
	}
}

#define OBJECT_NUM    50

struct VertexObejct
{
	float _n;
};

bool CSonyPinpRender::CreateMesh()
{
	struct Vertex{
		Vertex(){memset (this, 0, sizeof(Vertex));}
		Vertex(float x,float y, float z,
			float u, float v)
		{ _x = x; _y = y; _z = z;
		_u = u; _v = v;    }
		float _x,_y,_z;
		float _u,_v;
	};

	D3DVERTEXELEMENT9 decl[] = {
		{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
		{0,12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
		{1,0, D3DDECLTYPE_FLOAT1,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,1},
		D3DDECL_END()
	};


	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	LPCTSTR pszMeshName = _T("PinPMesh");
	m_pMesh = pResMan->FindMesh(pszMeshName);
	if(!m_pMesh)
	{
		Vertex _ver[4];
		_ver[0] = Vertex(-0.5f,-0.5f, 0.f, 0.f, 1.f);
		_ver[1] = Vertex(-0.5f, 0.5f, 0.f, 0.f, 0.f);
		_ver[2] = Vertex( 0.5f, 0.5f, 0.f, 1.f, 0.f);
		_ver[3] = Vertex( 0.5f,-0.5f, 0.f, 1.f, 1.f);

		WORD pIndex[6] = {0,1,2,0,2,3};
		const WORD * ppIB[1];
		ppIB[0] = pIndex;


		m_pMesh = pResMan->CreateMesh(pDevice,
			_ver,
			4*sizeof(Vertex),
			sizeof(Vertex),
			D3DPT_TRIANGLELIST,
			2,
			0,
			decl,
			ppIB,
			6*sizeof(WORD),
			1,
			pszMeshName);
		ASSERT(m_pMesh);
	}

	LPCTSTR pszBorderMeshName = _T("PinPBorderMesh");
	m_pBorderMesh = pResMan->FindMesh(pszBorderMeshName);
	if(!m_pBorderMesh)
	{
		Vertex BorderVer[8];
		BorderVer[0] = Vertex(-0.5f,-0.5f,0.0f,0.0f,1.f);
		BorderVer[1] = Vertex(-0.5f,0.5f,0.0f,0.0f,0.f);
		BorderVer[2] = Vertex(0.5f,0.5f,0.0f,1.0f,0.f);
		BorderVer[3] = Vertex(0.5f,-0.5f,0.0f,1.0f,1.f);
		BorderVer[4] = Vertex(-0.5f,-0.5f,1.0f,0.0f,1.f);
		BorderVer[5] = Vertex(-0.5f,0.5f,1.0f,0.0f,0.f);
		BorderVer[6] = Vertex(0.5f,0.5f,1.0f,1.0f,0.f);
		BorderVer[7] = Vertex(0.5f,-0.5f,1.0f,1.0f,1.f);
		WORD pIB[12];
		int iRet = 0;	

		pIB[iRet++] = 1;
		pIB[iRet++] = 3;
		pIB[iRet++] = 0;

		pIB[iRet++] = 1;
		pIB[iRet++] = 2;
		pIB[iRet++] = 3;

		pIB[iRet++] = 4;
		pIB[iRet++] = 5;
		pIB[iRet++] = 7;

		pIB[iRet++] = 5;
		pIB[iRet++] = 6;
		pIB[iRet++] = 7;

		const WORD * p[1];
		p[0] = pIB;

		m_pBorderMesh = pResMan->CreateMesh(pDevice,
			BorderVer,
			8*sizeof(Vertex),
			sizeof(Vertex),
			D3DPT_TRIANGLELIST,
			4,
			0,
			decl,
			p,
			12*sizeof(WORD),
			1,
			pszBorderMeshName);
		ASSERT(m_pBorderMesh);
	}

	//LPCTSTR pszDecayMeshName = _T("PinPDecayMesh");
	//m_pDecayMesh = pResMan->FindMesh(pszDecayMeshName);
	if(!m_pDecayMesh)
	{
		//LPD3DXMESH pMesh = NULL;
		//HRESULT hr = D3DXCreateMesh(nNumFace,nNumVertex, D3DXMESH_MANAGED | D3DXMESH_WRITEONLY, declWipe, pDevice, &pMesh);
		HRESULT hr = pDevice->CreateVertexBuffer(OBJECT_NUM*sizeof(VertexObejct),0,0,D3DPOOL_MANAGED,
			&m_pDecayMesh,NULL); 
		ASSERT(SUCCEEDED(hr));

		VertexObejct *verobj;
		hr = m_pDecayMesh->Lock(0,0,(void**)&verobj,0);
		ASSERT(SUCCEEDED(hr));
		for(int i = 0; i < OBJECT_NUM; ++i)
		{
			verobj[i]._n =(float) i;
		}    
		hr = m_pDecayMesh->Unlock();
		ASSERT(SUCCEEDED(hr));
	}
	return true;
}

bool CSonyPinpRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, FxParamBase* pParamRaw)
{
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	CVideoBuffer* pSrcDef = pSrc;
	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo();
	SonyPinPFxParam* pParam = (SonyPinPFxParam*)pParamRaw;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	//float fAspect = m_pResMan->GetAspect() * pProfile->nEditWidth / (float) (pProfile->nEditHeight * m_pResMan->GetAspectVerifyCoef()); 
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;
	float pSrcDef_OffsetX = 0, pSrcDef_OffsetY = 0;

	// zms begin
	D3DXMATRIXA16 m_matPreGuideLine, m_matAfterGuideLine;
	{
		//D3DXVECTOR4	rcImage(pSrcDef->rcImage.left,pSrcDef->rcImage.top,
		//	pSrcDef->rcImage.right,pSrcDef->rcImage.bottom);	// left top right bottom
		D3DXVECTOR4	rcImage(0, 0, biSrc.nWidth, biSrc.nHeight);
		rcImage.x += pParam->fBoundsLeft * biSrc.nWidth / fAspect;
		rcImage.z -= pParam->fBoundsRight * biSrc.nWidth / fAspect;
		rcImage.y += pParam->fBoundsTop * biSrc.nHeight;
		rcImage.w -= pParam->fBoundsBottom * biSrc.nHeight;
		rcImage.x = min(rcImage.x,rcImage.z);
		rcImage.y = min(rcImage.y,rcImage.w);
		float fOffsetX = pSrcDef_OffsetX + rcImage.x - 0;
		float fOffsetY = pSrcDef_OffsetY + rcImage.y - 0;

		float fxZoom = (rcImage.z-rcImage.x) * (1.0f) / (float)nEditWidth;
		float fyZoom = (rcImage.w-rcImage.y) / (float)nEditHeight;	
		float ofx = -0.5f + fxZoom*0.5f + fOffsetX / (float)nEditWidth;
		float ofy =  0.5f - fyZoom*0.5f - fOffsetY / (float)nEditHeight;	

		D3DXMATRIXA16 matPreScaleForGuideLine, matPreTransForGuideLine;
		D3DXMatrixScaling(&matPreScaleForGuideLine, fxZoom, fyZoom , 1.0f);
		D3DXMatrixTranslation(&matPreTransForGuideLine, ofx, ofy, 0.0f);  

		m_matPreGuideLine = matPreScaleForGuideLine * matPreTransForGuideLine;

		float fCenterImageX = (pSrcDef_OffsetX + biSrc.nWidth / 2.f) / (float)nEditWidth;
		float fCenterImageY = (pSrcDef_OffsetY + biSrc.nHeight / 2.f) / (float)nEditHeight;
		fCenterImageX -= 0.5f;
		fCenterImageY = 0.5-fCenterImageY;
		D3DXMATRIXA16 matCenter;
		D3DXMatrixTranslation( &matCenter, -fCenterImageX, -fCenterImageY, 0.f );
		m_matPreGuideLine = m_matPreGuideLine * matCenter;

		D3DXMatrixTranslation( &matCenter, fCenterImageX, fCenterImageY, 0.f );
		m_matAfterGuideLine = matCenter;

		//m_fAspect = fAspect;
	}
	// zms end

	float fScaleX = pParam->fScaleX / 100.0f;
	float fScaleY = pParam->fScaleY / 100.0f;

	D3DXVECTOR4 picBound;
	picBound.x = 0.f;//pSrcDef->OffsetX / (float)(m_pEngine->GetCurProfile()->nEditWidth);
	picBound.y = 1.f;//picBound.x + pSrcDef->GetImageWidth() / (float)(m_pEngine->GetCurProfile()->nEditWidth);
	picBound.z = 0.f;//pSrcDef->OffsetY / (float)(m_pEngine->GetCurProfile()->nEditHeight);
	picBound.w = 1.f;//picBound.z + pSrcDef->GetImageHeight() / (float)(m_pEngine->GetCurProfile()->nEditHeight);

	//g_paramAdjust.SetPicBoundLRTB(picBound);

	//const Custom_Profile *pProfile = m_pEngine->GetCurProfile();
	BOOL bDecay = FALSE;
	//Trail
	if(pParam->iTrailDecayTime > 0)
	{
		D3DXMATRIXA16 matPrevScale, matPrevTransition;
		float fxZoom = biSrc.nWidth * (1.0f) / (float)nEditWidth;
		float fyZoom = biSrc.nHeight / (float)nEditHeight;	
		float ofx = -0.5f + fxZoom * 0.5f + pSrcDef_OffsetX / (float)nEditWidth;
		float ofy =  0.5f - fyZoom * 0.5f - pSrcDef_OffsetY / (float)nEditHeight;	

		D3DXMatrixScaling(&matPrevScale, fxZoom, fyZoom , 1.0f);
		D3DXMatrixTranslation(&matPrevTransition, ofx, ofy, 0.0f);

		D3DXMATRIXA16 matScale,matTrans,matCombine;
		D3DXVECTOR2 vCenter(0,0),vNewCenter[2];
		for(int i = 0; i < 2;i ++)
		{
			D3DXMatrixScaling(&matScale, pParam->fKeyScaleX[i] / 100.0f, pParam->fKeyScaleY[i] / 100.0f, 1.0f);
			D3DXMatrixTranslation(&matTrans, pParam->fKeyPositionX[i], pParam->fKeyPositionY[i], 0.0f);

			matCombine =  matPrevScale *  matScale * matPrevTransition * matTrans;

			D3DXVec2TransformCoord(&vNewCenter[i],&vCenter,&matCombine);
		}


		D3DXVECTOR2 vecMove = vNewCenter[0] - vNewCenter[1];
		if(vecMove.x != 0.0f || vecMove.y != 0.0f)
		{
			float fDencity = 1.0f;
			float fDistance =  D3DXVec2Length(&vecMove) / pParam->nKeyCount * 30.0f;
			float fDust = 1.0f;
			float fRainbow = 0.0f;
			if (pParam->dwTrailDecayType == 0)
			{
				fDust = 0.f;
			}
			D3DXVec2Normalize(&vecMove,&vecMove);
			D3DXVECTOR4 vDir = D3DXVECTOR4(  ceil(vecMove.x * nEditWidth) / (float)nEditWidth, ceil(vecMove.y * nEditHeight) / (float)nEditHeight ,0.0f, 0.0f);
			D3DXVec4Normalize(&vDir,&vDir);
			m_pPinPEffect->SetVector("g_vDir",&vDir);
			D3DXVECTOR4 vMisc = D3DXVECTOR4(fDencity,fDistance ,fDust,fRainbow );
			m_pPinPEffect->SetVector("g_vMisc",&vMisc);

			LPDIRECT3DTEXTURE9 pNoiseTex = m_pNoiseTexture->GetTexture();
			m_pPinPEffect->SetTexture("g_texNoise",pNoiseTex);

			bDecay = TRUE;
		}	
		else
		{
			//assert(0);
		}
	}
	BOOL bAlphaBlend = pParam->dwBackGroundType > 0 || bDecay || (pParam->bEnableShadow && pParam->fShadowDropDistance > 0.f);
	CVideoBuffer* pTempDef = pDst;
	if(bAlphaBlend)
	{
		//hTemp = NewRTBuffer(pDstDef->OffsetX,pDstDef->OffsetY,pDstDef->GetImageWidth(),pDstDef->GetImageHeight(),pDstDef->bIsCGBuffer);
		//pTempDef = m_pResMan->GetBufferDef(hTemp);
		//std::swap(pDstDef->pAlpha,pTempDef->pAlpha);
		VideoBufferInfo biDest = pDst->GetVideoBufferInfo();
		biDest.eUsage = VideoBufferInfo::_IN_OUT;
		CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
		pTempDef = pVM->CreateVideoBuffer(biDest);
	}

	_render_for_rgb32(pTempDef, pSrcDef, pParam, bDecay, bAlphaBlend);

	pDevice->SetRenderTarget(1,NULL);	

	if(pDst != pTempDef)
	{
		ClearCGBlend(pDst, pTempDef);

		//FreeRTBuffer(hTemp);
		CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
		pVM->ReleaseVideoBuffer(pTempDef);
		pTempDef = NULL;
	}

	return true;
}

void CSonyPinpRender::_render_for_rgb32(CVideoBuffer* pDstDef, CVideoBuffer* pSrcDef, SonyPinPFxParam* pParam, BOOL bDecay, BOOL bAlphaBlend)
{
	bool bOK = m_pEngine->SetRenderTarget(pDstDef);
	ASSERT(bOK);

	const VideoBufferInfo& biDest = pDstDef->GetVideoBufferInfo();
	RECT rcDestImage = {0, 0, biDest.nWidth, biDest.nHeight};
	BOOL pDstDef_bNeedBlend = FALSE;
	float pDstDef_fAlphaValue = 1.0f;
	_do_render_scene( pSrcDef, pParam, bDecay, bAlphaBlend, rcDestImage, 0, 0,
		pDstDef_bNeedBlend, pDstDef_bNeedBlend ? pDstDef_fAlphaValue : 1.0f);	
}


void CSonyPinpRender::_do_render_scene(CVideoBuffer* pSrc, SonyPinPFxParam* pParam, BOOL bDecay, BOOL bAlphaBlend, 
									   const RECT& rcDestImage, float offsetX,float offsetY, BOOL bSingleWord,float fAlpha)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	float fScaleX = pParam->fScaleX / 100.0f;
	float fScaleY = pParam->fScaleY / 100.0f;
	float fBorderScale =  pParam->fBorderWidth;
	//float fAspect = m_pResMan->GetAspect() * pProfile->nEditWidth / (float) (pProfile->nEditHeight * m_pResMan->GetAspectVerifyCoef()); 
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float	fAspect = nEditWidth*1.0f/nEditHeight;
	float pSrcDef_OffsetX = 0, pSrcDef_OffsetY = 0;

	float fBorderX = fBorderScale / fAspect * nEditWidth;
	float fBorderY = fBorderScale * nEditHeight;
	float fBoundsLeft = pParam->fBoundsLeft / fAspect;
	float fBoundsRight = pParam->fBoundsRight / fAspect;

	const VideoBufferInfo& biSrc = pSrc->GetVideoBufferInfo();
	RECT rcImage = {0, 0, biSrc.nWidth, biSrc.nHeight};
	int pSrcDef_GetImageWidth = biSrc.nWidth;
	int pSrcDef_GetImageHeight = biSrc.nHeight;
	D3DXMATRIXA16 matSrcTex;
	D3DXMatrixIdentity( &matSrcTex );
	int left = 0, top = 0;
	matSrcTex._11   =  max(0,pSrcDef_GetImageWidth * (1.0f - pParam->fBoundsLeft - pParam->fBoundsRight)) / ((float)biSrc.nAllocWidth);
	matSrcTex._22   =  max(0,pSrcDef_GetImageHeight * (1.0f - pParam->fBoundsTop - pParam->fBoundsBottom)) / ((float)biSrc.nAllocHeight);
	matSrcTex._31 =  (0.5f + left + pSrcDef_GetImageWidth * pParam->fBoundsLeft)   / (float)(biSrc.nAllocWidth);
	matSrcTex._32 =  (0.5f + top + pSrcDef_GetImageHeight * pParam->fBoundsTop)  / (float)(biSrc.nAllocHeight);

	D3DXMATRIXA16 matTransition,matPrevScale, matPrevTransition,matCombine,matWorld,matClip,matborder;
	float fxZoom = max(0,pSrcDef_GetImageWidth * (1.0f - pParam->fBoundsLeft - pParam->fBoundsRight) * fScaleX)  / (float)nEditWidth;
	float fyZoom = max(0,pSrcDef_GetImageHeight * (1.0 - pParam->fBoundsTop - pParam->fBoundsBottom) * fScaleY) / (float)nEditHeight;	

	D3DXMatrixScaling(&matPrevScale, fxZoom, fyZoom , 1.0f);
	D3DXMatrixIdentity(&matborder);
	matborder._11 = max(0,pSrcDef_GetImageWidth * (1.0f - pParam->fBoundsLeft - pParam->fBoundsRight) * fScaleX + 2 * fBorderX) / (float)nEditWidth;
	matborder._22 = max(0, pSrcDef_GetImageHeight * (1.0f - pParam->fBoundsTop - pParam->fBoundsBottom) * fScaleY + 2 * fBorderY) / (float)nEditHeight;

	D3DXMatrixIdentity(&matPrevTransition);
	matPrevTransition._41 = (pSrcDef_OffsetX + pSrcDef_GetImageWidth / 2.0f - nEditWidth / 2.0f) / nEditWidth;
	matPrevTransition._42 = -(pSrcDef_OffsetY + pSrcDef_GetImageHeight / 2.0f - nEditHeight / 2.0f) / nEditHeight;

	D3DXMatrixIdentity(&matClip);
	matClip._41 = (rcImage.left + pSrcDef_GetImageWidth * pParam->fBoundsLeft + rcImage.right - pSrcDef_GetImageWidth  * pParam->fBoundsRight - (rcImage.left + rcImage.right)) / 2.0f / (float)nEditWidth;
	matClip._42 = -(rcImage.top + pSrcDef_GetImageHeight * pParam->fBoundsTop + rcImage.bottom - pSrcDef_GetImageHeight  * pParam->fBoundsBottom - (rcImage.top + rcImage.bottom)) / 2.0f / (float)nEditHeight;
	matClip._41 *= fScaleX;
	matClip._42 *= fScaleY;

	float fTransX = pParam->fPositionX/2.f * nEditWidth;
	float fTransY = pParam->fPositionY/2.f * nEditHeight;
	D3DXMatrixTranslation(&matTransition, fTransX / (float) nEditWidth, fTransY / (float) nEditHeight, 0.0f);

	//m_matPrevScale = matPrevScale;
	//m_matPrevTransition = matPrevTransition;

	//m_matOriginal = matPrevScale  * matClip * matPrevTransition * matTransition;

	HRESULT hr = m_pPinPEffect->SetInt("g_nFilter",(int)(pParam->bFilter ? 2 : 0));
	ASSERT(SUCCEEDED(hr));

	D3DVIEWPORT9 vPort;
	hr = pDevice->GetViewport(&vPort);
	ASSERT(SUCCEEDED(hr));
	vPort.Width = nEditWidth;
	vPort.Height = nEditHeight;
	hr = pDevice->SetViewport(&vPort);
	ASSERT(SUCCEEDED(hr));

	if ( SUCCEEDED(pDevice->BeginScene()))
	{
		hr = pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetRenderState(D3DRS_SRCBLENDALPHA,D3DBLEND_ONE);
		hr = pDevice->SetRenderState(D3DRS_DESTBLENDALPHA,D3DBLEND_INVSRCALPHA);
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetRenderState(D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD);
		ASSERT(SUCCEEDED(hr));

		UINT cPass,uPass;
		int bufferFormat = FMT_RGBA32;
		if(bufferFormat == FMT_RGBA32)
			uPass = 0;
		else
			uPass = 1;

		RECT rcDestImage,rcTemp;
		SetRect(&rcDestImage,0,0,0,0);

		if (pParam->dwBackGroundType > 0)
		{
			m_pPinPEffect->SetTexture("g_tex", pSrc->GetTexture());	
			D3DXMATRIX matBGTex;	
			D3DXMatrixIdentity(&matBGTex);
			matBGTex._11 = biSrc.nWidth / (float) biSrc.nAllocWidth;
			matBGTex._22 = biSrc.nHeight / (float) biSrc.nAllocHeight;
			matBGTex._31 = (0.5f + rcImage.left) / (float) biSrc.nAllocWidth;
			matBGTex._32 = (0.5f + rcImage.top)/ (float) biSrc.nAllocHeight;

			m_pPinPEffect->SetMatrix("g_matTexture",&matBGTex);			
			D3DXMatrixIdentity(&matWorld);
			matWorld._11 = biSrc.nWidth  / (float)vPort.Width;
			matWorld._22 = biSrc.nHeight / (float)vPort.Height;
			matWorld._41 = -0.5f + matWorld._11 / 2.0f + pSrcDef_OffsetX / (float)vPort.Width;
			matWorld._42 = 0.5f  - matWorld._22 / 2.0f - pSrcDef_OffsetY / (float)vPort.Height;   

			matCombine = matWorld * m_matView * m_matProj;
			m_pPinPEffect->SetMatrix("g_matBackGround",&matCombine);

			D3DXVECTOR4 vecBGSepiaColor = (float*)(D3DXCOLOR)pParam->cBackGroundSepiaColor;

			m_pPinPEffect->SetVector("g_SepiaColor",&vecBGSepiaColor);
			m_pPinPEffect->SetInt("g_iBGType", (int)pParam->dwBackGroundType);			
			m_pPinPEffect->SetFloat("g_BGDensity", pParam->fBackGoundDensity/100.f);
			m_pPinPEffect->SetTechnique("BackGround");

			m_pPinPEffect->Begin(&cPass,0);
			m_pPinPEffect->BeginPass(uPass);
			m_pQuadMesh->DrawMeshFx();		  		
			m_pPinPEffect->EndPass();

			m_pPinPEffect->End();		
		}		

		m_pPinPEffect->SetTexture("g_tex", pSrc->GetTexture());
		m_pPinPEffect->SetMatrix("g_matTexture",&matSrcTex);
		//handle_tpr hTemp = NewRTBuffer(0,0,nEditWidth,nEditHeight);
		CVideoBuffer* pTempDef = NULL;
		LPDIRECT3DSURFACE9 pOldRT = NULL;
		if(bDecay)
		{
			VideoBufferInfo biTemp = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, nEditWidth, nEditHeight, 0, 0};
			CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
			pTempDef = pVM->CreateVideoBuffer(biTemp);
			ASSERT(pTempDef);
			hr = pDevice->GetRenderTarget(0,&pOldRT);
			ASSERT(SUCCEEDED(hr));
			bool bOK = m_pEngine->SetRenderTarget(pTempDef);
			ASSERT(bOK);
		}

		if (pParam->bEnableShadow && pParam->fShadowDropDistance > 0.f)
		{
			//shadow 
			D3DXMATRIXA16 matShadow;					

			float fDis  = -pParam->fShadowDropDistance;
			float fAngle= D3DXToRadian( pParam->fShadowDropLightAngle);

			D3DXMatrixTranslation(&matShadow,fDis*cosf(fAngle),fDis*sinf(fAngle),0.f);			

			matWorld = matborder * matClip * matPrevTransition * matTransition * matShadow;				
			matWorld._11 *= (1.f + pParam->fShadowDropSoftness*0.5f);
			matWorld._22 *= (1.f + pParam->fShadowDropSoftness*0.5f);

			matCombine =  matWorld * m_matView * m_matProj;

			m_pPinPEffect->SetMatrix("g_matShadow",&matCombine);
			D3DXCOLOR cShadowColor = pParam->cShadowColor;			
			ColorConvertor::RGBA2( bufferFormat == FMT_RGBA32? FMT_RGBA32:FMT_YUVA32, &cShadowColor, &cShadowColor);
			D3DXVECTOR4 vecShadowColor = (float*)cShadowColor;

			m_pPinPEffect->SetVector("g_ShadowColor",&vecShadowColor);
			m_pPinPEffect->SetFloat("g_ShadowSoftness",pParam->fShadowDropSoftness);
			m_pPinPEffect->SetFloat("g_ShadowTransparency",pParam->fShadowDropTransparency);

			m_pPinPEffect->SetTechnique("Shadow");	

			m_pPinPEffect->Begin(&cPass,0);		
			m_pPinPEffect->BeginPass(0);
			m_pQuadMesh->DrawMeshFx();	  		
			m_pPinPEffect->EndPass();

			m_pPinPEffect->End();

			CalcBox(&matWorld,&rcTemp);
			UnionRect(&rcDestImage,&rcDestImage,&rcTemp);
		}		

		if(pParam->bFilter && (fScaleX < 0.9999f || fScaleY < 0.9999f))
		{
			//handle_tpr hSonyFiltr = NewRTBuffer(0,0,nEditWidth,nEditHeight);
			//TP_VBufferDef *pSonyFilterDef = m_pResMan->GetBufferDef(hSonyFiltr);
			VideoBufferInfo biTemp = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, nEditWidth, nEditHeight, 0, 0};
			CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
			CVideoBuffer* pFilterBuf = pVM->CreateVideoBuffer(biTemp);

			{
				LPDIRECT3DSURFACE9 pOldSurf = NULL;
				pDevice->GetRenderTarget(0,&pOldSurf);
				pDevice->EndScene();

				matWorld = matPrevScale * matClip * matPrevTransition * matTransition;				

				D3DXVECTOR4 vSrc,vDst;
				D3DXVECTOR3 vCorner[2] = {D3DXVECTOR3(-0.5f,0.5f,0.0f),D3DXVECTOR3(0.5f,-0.5f,0.0f)};
				D3DXVec3TransformCoordArray(vCorner,sizeof(D3DXVECTOR3),vCorner,sizeof(D3DXVECTOR3),&matWorld,2);
				vDst = D3DXVECTOR4(vCorner[0].x,vCorner[0].y,vCorner[1].x,vCorner[1].y);

				double dbXScale = (vDst.z - vDst.x) / (matSrcTex._11 * biSrc.nAllocWidth / vPort.Width);
				double dbYScale = -(vDst.w - vDst.y) / (matSrcTex._22 * biSrc.nAllocHeight / vPort.Height);

				vSrc.x = biSrc.nAllocWidth * matSrcTex._31 - 0.5;
				vSrc.y = biSrc.nAllocHeight * matSrcTex._32 - 0.5;
				vSrc.z = biSrc.nAllocWidth * (matSrcTex._11   + matSrcTex._31) - 0.5;
				vSrc.w = biSrc.nAllocHeight * (matSrcTex._22   + matSrcTex._32) - 0.5;

				SonyFilterFxParam param;
				param.emPosDefMode = SonyFilterFxParam::POS_SONY_PINP;
				//SrcDef.bFirstField = m_pParam->bOdd;
				param.OffsetX = -(1.0f - (vDst.z - vDst.x) ) / 2.0f + vDst.x + 0.5f;
				param.OffsetY =  (1.0f + (vDst.w - vDst.y) ) / 2.0f + vDst.y - 0.5f;
				param.OffsetX = 0.5f + param.OffsetX;
				param.OffsetY = 0.5f - param.OffsetY;

				param.fSacleX = dbXScale;
				param.fSacleY = dbYScale;
				param.nTaps = 8;
				SetRect(&param.rcBound, 0, 0, nEditWidth, nEditHeight);

				//m_pEngine->SonyFilter(&SrcDef,pSonyFilterDef,dbXScale,dbYScale,8);
				CSonyFilter render;
				render.Init(m_pEngine);
				render.Render(pFilterBuf, pSrc, &param);
				render.Uninit();

				D3DXSaveSurfaceToFile(_T("./FilterBuf.dds"), D3DXIFF_DDS, pFilterBuf->GetSurface(), NULL, NULL);

				pDevice->SetRenderTarget(0,pOldSurf);
				SAFE_RELEASE(pOldSurf);
				pDevice->BeginScene();
			}

			D3DXMatrixIdentity(&matWorld);
			matWorld._11 = biTemp.nWidth / (float)nEditWidth;
			matWorld._22 = biTemp.nHeight / (float)nEditHeight;
			//matWorld._41 = -(1.0f - matWorld._11) / 2.0f + pSonyFilterDef->OffsetX / nEditWidth;
			//matWorld._42 = (1.0f - matWorld._22) / 2.0f - pSonyFilterDef->OffsetY / nEditHeight;
			matWorld._41 = -(1.0f - matWorld._11) / 2.0f + 0 / nEditWidth;
			matWorld._42 = (1.0f - matWorld._22) / 2.0f - 0 / nEditHeight;

			matCombine =  matWorld * m_matView * m_matProj;						
			m_pPinPEffect->SetMatrix("g_matOriginal", &matCombine);			
			m_pPinPEffect->SetTexture("g_tex", pFilterBuf->GetTexture());

			GenerateMatrix(pFilterBuf, &matSrcTex, mat_Image);

			m_pPinPEffect->SetMatrix("g_matTexture",&matSrcTex);			

			m_pPinPEffect->SetTechnique("ORG");
			m_pPinPEffect->Begin(&cPass,0);
			m_pPinPEffect->BeginPass(bAlphaBlend);
			m_pQuadMesh->DrawMeshFx();			  		
			m_pPinPEffect->EndPass();

			m_pPinPEffect->End();	

			//FreeRTBuffer(hSonyFiltr);
			if(pFilterBuf)
			{
				CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
				pBufMgr->ReleaseVideoBuffer(pFilterBuf);
				pFilterBuf = NULL;
			}

			matWorld = matPrevScale * matClip * matPrevTransition * matTransition;
			CalcBox(&matWorld,&rcTemp);
			UnionRect(&rcDestImage,&rcDestImage,&rcTemp); 	

		}
		else
		{
			matWorld = matPrevScale * matClip * matPrevTransition * matTransition;
			matCombine =  matWorld * m_matView * m_matProj;						
			m_pPinPEffect->SetMatrix("g_matOriginal", &matCombine);			
			m_pPinPEffect->SetTexture("g_tex",pSrc->GetTexture());

			m_pPinPEffect->SetMatrix("g_matTexture",&matSrcTex);			

			m_pPinPEffect->SetTechnique("ORG");
			m_pPinPEffect->Begin(&cPass,0);
			m_pPinPEffect->BeginPass(bAlphaBlend);
			m_pQuadMesh->DrawMeshFx();
			m_pPinPEffect->EndPass();

			m_pPinPEffect->End();	

			CalcBox(&matWorld, &rcTemp);
			UnionRect(&rcDestImage, &rcDestImage, &rcTemp); 	
		}

		if (pParam->fBorderWidth > 0.f)
		{			
			//Border
			matWorld = matborder * matClip * matPrevTransition * matTransition;

			matCombine =  matWorld * m_matView * m_matProj;						
			m_pPinPEffect->SetMatrix("g_matOriginal", &matCombine);		

			D3DXCOLOR cBorderColor = pParam->cBorderColor;			
			ColorConvertor::RGBA2( bufferFormat == FMT_RGBA32? FMT_RGBA32:FMT_YUVA32,&cBorderColor,&cBorderColor);
			D3DXVECTOR4 vecBorderColor = (float*) cBorderColor;
			m_pPinPEffect->SetVector("g_BorderColor",&vecBorderColor);
			pDevice->SetTexture(0,NULL);
			m_pPinPEffect->SetTechnique("Border");
			m_pPinPEffect->Begin(&cPass,0);
			m_pPinPEffect->BeginPass(0);
			m_pQuadMesh->DrawMeshFx();			  		
			m_pPinPEffect->EndPass();

			m_pPinPEffect->End();

			CalcBox(&matWorld,&rcTemp);
			UnionRect(&rcDestImage,&rcDestImage,&rcTemp);				
		}

		if (bDecay)
		{
			pDevice->SetRenderTarget(0, pOldRT);				
			pDevice->SetRenderTarget(1, NULL);
			SAFE_RELEASE(pOldRT);

			pDevice->SetViewport(&vPort);

			ASSERT(pTempDef);
			const VideoBufferInfo& biTemp = pTempDef->GetVideoBufferInfo();

			m_pPinPEffect->SetTexture("g_tex", pTempDef->GetTexture());
			D3DXMATRIX matSrcImage;	
			D3DXMatrixIdentity( &matSrcImage );			

			matSrcImage._11   = (RECTWIDTH(rcDestImage) - 0.50f) /((float)biTemp.nAllocWidth);
			matSrcImage._22   = (RECTHEIGHT(rcDestImage) - 0.50f)/((float)biTemp.nAllocHeight); 

			matSrcImage._31   = (0.5f + rcDestImage.left) /((float)biTemp.nAllocWidth);
			matSrcImage._32   = (0.5f + rcDestImage.top) /((float)biTemp.nAllocHeight); 


			m_pPinPEffect->SetMatrix("g_matTexture", &matSrcImage); 

			D3DXMATRIXA16 matWorld;
			D3DXMatrixIdentity(&matWorld);
			matWorld._11 = RECTWIDTH(rcDestImage) /((float)nEditWidth);
			matWorld._22 = RECTHEIGHT(rcDestImage)/((float)nEditHeight);

			matWorld._41 = -0.5f + matWorld._11 * 0.5f + rcDestImage.left / (float)nEditWidth;
			matWorld._42 = 0.5f - matWorld._22 * 0.5f - rcDestImage.top / (float)nEditHeight;			

			m_pPinPEffect->SetMatrix("g_matViewProj", &(matWorld * m_matView * m_matProj));			

			m_pPinPEffect->SetFloat("g_fInstanceCount",pParam->iTrailDecayTime);

			m_pPinPEffect->SetTechnique("Decay");

			pDevice->SetStreamSourceFreq(0,D3DSTREAMSOURCE_INDEXEDDATA | (pParam->iTrailDecayTime + 1));
			pDevice->SetStreamSource(1,m_pDecayMesh,0,sizeof(VertexObejct));
			pDevice->SetStreamSourceFreq(1,D3DSTREAMSOURCE_INSTANCEDATA | 1ul);

			m_pPinPEffect->Begin(&cPass,0);

			m_pPinPEffect->BeginPass(uPass);
			m_pMesh->DrawMeshFx(0);	  	  		
			m_pPinPEffect->EndPass();

			m_pPinPEffect->End();

			pDevice->SetStreamSourceFreq(0,1ul);
			pDevice->SetStreamSourceFreq(1,1ul);			

			bDecay = FALSE;

			SetRect(&rcDestImage,0,0,nEditWidth,nEditHeight);
		}
		if (pParam->dwBackGroundType > 0)
		{
			bool IsYUV16Buffer = false;
			SetRect(&rcTemp,CEIL(pSrcDef_OffsetX),CEIL(pSrcDef_OffsetY),
				CEIL(pSrcDef_OffsetX) + pSrcDef_GetImageWidth * (IsYUV16Buffer?2.0f:1.0f),
				CEIL(pSrcDef_OffsetY) + pSrcDef_GetImageHeight);
			UnionRect(&rcDestImage,&rcDestImage,&rcTemp);
		}
		//m_rcDestImage = rcDestImage;


		pDevice->EndScene();

		//{
		//	LPDIRECT3DSURFACE9 pOldRT = NULL;
		//	hr = pDevice->GetRenderTarget(0,&pOldRT);
		//	D3DXSaveSurfaceToFile(_T("./DestDef.dds"), D3DXIFF_DDS, pOldRT, NULL, NULL);
		//	pOldRT->Release();
		//}

		//FreeRTBuffer(hTemp);
		if(pTempDef)
		{
			//D3DXSaveSurfaceToFile(_T("./TempDef.dds"), D3DXIFF_DDS, pTempDef->GetSurface(), NULL, NULL);

			CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
			pBufMgr->ReleaseVideoBuffer(pTempDef);
			pTempDef = NULL;
		}

		pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,FALSE);
	} 
}

void CSonyPinpRender::CalcBox(IN D3DXMATRIXA16 * matWorld, OUT RECT * rcImage)
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
	D3DXVec3ProjectArray(vMesh,sizeof(D3DXVECTOR3),vMesh,sizeof(D3DXVECTOR3),&vPort,&m_matProj,&m_matView,matWorld,4);
	D3DXVECTOR3 vMin,vMax;
	D3DXComputeBoundingBox(vMesh,4,sizeof(D3DXVECTOR3),&vMin,&vMax);
	//RECT rcImage;
	SetRect(rcImage,CEIL(vMin.x),CEIL(vMin.y),CEIL(vMax.x),CEIL(vMax.y));
	rcImage->left = CLAMP(rcImage->left,0,vPort.Width);
	rcImage->top = CLAMP(rcImage->top,0,vPort.Height);
	rcImage->right = CLAMP(rcImage->right,0,vPort.Width);
	rcImage->bottom = CLAMP(rcImage->bottom,0,vPort.Height);
}

HRESULT CSonyPinpRender::ClearCGBlend(CVideoBuffer* pDest, CVideoBuffer* pSrc)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	D3DXMATRIXA16 matCombine,matTex,matWorld;
	CVideoBuffer * pSrcDef = pSrc;
	CVideoBuffer * pDestDef = pDest;
	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo();
	float pSrcDef_OffsetX = 0, pSrcDef_OffsetY = 0;
	const VideoBufferInfo& biDest = pDestDef->GetVideoBufferInfo();
	// Begin the scene
	if( SUCCEEDED( pDevice->BeginScene() ) )
	{
		UINT pSrcDef_bufferFormat = FMT_RGBA32;
		//Draw BackGround
		UINT cPass,uPass = 0;
		switch(pSrcDef_bufferFormat)
		{
		//case FMT_YUYV:
		//	uPass = 0;
		//	m_pDirectOutEffect->SetTexture("g_txAlpha",((CBaseTexture*)pSrcDef->pAlpha)->GetTexture());
		//	break;
		case FMT_YUVA32:uPass = 1;break;
		case FMT_RGBA32:uPass = 2;break;
		}
		m_pEngine->SetRenderTarget(pDestDef);
		::GenerateMatrix(pSrcDef, &matTex, mat_Image);

		RECT rcImage = {0, 0, biSrc.nWidth, biSrc.nHeight};
		D3DXVECTOR2 offset(pSrcDef_OffsetX,pSrcDef_OffsetY);
		D3DXVECTOR2 destBuffSize(biDest.nAllocWidth, biDest.nAllocHeight);
		GenerateWorld(&rcImage, &offset, &destBuffSize, &matWorld);

		matCombine = matWorld * m_matView * m_matProj;
		m_pDirectOutEffect->SetTechnique("Clear_CGBlend");
		m_pDirectOutEffect->SetMatrix("g_matWorldViewProj",&matCombine);
		m_pDirectOutEffect->SetMatrix("g_matTexture",&matTex);
		m_pDirectOutEffect->SetTexture("g_txColor",pSrcDef->GetTexture());

		m_pDirectOutEffect->Begin(&cPass,0);
		m_pDirectOutEffect->BeginPass(uPass);
		m_pQuadMesh->DrawMeshFx();
		m_pDirectOutEffect->EndPass();
		m_pDirectOutEffect->End();
		// End the scene
		pDevice->EndScene();
	}
	return S_OK;
}

void CSonyPinpRender::GenerateWorld(const RECT * pSrcRcImage,const D3DXVECTOR2 * pOffset,const D3DXVECTOR2 * pDstSize,D3DXMATRIXA16 * pMatWorld)
{
	D3DXMatrixIdentity(pMatWorld);

	pMatWorld->_11 = (pSrcRcImage->right - pSrcRcImage->left)  / (float)pDstSize->x;
	pMatWorld->_22 = (pSrcRcImage->bottom - pSrcRcImage->top) / (float)pDstSize->y;
	pMatWorld->_41 = -0.5f + pMatWorld->_11 / 2.0f + pOffset->x / (float)pDstSize->x;
	pMatWorld->_42 = 0.5f  - pMatWorld->_22 / 2.0f - pOffset->y / (float)pDstSize->y;      
}

