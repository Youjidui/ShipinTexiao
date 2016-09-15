#include "stdafx.h"

#include "SonyDME3DTransformRender.h"
#include "../Utility/SafeDelete.h"

#define  Pi2  (2.f*D3DX_PI)

CSonyDME3DTransformRender::CSonyDME3DTransformRender(void)
{
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tanf(D3DX_PI/8) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );	
	D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 1.0f, 0.1f, 1000.0f );
}

CSonyDME3DTransformRender::~CSonyDME3DTransformRender(void)
{
	Uninit();
}

bool CSonyDME3DTransformRender::Init(CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pSonyDME3DEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/SonyDME3DTransform.fx"));
	ASSERT(m_pSonyDME3DEffect);
	return true;
}

void CSonyDME3DTransformRender::Uninit()
{

}

bool CSonyDME3DTransformRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, FxParamBase* pParamRaw)
{
	SonyDME3DTransfromFxPrarm* pParam = (SonyDME3DTransfromFxPrarm*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	//TODO: only support RGBA
	bool bYUYV = false;
	m_pParam = pParam;
	if(bYUYV)
	{
		// TODO
	}
	else
	{
		RenderRGBA(pDst, pSrc);
	}
	
	return true;
}

bool CSonyDME3DTransformRender::Render( CVideoBuffer* pDst, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB, FxParamBase* pParamRaw )
{
	SonyDME3DTransfromFxPrarm* pParam = (SonyDME3DTransfromFxPrarm*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	CVideoBuffer* pTempDef = m_pEngine->CreateRenderTargetBuffer();//CreateVideoBuffer(pDst->GetVideoBufferInfo());
	m_pParam = pParam;
	RenderRGBA(pTempDef, pSrcA);

	bool bOK = m_pEngine->BlendCompose(pDst, pSrcB, pTempDef);
	ASSERT(bOK);
	bool bOK2 = m_pEngine->GetVideoBufferManager()->ReleaseVideoBuffer(pTempDef);
	ASSERT(bOK2);
	return bOK;
}

void CSonyDME3DTransformRender::RenderRGBA(CVideoBuffer* pDst, CVideoBuffer* pSrc)
{
	//LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	//CResourceManager* pResMan = m_pEngine->GetResourceManager();
	if(m_pEngine->SetRenderTarget(pDst))
	{
		RenderScene(pDst, pSrc);
	}
}

void CSonyDME3DTransformRender::RenderScene(CVideoBuffer* pDst, CVideoBuffer* pSrc) 
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	HRESULT hr = E_FAIL;

	//m_fPixelAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());    
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float fPixelAspect = nEditWidth * 1.0f / nEditHeight;	

	const VideoBufferInfo& biSrc = pSrc->GetVideoBufferInfo();

	D3DXMATRIX matSrcImage;	
	D3DXMatrixIdentity( &matSrcImage );
	float fu = 0.5f/(float)(biSrc.nAllocWidth);
	float fv = 0.5f/(float)(biSrc.nAllocHeight);
	//if(!m_pParam->bOdd)
	//	fv -= 0.5f / (float)(biSrc.nAllocHeight);

	CRect rcImage(0, 0, biSrc.nWidth, biSrc.nHeight);
	//int iWidth = rcImage.right- rcImage.left,iHeight = rcImage.bottom-rcImage.top;
	matSrcImage._11   =  biSrc.nWidth /((float)biSrc.nAllocWidth);
	matSrcImage._22   =  biSrc.nHeight/((float)biSrc.nAllocHeight); 
	matSrcImage._31 =  fu + rcImage.left/((float)biSrc.nAllocWidth);
	matSrcImage._32 =  fv + rcImage.top/((float)biSrc.nAllocHeight); 

	//CSetEffectParam _E(m_pSonyDME3DEffect);

	D3DXMATRIXA16 matView,matProj;
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tan(D3DXToRadian(m_pParam->fPerspective) / 2.0f) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	D3DXMatrixPerspectiveFovLH( &matProj,D3DXToRadian(m_pParam->fPerspective), fPixelAspect, 0.001f, 1000.0f );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	GetWorldMatrix(&matWorld);


	D3DVIEWPORT9 vPort;
	hr = pDevice->GetViewport(&vPort);
	ASSERT(SUCCEEDED(hr));
	//vPort.Width = nEditWidth;
	//vPort.Height = nEditHeight;
	//hr = pDevice->SetViewport(&vPort);
	//ASSERT(SUCCEEDED(hr));

	D3DXMATRIX matScale, matTransition;
	//float fxZoom = biSrc.nWidth * 1.0f  / (float)nEditWidth;
	//float fyZoom = biSrc.nHeight / (float)nEditHeight;
	float fxZoom = nEditWidth / (float)vPort.Width;
	float fyZoom = nEditHeight / (float)vPort.Height;
	D3DXVECTOR2 srcOffset(0, 0);
	//if ( pSrcDef->IsYUV16Buffer() )
	//	srcOffset.x /= 2.0f;
	float ofx = -0.5f + fxZoom*0.5f + srcOffset.x / (float)nEditWidth;
	float ofy =  0.5f - fyZoom*0.5f - srcOffset.y / (float)nEditHeight;
	D3DXMatrixScaling(&matScale, fxZoom, fyZoom , 1.0f);
	D3DXMatrixTranslation(&matTransition, ofx*fPixelAspect, ofy, 0.0f);

	D3DXMATRIX matAspect,matShift;
	D3DXVECTOR4 vMisc;
	//vMisc.x = m_pParam->bOdd?0.0f: 1.0f / vPort.Height;
	vMisc.x = 1.0f / vPort.Height;
	hr = m_pSonyDME3DEffect->SetVector("g_vMisc", &vMisc);
	ASSERT(SUCCEEDED(hr));

	D3DXMatrixIdentity(&matAspect);
	matAspect._11 = fPixelAspect;	

	D3DXMATRIX matWVP =  matAspect  * matScale * matWorld * matTransition * matView * matProj;
	hr = m_pSonyDME3DEffect->SetMatrix("g_matTexture", &matSrcImage);
	ASSERT(SUCCEEDED(hr));
	hr = m_pSonyDME3DEffect->SetMatrix("g_matWorldViewProj", &matWVP);
	ASSERT(SUCCEEDED(hr));
	hr = m_pSonyDME3DEffect->SetTexture("g_txColor", pSrc->GetTexture());
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR3 vNormal(1,0,0),vNewNormal;
	D3DXVec3TransformCoord(&vNewNormal,&vNormal,&matWorld);
	D3DXVec3Normalize(&vNewNormal,&vNewNormal);
	BOOL bAntiAlias = D3DXVec3Dot(&vNormal, &vNewNormal) < cosf(1e-3f);

	CVideoBuffer* pTemp = NULL;
	if(bAntiAlias && (m_pParam->bFilter))
	{
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		//VideoBufferInfo bi = biSrc;
		VideoBufferInfo bi = pDst->GetVideoBufferInfo();
		bi.eUsage = VideoBufferInfo::_IN_OUT;	//must set this
		pTemp = pBufMgr->CreateVideoBuffer(bi);
		ASSERT(pTemp);
		bool bOK = m_pEngine->SetRenderTarget(pTemp);
		ASSERT(bOK);

		D3DXVECTOR4 vSoft(3.0f / nEditWidth,3.0f / nEditHeight,0,0);
		hr = m_pSonyDME3DEffect->SetVector("g_vSoft", &vSoft);
		ASSERT(SUCCEEDED(hr));
		D3DXMATRIXA16 matCombine = m_matView * m_matProj;
		hr = m_pSonyDME3DEffect->SetMatrix("g_matWorldViewProj",&matCombine);
		ASSERT(SUCCEEDED(hr));

		hr = m_pSonyDME3DEffect->SetTechnique("DME");
		ASSERT(SUCCEEDED(hr));
		if ( SUCCEEDED(pDevice->BeginScene()))                                   // begin tech 
		{
			UINT cPass;
			hr = m_pSonyDME3DEffect->Begin(&cPass,0);
			ASSERT(SUCCEEDED(hr));
			hr = m_pSonyDME3DEffect->BeginPass(1);
			ASSERT(SUCCEEDED(hr));
			bool bOK = m_pQuadMesh-> DrawMeshFx();
			ASSERT(bOK);
			hr = m_pSonyDME3DEffect->EndPass();
			ASSERT(SUCCEEDED(hr));
			hr = m_pSonyDME3DEffect->End();
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->EndScene();
			ASSERT(SUCCEEDED(hr));
		}

		bOK = m_pEngine->SetRenderTarget(pDst);
		ASSERT(bOK);
		hr = m_pSonyDME3DEffect->SetMatrix("g_matWorldViewProj",&matWVP);
		ASSERT(SUCCEEDED(hr));

		//TP_VBufferDef* pTempDef = m_pResMan->GetBufferDef(hTemp);
		//CVideoBuffer* pTempDef = pBufMgr->CreateVideoBuffer(pDst->GetVideoBufferInfo());
		hr = m_pSonyDME3DEffect->SetTexture("g_txColor", pTemp->GetTexture());
		ASSERT(SUCCEEDED(hr));

		// wxb,wsp add
		GenerateMatrix(pTemp, &matSrcImage, mat_Image);

		//add by szm. 旋转变模糊
		fv = 0.f;
		//if(!m_pParam->bOdd) // jason 没考虑奇场
		//	fv -= 0.5f / (float)(pTempDef->GetVideoBufferInfo().nHeight);
		matSrcImage._32 += fv;
		//end by szm. 旋转变模糊
	
		hr = m_pSonyDME3DEffect->SetMatrix("g_matTexture", &matSrcImage);
		ASSERT(SUCCEEDED(hr));
		
		//D3DXSaveSurfaceToFile(_T("./filter.dds"), D3DXIFF_DDS, pTemp->GetSurface(), NULL, NULL);
	}


	hr = m_pSonyDME3DEffect->SetTechnique("DME");
	ASSERT(SUCCEEDED(hr));
	//BeginAntiAlias();
	if ( SUCCEEDED(pDevice->BeginScene()))                                   // begin tech 
	{
		UINT cPass;
		hr = m_pSonyDME3DEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		hr = m_pSonyDME3DEffect->BeginPass(0);
		ASSERT(SUCCEEDED(hr));
		bool bOK = m_pQuadMesh-> DrawMeshFx();
		ASSERT(bOK);
		hr = m_pSonyDME3DEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
		hr = m_pSonyDME3DEffect->End();
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->EndScene();
		ASSERT(SUCCEEDED(hr));
	}
	//EndAntiAlias(m_pDstDef);

	//D3DXSaveSurfaceToFile(_T("./dme.dds"), D3DXIFF_DDS, pDst->GetSurface(), NULL, NULL);

	if(pTemp)
	{
		CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
		pVM->ReleaseVideoBuffer(pTemp);
		pTemp = NULL;
	}
}

void CSonyDME3DTransformRender::GetWorldMatrix(D3DXMATRIX * matWorld)
{
	//post
    D3DXMATRIX matPost;

	//location
	float  fPostLocationX = m_pParam->fPostLocationX;
	float  fPostLocationY = m_pParam->fPostLocationY;
	D3DXMATRIX matPostLoc;
	D3DXMatrixTranslation(&matPostLoc,fPostLocationX,fPostLocationY,0.f);
	//Size
	float fPostSizeX = m_pParam->fPostSizeX;
	float fPostSizeY = m_pParam->fPostSizeY;
	D3DXMATRIX matPostSize;
	D3DXMatrixScaling(&matPostSize,fPostSizeX,fPostSizeY,1.0f);
    
	matPost = matPostLoc * matPostSize;
    
	//Perspective
 //   float fPerspective  = m_pParam->fPerspective / 180.f *D3DX_PI;
 //	D3DXMATRIX matPers;
	//D3DXMatrixIdentity(&matPers);
	//matPers._33  = 0.f;
	//matPers._34 = tanf(fPerspective);
    
	
	// Global
    D3DXMATRIX matGlobal;
	//location
	float fLocationX =  m_pParam->sGlobal.fLocationX ;
// 	float fLocationY = m_pParam->sGlobal.fLocationY / m_fPixelAspect;
	float fLocationY = m_pParam->sGlobal.fLocationY;
	float fLocationZ = m_pParam->sGlobal.fLocationZ;

	fLocationX *= 0.5f;
	fLocationY *= 0.5f;
	// 	m_pParam->sGlobal.fLocationX /= m_fPixelAspect*2.f;
	fLocationZ *= 0.5f;

	D3DXMATRIX matLocation_Global;
	D3DXMatrixTranslation(&matLocation_Global,fLocationX,fLocationY/* / m_fPixelAspect*/,fLocationZ);
	//spin
	float fSpinX  =  m_pParam->sGlobal.fSpinX*Pi2; 
	float fSpinY  =  m_pParam->sGlobal.fSpinY*Pi2;
	float fSpinZ  =  m_pParam->sGlobal.fSpinZ*Pi2;
	D3DXMATRIX matSpin_Global;
	D3DXMatrixRotationYawPitchRoll(&matSpin_Global,fSpinY,fSpinX,fSpinZ);

	//Rotation;
	float fRotationX = m_pParam->sGlobal.fRotationX*Pi2;
	float fRotationY = m_pParam->sGlobal.fRotationY*Pi2;
	float fRotationZ = m_pParam->sGlobal.fRotationZ*Pi2;
	D3DXMATRIX matRotation_Global;
	D3DXMatrixRotationYawPitchRoll(&matRotation_Global,fRotationY,fRotationX,fRotationZ);

    // source Spin
	
	float fSrcSpinX = m_pParam->sGlobal.fSrcSpinX*Pi2;
	float fSrcSpinY = m_pParam->sGlobal.fSrcSpinY*Pi2;
	float fSrcSpinZ = m_pParam->sGlobal.fSrcSpinZ*Pi2;
    D3DXMATRIX matSrcSpin_Global;
	D3DXMatrixRotationYawPitchRoll(&matSrcSpin_Global,fSrcSpinY,fSrcSpinX,fSrcSpinZ);
	
	
	//Axis
  
	float fAxisX  =  m_pParam->sGlobal.fAxisX + fLocationX;
	float fAxisY  =  m_pParam->sGlobal.fAxisY + fLocationY;
	float fAxisZ  =  m_pParam->sGlobal.fAxisZ + fLocationZ; 
	D3DXMATRIX matAxisP_Global,matAxisN_Global;
	D3DXMatrixTranslation(&matAxisP_Global,fAxisX,fAxisY,fAxisZ);
	D3DXMatrixTranslation(&matAxisN_Global,-fAxisX,-fAxisY,-fAxisZ);


    matGlobal = matLocation_Global * matAxisN_Global * matSrcSpin_Global * matRotation_Global * matSpin_Global *matAxisP_Global;


	// Local
    D3DXMATRIX matLocal;
	//location
	fLocationX =  m_pParam->sLocal.fLocationX;
// 	 fLocationY = m_pParam->sLocal.fLocationY / m_fPixelAspect;
	fLocationY = m_pParam->sLocal.fLocationY;
	fLocationZ = m_pParam->sLocal.fLocationZ;

	fLocationX *= 0.5f;
	fLocationY *= 0.5f;
	// 	m_pParam->sLocal.fLocationX /= m_fPixelAspect*2.f;
	fLocationZ *= 0.5f;

	D3DXMATRIX matLocation_Local;
	D3DXMatrixTranslation(&matLocation_Local,fLocationX,fLocationY/* / m_fPixelAspect*/,fLocationZ);
	//spin
	 fSpinX  =  m_pParam->sLocal.fSpinX*Pi2; 
	 fSpinY  =  m_pParam->sLocal.fSpinY*Pi2;
	 fSpinZ  =  m_pParam->sLocal.fSpinZ*Pi2;
	D3DXMATRIX matSpin_Local;
	D3DXMatrixRotationYawPitchRoll(&matSpin_Local,fSpinY,fSpinX,fSpinZ);

	//Rotation;
	 fRotationX = m_pParam->sLocal.fRotationX*Pi2;
	 fRotationY = m_pParam->sLocal.fRotationY*Pi2;
	 fRotationZ = m_pParam->sLocal.fRotationZ*Pi2;
	D3DXMATRIX matRotation_Local,matRotX,matRotY,matRotZ;
    D3DXMatrixRotationX(&matRotX,fRotationX);
	D3DXMatrixRotationY(&matRotY,fRotationY);
	D3DXMatrixRotationZ(&matRotZ,fRotationZ);
	matRotation_Local = matRotZ * matRotX * matRotY;
//	D3DXMatrixRotationYawPitchRoll(&matRotation_Local,fRotationY,fRotationX,fRotationZ);
	
	
	
	//Source Spin
	 fSrcSpinX = m_pParam->sLocal.fSrcSpinX*Pi2;
	 fSrcSpinY = m_pParam->sLocal.fSrcSpinY*Pi2;
	 fSrcSpinZ = m_pParam->sLocal.fSrcSpinZ*Pi2;
	D3DXMATRIX matSrcSpin_Local;
	D3DXMatrixRotationYawPitchRoll(&matSrcSpin_Local,fSrcSpinY,fSrcSpinX,fSrcSpinZ);
	
	//Axis
	fAxisX  =  m_pParam->sLocal.fAxisX + fLocationX;
	fAxisY  =  m_pParam->sLocal.fAxisY + fLocationY;
	fAxisZ  =  m_pParam->sLocal.fAxisZ + fLocationZ; 
/*	 fAxisX  =  m_pParam->sLocal.fAxisX ;
	 fAxisY  =  m_pParam->sLocal.fAxisY ;
	 fAxisZ  =  m_pParam->sLocal.fAxisZ ;*/ 
	D3DXMATRIX matAxisP_Local,matAxisN_Local;
	D3DXMatrixTranslation(&matAxisP_Local,fAxisX,fAxisY,fAxisZ);
	D3DXMatrixTranslation(&matAxisN_Local,-fAxisX,-fAxisY,-fAxisZ);

	matLocal = matLocation_Local * matAxisN_Local* matSrcSpin_Local * matRotation_Local * matSpin_Local * matAxisP_Local;
   
    
	//pre
	D3DXMATRIX matPre;
	//skew ;

	float fSkewX = m_pParam->fSkewX;
	float fSkewY = m_pParam->fSkewY;
	D3DXMATRIX matSkew;
    D3DXMatrixIdentity(&matSkew);
    matSkew._12 = fSkewY;
	matSkew._21 = fSkewX;

	//pre size
	float fPreSizeX = m_pParam->fPreSizeX;
	float fPreSizeY = m_pParam->fPreSizeY;
	D3DXMATRIX matPreSize;
	D3DXMatrixScaling(&matPreSize,fPreSizeX,fPreSizeY,1.f);
    
	matPre = matSkew * matPreSize;

 	*matWorld =   matPre * matLocal *matGlobal  *matPost/** matPers*/ ;

	m_bRotate = D3DXMatrixIsIdentity(&matSpin_Global) && D3DXMatrixIsIdentity(&matRotation_Global) && D3DXMatrixIsIdentity(&matSrcSpin_Global) 
		&& D3DXMatrixIsIdentity(&matSpin_Local) && D3DXMatrixIsIdentity(&matSrcSpin_Local) && D3DXMatrixIsIdentity(&matSkew);
}
