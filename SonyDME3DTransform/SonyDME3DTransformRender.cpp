#include "stdafx.h"

#include "SonyDME3DTransformRender.h"
#include "../Utility/SafeDelete.h"

#define  Pi2  (2.f*D3DX_PI)

CSonyDME3DTransformRender::CSonyDME3DTransformRender(void)
{
	D3DXMatrixIdentity(&m_matWorld);
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
	m_pSonyDME3DEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/SonyDME3DTransformfx.fx"));
	ASSERT(m_pSonyDME3DEffect);
	return true;
}

void CSonyDME3DTransformRender::Uninit()
{

}

bool CSonyDME3DTransformRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, SonyDME3DTransfromFxPrarm* pParam)
{
	HRESULT hr = E_FAIL;

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

void CSonyDME3DTransformRender::RenderRGBA(CVideoBuffer* pDst, CVideoBuffer* pSrc)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
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

	//TODO
	//m_fPixelAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());    
	float fPixelAspect = 0;
	

	D3DXMATRIX matSrcImage;	
	D3DXMatrixIdentity( &matSrcImage );

	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	const float pSrcDef0_OffsetX = 0.f, pSrcDef0_OffsetY = 0.f;
	float fu = 0.5f/(float)(nEditWidth);
	float fv = 0.5f/(float)(nEditHeight); 

	RECT rcImage;
	rcImage.top = 0;
	rcImage.left = 0;
	rcImage.right = nEditWidth;
	rcImage.bottom = nEditHeight;

	int iWidth = rcImage.right- rcImage.left,iHeight = rcImage.bottom-rcImage.top;
	matSrcImage._11   =  iWidth /((float)nEditWidth);
	matSrcImage._22   =  iHeight/((float)nEditHeight); 
	matSrcImage._31 =  fu + rcImage.left/((float)nEditWidth);
	matSrcImage._32 =  fv + rcImage.top/((float)nEditHeight); 

	//CSetEffectParam _E(m_pSonyDME3DEffect);

	D3DXMATRIXA16 matView,matProj;

	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tan(D3DXToRadian(m_pParam->fPerspective) / 2.0f) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
		
	//D3DXMatrixPerspectiveFovLH( &matProj,D3DXToRadian(m_pParam->fPerspective), m_fPixelAspect, 0.001f, 1000.0f );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	GetWorldMatrix(&matWorld);


	D3DVIEWPORT9 vPort;
	pDevice->GetViewport(&vPort);
	vPort.Width = nEditWidth;
	vPort.Height = nEditHeight;
	pDevice->SetViewport(&vPort);

	D3DXMATRIX matScale, matTransition;

	float fxZoom = (float)pSrc->GetVideoBufferInfo().nWidth * 1.0f  / (float)nEditWidth;
	float fyZoom = (float)pSrc->GetVideoBufferInfo().nHeight / (float)nEditHeight;

	// TODO offset
	D3DXVECTOR2 srcOffset(0, 0);
	//if ( pSrcDef->IsYUV16Buffer() )
	//	srcOffset.x /= 2.0f;
	float ofx = -0.5f + fxZoom*0.5f + srcOffset.x / (float)nEditWidth;
	float ofy =  0.5f - fyZoom*0.5f - srcOffset.y / (float)nEditHeight;

	D3DXMatrixScaling(&matScale, fxZoom, fyZoom , 1.0f);
	D3DXMatrixTranslation(&matTransition, ofx*fPixelAspect, ofy, 0.0f);

	D3DXMATRIX matAspect,matShift;
	// 	if(D3DXMatrixIsIdentity(&matWorld))
	//   	     D3DXMatrixIdentity(&matShift);
	//	else 
	///D3DXMatrixTranslation(&matShift,0.0f,m_pParam->bOdd?0.0f: 0.5f / m_fPixelAspect /  vPort.Height,0.0f);

	D3DXVECTOR4 vMisc;
	vMisc.x = 1.0f / vPort.Height;//m_pParam->bOdd?0.0f: 1.0f
	m_pSonyDME3DEffect->SetVector("g_vMisc", &vMisc);

	D3DXMatrixIdentity(&matAspect) ;
	//     matAspect._22 = 1.f/m_fPixelAspect;	
	matAspect._11 = fPixelAspect;	

	D3DXMATRIX matWVP =  matAspect  * matScale * matWorld * matTransition * matView * matProj;

	m_pSonyDME3DEffect->SetMatrix("g_matTexture", &matSrcImage);
	m_pSonyDME3DEffect->SetMatrix("g_matWorldViewProj", &matWVP);
	m_pSonyDME3DEffect->SetTexture("g_txColor", pDst->GetTexture());


	/* allways false
	handle_tpr		hFilterBuffer	= INVALID_RESID;
	TP_VBufferDef*	pFilterDef		= NULL;
	//yanwei begin

	if(m_pParam->bFilter && hFilterBuffer != INVALID_RESID)
	{
		hFilterBuffer = NewRTBuffer(pSrcDef->OffsetX, pSrcDef->OffsetY, pSrcDef->GetImageWidth(), pSrcDef->GetImageHeight());
		pFilterDef = m_pResMan->GetBufferDef(hFilterBuffer);

		CBaseTexture* pBaseTexFilter = (CBaseTexture*)m_pResMan->GetResource(m_uResID_8TapTable);

		D3DXMATRIX *_matWorld, *_matView, *_matProj;
		m_pEngine->GetQuadMatrix(&_matWorld, &_matView, &_matProj);
		float fScaleX = (sqrt(matWorld._11*matWorld._11 + matWorld._21*matWorld._21) * matView._43 / (matView._43 + matWorld._43));
		float fScaleY = (sqrt(matWorld._12*matWorld._12 + matWorld._22*matWorld._22) * matView._43 / (matView._43 + matWorld._43));
		
		LPDIRECT3DTEXTURE9 pFilterTex = ((CBaseTexture*)pFilterDef->pContainer)->GetTexture();
		LPDIRECT3DTEXTURE9 p8TapTableTex = pBaseTexFilter->GetTexture();

		const Custom_Profile* pProfile = m_pEngine->GetCurProfile();
		bool bInterlaced = (pProfile->m_iRate == 1);
		FillKaiserCoeff_8tap(p8TapTableTex, fScaleX, fScaleY, bInterlaced);

		//这里需要先将整张buffer清空为YUVA的BLACK，否则这里会变成只有视口部分为YUVA_BLACK，其余部分为RGBA_BLACK
		//在当前采样模式下有半像素偏移，因此逐字下会显示出边缘，日后特技SDK迁移的话可以不考虑这一点
		pFilterDef->bRenderToFullTarget = true;
		m_pEngine->SetRenderTarget(0, hFilterBuffer, pFilterDef->COLOR_BLACK());
		pFilterDef->bRenderToFullTarget = false;
		m_pEngine->SetRenderTarget(0, hFilterBuffer, pFilterDef->COLOR_BLACK());
		GenerateMatrix(m_pResMan, pSrcDef->handle, &matSrcImage, mat_Image);
		D3DXVECTOR2 vBaseCoef;
		vBaseCoef.x = 1.f / pSrcDef->BaseWidth;
		vBaseCoef.y = 1.f / pSrcDef->BaseHeight;
		D3DXMATRIX matDummy;
		D3DXMATRIX matPixelOffset;
		D3DXMatrixIdentity(&matDummy);
		D3DXMatrixTranslation(&matPixelOffset, -vBaseCoef.x, vBaseCoef.y, 0.f);
		matDummy = (*_matWorld)*(*_matView)*(*_matProj);//*matPixelOffset;
		_E("fBaseCoef", &vBaseCoef);
		_E("tableCoef8", &p8TapTableTex);
		_E("g_matWorldViewProj", &matDummy);
		_E("g_matTexture", &matSrcImage);
		_E("Imai8Tap");
		if ( SUCCEEDED(m_pDevice->BeginScene()))                                   // begin tech 
		{
			UINT cPass;
			m_pEffect->Begin(&cPass,0);
				m_pEffect->BeginPass(0);
				m_pQuadMesh->DrawMeshFx();
				m_pEffect->EndPass();
			m_pEffect->End();
			m_pDevice->EndScene();
		}
		m_pEngine->SetRenderTarget(0,pDstDef->handle,pDstDef->COLOR_BLACK());		
		_E("g_txColor", &pFilterTex);
		_E("g_matWorldViewProj",&matWVP);
		GenerateMatrix(m_pResMan,hFilterBuffer,&matSrcImage,mat_Image);
		_E("g_matTexture", &matSrcImage);
	}
	//yanwei end
	*/

	D3DXVECTOR3 vNormal(1,0,0),vNewNormal;
	D3DXVec3TransformCoord(&vNewNormal,&vNormal,&matWorld);
	D3DXVec3Normalize(&vNewNormal,&vNewNormal);

	BOOL bAntiAlias = D3DXVec3Dot(&vNormal,&vNewNormal) < cosf(1e-3);
	//handle_tpr hTemp = INVALID_RESID;
	if(bAntiAlias && (m_pParam->bFilter))
	{
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		CVideoBuffer* pTemp = pBufMgr->CreateVideoBuffer(pDst->GetVideoBufferInfo());
		m_pEngine->SetRenderTarget(pTemp);

		D3DXVECTOR4 vSoft(3.0f / nEditWidth,3.0f / nEditHeight,0,0);
		m_pSonyDME3DEffect->SetVector("g_vSoft", &vSoft);
		D3DXMATRIXA16 matCombine = m_matView * m_matProj;
		m_pSonyDME3DEffect->SetMatrix("g_matWorldViewProj",&matCombine);


		//add by szm. 旋转变模糊
		//D3DXMATRIX matTex = matSrcImage;
		//if(pFilterDef)
		//	GenerateMatrix(m_pResMan, pFilterDef->handle, &matTex, mat_Image);
		//m_pSonyDME3DEffect->SetTexture("g_matTexture", &matTex);
		//end by szm. 旋转变模糊

		m_pSonyDME3DEffect->SetTechnique("DME");
		if ( SUCCEEDED(pDevice->BeginScene()))                                   // begin tech 
		{
			UINT cPass;
			m_pSonyDME3DEffect->Begin(&cPass,0);

			m_pSonyDME3DEffect->BeginPass(1);
			m_pQuadMesh-> DrawMeshFx();
			m_pSonyDME3DEffect->EndPass();

			m_pSonyDME3DEffect->End();
			pDevice->EndScene();
		}
		m_pEngine->SetRenderTarget(pDst);		
		m_pSonyDME3DEffect->SetMatrix("g_matWorldViewProj",&matWVP);

		//TP_VBufferDef* pTempDef = m_pResMan->GetBufferDef(hTemp);
		CVideoBuffer* pTempDef = pBufMgr->CreateVideoBuffer(pDst->GetVideoBufferInfo());
		
		m_pSonyDME3DEffect->SetTexture("g_txColor", pTempDef->GetTexture());

		// wxb,wsp add
		GenerateMatrix(pTempDef,&matSrcImage,mat_Image);

		//add by szm. 旋转变模糊
		fv = 0.f;
		//if(!m_pParam->bOdd) // jason 没考虑奇场
		//	fv -= 0.5f / (float)(pTempDef->GetVideoBufferInfo().nHeight);
		matSrcImage._32 += fv;
		//end by szm. 旋转变模糊
	
		m_pSonyDME3DEffect->SetMatrix("g_matTexture", &matSrcImage);
		//_E("g_matTexture", &matSrcImage);
		// wxb,wsp add end
	}


	m_pSonyDME3DEffect->SetTechnique("DME");
	//BeginAntiAlias();
	if ( SUCCEEDED(pDevice->BeginScene()))                                   // begin tech 
	{
		UINT cPass;
		m_pSonyDME3DEffect->Begin(&cPass,0);

		m_pSonyDME3DEffect->BeginPass(0);
		m_pQuadMesh-> DrawMeshFx();
		m_pSonyDME3DEffect->EndPass();

		m_pSonyDME3DEffect->End();

		pDevice->EndScene();
	}
	//EndAntiAlias(m_pDstDef);
	/*
	if(hFilterBuffer != INVALID_RESID)
	{
		m_pResMan->FreeRTBuffer(hFilterBuffer);
	}
	if(hTemp != INVALID_RESID)
	{
		m_pResMan->FreeRTBuffer(hTemp);
	}
	*/
	// m_pResMan->DumpResourceToFile(pDstDef->handle, L"c:\\tempBuffer.dds");
}

void CSonyDME3DTransformRender::GetWorldMatrix(D3DXMATRIX * matWorld)
{
	m_pParam->sLocal.fLocationX *= 0.5f;
	m_pParam->sLocal.fLocationY *= 0.5f;
// 	m_pParam->sLocal.fLocationX /= m_fPixelAspect*2.f;
	m_pParam->sLocal.fLocationZ *= 0.5f;
	m_pParam->sGlobal.fLocationX *= 0.5f;
	m_pParam->sGlobal.fLocationY *= 0.5f;
// 	m_pParam->sGlobal.fLocationX /= m_fPixelAspect*2.f;
    m_pParam->sGlobal.fLocationZ *= 0.5f;
    
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
