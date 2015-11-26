#include "stdafx.h"

#include "SonyDME3DTransformRender.h"
#include "../Utility/SafeDelete.h"

#define  Pi2  (2.f*D3DX_PI)

CSonyDME3DTransformRender::CSonyDME3DTransformRender(void)
{

}

CSonyDME3DTransformRender::~CSonyDME3DTransformRender(void)
{

}

bool CSonyDME3DTransformRender::Init(CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	m_pSonyDME3DEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/SonyDME3DTransformfx.fx"));
	return true;
}

void CSonyDME3DTransformRender::Uninit()
{

}

bool CSonyDME3DTransformRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc, SonyDME3DTransfromFxPrarm* pParam)
{
	//TODO: only support RGBA
	bool bYUYV = false;

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
	//vPort.Width = pProfile->nEditWidth;
	//vPort.Height = pProfile->nEditHeight;
	//m_pDevice->SetViewport(&vPort);


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