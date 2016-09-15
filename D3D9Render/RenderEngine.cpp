#include "StdAfx.h"
#include <DxErr.h>
#include "RenderEngine.h"
#include "VideoBuffer.h"
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"
#include "../Logger/Logging.h"

#pragma warning(disable:4996)

CRenderEngine::CRenderEngine(void)
: m_pD3D(NULL)
, m_pDevice(NULL)
, m_pDepthNoMultiSample(NULL)
, m_pResMgr(NULL)
, m_pBuffMgr(NULL)
{
	ZeroMemory(&m_DeviceSettings, sizeof(m_DeviceSettings));
	//ZeroMemory(&m_DeviceSettings.pp, sizeof(m_DeviceSettings.pp));
	m_TargetVideoSize.cx = 0;
	m_TargetVideoSize.cy = 0;
}

CRenderEngine::~CRenderEngine(void)
{
	SAFE_RELEASE(m_pDepthNoMultiSample);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pD3D);
}

HRESULT CRenderEngine::Create( HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight )
{
	HRESULT hr = E_FAIL;
	//Initialize Direct3D 9.0
	m_pD3D        = Direct3DCreate9( D3D_SDK_VERSION );

	m_DeviceSettings.pp.Windowed               = true;
	m_DeviceSettings.pp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	//m_DeviceSettings.pp.BackBufferWidth        = nBackBufferWidth ;
	//m_DeviceSettings.pp.BackBufferHeight       = nBackBufferHeight;  
	RECT rcClient;
	GetClientRect(hDeviceWnd,&rcClient);
	m_DeviceSettings.pp.BackBufferWidth = UINT(RECTWIDTH(rcClient) * 0.99f);
	m_DeviceSettings.pp.BackBufferHeight = UINT(RECTHEIGHT(rcClient) * 0.8f);

	m_DeviceSettings.pp.BackBufferCount        = 1;	
	m_DeviceSettings.pp.BackBufferFormat       = D3DFMT_A8R8G8B8;
	m_DeviceSettings.pp.hDeviceWindow          = hDeviceWnd?hDeviceWnd:GetDesktopWindow();    
	m_DeviceSettings.pp.MultiSampleType        = D3DMULTISAMPLE_NONE;    
	m_DeviceSettings.pp.MultiSampleQuality     = 0;
	m_DeviceSettings.pp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE/*|D3DPRESENT_DONOTWAIT*/;

	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(0,&mode);

	m_DeviceSettings.AdapterOrdinal =  D3DADAPTER_DEFAULT;
	m_DeviceSettings.DeviceType = D3DDEVTYPE_HAL          ;
	m_DeviceSettings.AdapterFormat = mode.Format;
	m_DeviceSettings.BehaviorFlags = D3DCREATE_MULTITHREADED
		|D3DCREATE_HARDWARE_VERTEXPROCESSING
		|D3DCREATE_FPU_PRESERVE
		|D3DCREATE_NOWINDOWCHANGES;

	if( FAILED(hr = m_pD3D->CreateDevice( m_DeviceSettings.AdapterOrdinal, 
		m_DeviceSettings.DeviceType,
		hDeviceWnd, 
		m_DeviceSettings.BehaviorFlags,	
		&m_DeviceSettings.pp,
		&m_pDevice ) ) )                                 
	{
		MessageBox(NULL,L"Initial GPU Failed ,Please Restart Computer !!!",L"Error",MB_OK|MB_ICONERROR);
		return hr;
	}

	ASSERT(m_pDevice);
	SetTargetVideoSize(nBackBufferWidth, nBackBufferHeight);
	return hr;
}

bool CRenderEngine::SetRenderTarget( CVideoBuffer* pDest, bool bSetDepthStencil )
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPDIRECT3DSURFACE9 pRTSurface = NULL;
	if(pDest)
	{
		pRTSurface = pDest->GetSurface();
	}

	// set render target
	ASSERT(NULL != pRTSurface);
	hr = pDevice->SetRenderTarget(0, pRTSurface);
	if(FAILED(hr))
	{
		LPCTSTR pszErrorString = DXGetErrorString(hr);
		LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
		TRACE(pszErrorString);
		TRACE(pszErrorDesc);

		char buf[MAX_PATH];
		wcstombs(buf, pszErrorDesc, MAX_PATH);
		LOG_ERROR_FORMAT("%s:pDevice->SetRenderTarget failed because %s", __FUNCTION__, buf);
	}
	//SAFE_RELEASE(pRTSurface);
	hr = pDevice->SetRenderTarget(1, NULL);
	ASSERT(SUCCEEDED(hr));

	if(pRTSurface)
	{
		const VideoBufferInfo& buffInfo = pDest->GetVideoBufferInfo();
		// set view port
		D3DVIEWPORT9 vp;
		vp.MaxZ = 1.0f;
		vp.MinZ = 0.0f;
		vp.X        = 0;
		vp.Y        = 0;
		vp.Width    = buffInfo.nWidth;
		vp.Height   = buffInfo.nHeight;
		hr = pDevice->SetViewport(&vp);
		ASSERT(SUCCEEDED(hr));

		DWORD flag = D3DCLEAR_TARGET;
		if(bSetDepthStencil)
		{
			SetDepthStencilBuffer(true);
			flag = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL;
		}
		hr = pDevice->Clear(0, NULL, flag, 0, 1.0f, 0);
		if(FAILED(hr))
		{
			LPCTSTR pszErrorString = DXGetErrorString(hr);
			LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
			TRACE(pszErrorString);
			TRACE(pszErrorDesc);

			char buf[MAX_PATH];
			wcstombs(buf, pszErrorDesc, MAX_PATH);
			LOG_ERROR_FORMAT("%s:pDevice->Clear failed because %s", __FUNCTION__, buf);
		}
	}
	return !!pRTSurface;
}


bool CRenderEngine::SetRenderTarget( CVideoBuffer* pDest )
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPDIRECT3DSURFACE9 pRTSurface = NULL;
	if(pDest)
	{
		pRTSurface = pDest->GetSurface();
	}

	// set render target
	ASSERT(NULL != pRTSurface);
	hr = pDevice->SetRenderTarget(0, pRTSurface);
	if(FAILED(hr))
	{
		LPCTSTR pszErrorString = DXGetErrorString(hr);
		LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
		TRACE(pszErrorString);
		TRACE(pszErrorDesc);

		char buf[MAX_PATH];
		wcstombs(buf, pszErrorDesc, MAX_PATH);
		LOG_ERROR_FORMAT("%s:pDevice->SetRenderTarget failed because %s", __FUNCTION__, buf);
	}
	//SAFE_RELEASE(pRTSurface);
	hr = pDevice->SetRenderTarget(1, NULL);
	ASSERT(SUCCEEDED(hr));

	if(pRTSurface)
	{
		const VideoBufferInfo& buffInfo = pDest->GetVideoBufferInfo();
		// set view port
		D3DVIEWPORT9 vp;
		vp.MaxZ = 1.0f;
		vp.MinZ = 0.0f;
		vp.X        = 0;
		vp.Y        = 0;
		vp.Width    = buffInfo.nWidth;
		vp.Height   = buffInfo.nHeight;
		hr = pDevice->SetViewport(&vp);
		ASSERT(SUCCEEDED(hr));

		DWORD flag = D3DCLEAR_TARGET;
		LPDIRECT3DSURFACE9 pDepthStencilBuffer = NULL;
		hr = pDevice->GetDepthStencilSurface(&pDepthStencilBuffer);
		//ASSERT(SUCCEEDED(hr));
		if(pDepthStencilBuffer)
		{
			flag = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL;
			pDepthStencilBuffer->Release();
		}
		hr = pDevice->Clear(0, NULL, flag, 0, 1.0f, 0);
		if(FAILED(hr))
		{
			LPCTSTR pszErrorString = DXGetErrorString(hr);
			LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
			TRACE(pszErrorString);
			TRACE(pszErrorDesc);

			char buf[MAX_PATH];
			wcstombs(buf, pszErrorDesc, MAX_PATH);
			LOG_ERROR_FORMAT("%s:pDevice->Clear failed because %s", __FUNCTION__, buf);
		}
	}
	return !!pRTSurface;
}

void CRenderEngine::GetTargetVideoSize( int& nEditWidth, int& nEditHeight )
{
	nEditWidth = m_TargetVideoSize.cx;
	nEditHeight = m_TargetVideoSize.cy;
}

bool CRenderEngine::EffectVideoCopy( CVideoBuffer* pDst, CVideoBuffer* pSrc )
{
	RESET_RENDER_TARGET(this);
	//SET_DEPTH_STENCIL(this);

	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	CResourceManager* pResMan = GetResourceManager();
	//这个函数只用于特技缺省Copy ，由调用者决定是否使用Zbuffer
	//CZBufferManager zbuffer(this,DEPTHSTENCIL_NONE);

	//TP_VBufferDef *vBufferSrc = m_pResManager->GetBufferDef( hSrc );
	//TP_VBufferDef *vBufferDst = m_pResManager->GetBufferDef( hDst );
	//assert( vBufferSrc->bufferFormat == vBufferDst->bufferFormat );	
	CBaseMesh *pMesh          = GetResourceManager()->CreateQuadMesh(GetDevice());
	ASSERT(pMesh);
	const VideoBufferInfo& srcBI = pSrc->GetVideoBufferInfo();
	const VideoBufferInfo& destBI = pDst->GetVideoBufferInfo();
	//GAutoLock l(&m_D3Dcs);
	//UINT PSShader,VSShader;
	//D3DXMATRIX matWorld,matScale;
	D3DXMATRIXA16 	matTexture;  
	float f_Offsetx;
	//long nWidth,nHeight;
	float vBufferSrcOffsetY = 0.0f;
	f_Offsetx = vBufferSrcOffsetY;
	int iTargetImageWidth  = srcBI.nWidth;
	int iTargetImageHeight = srcBI.nHeight;
	int RTTextureWidth     = destBI.nAllocWidth;
	int RTTextureHeight    = destBI.nAllocHeight;

	D3DXMATRIXA16  matWorld;
	D3DXMATRIX *pMatView = NULL, *pMatProj = NULL;
	D3DXMatrixIdentity(&matWorld);
	GetResourceManager()->GetOrthoMatrix(&pMatView, &pMatProj);
	D3DXMATRIXA16 matWVP = matWorld * (*pMatView)* (*pMatProj);
	D3DXMATRIXA16 matCC = (*pMatView)*(*pMatProj);
	HRESULT hr = pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE );
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetRenderState( D3DRS_ALPHATESTENABLE ,FALSE );
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR4 vMeshArgs(  f_Offsetx/(float)RTTextureWidth,
		vBufferSrcOffsetY/(float)RTTextureHeight,
		iTargetImageWidth/(float)RTTextureWidth,
		iTargetImageHeight/(float)RTTextureHeight);
	hr = pDevice->SetTexture( 0,pSrc->GetTexture());
	ASSERT(SUCCEEDED(hr));
	//if(vBufferSrc->pAlpha )	SetSourceTexture( 1,vBufferSrc->pAlpha );
	bool bOk = GenerateMatrix( pSrc, &matTexture, mat_Image );
	ASSERT(bOk);

	//vBufferDst->bRenderToFullTarget = true;
	//vBufferDst->bClearFullTarget    = true;
	//vBufferDst->bDiscardAlpha       = false;
	//if(vBufferSrc->pAlpha&&!vBufferDst->pAlpha)
	//{
	//	vBufferDst->pAlpha= AllocateVideoAlphaRT();
	//}
	//SetRenderTarget( 0, hDst,D3DCLEAR_TARGET,vBufferDst->COLOR_BLACK(), 0,1.0f,0 );	// --zms-- yuyv无论何种情况，都清除为黑色（而不是绿色）。
	bOk = SetRenderTarget(pDst);
	ASSERT(bOk);

	float bHaveAlpha     = 0.0f;//vBufferSrc->pAlpha?1.0:0.0;
	float bParticleBlend = 0.0f;//vBufferSrc->bParticleBlend?1.0:0.0;

	CVertexShader* pVS = pResMan->CreateVertexShader(pDevice, _T("Shaders/VS_SplitField.VSH"));
	ASSERT(pVS);
	CPixelShader* pPS = pResMan->CreatePixelShader(pDevice, _T("Shaders/PS_DirectOutEffect.PSH"));
	ASSERT(pPS);

	if(SUCCEEDED(pDevice -> BeginScene()))
	{				 
		hr = pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		ASSERT(SUCCEEDED(hr));

		hr = pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetPixelShader(pPS->GetPixelShaderPtr());
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetPixelShaderConstantF(0,&bHaveAlpha,1);
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetPixelShaderConstantF(1,&bParticleBlend,1);
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetVertexShaderConstantF( 0, (float*)&matCC,      4 );
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetVertexShaderConstantF( 4, (float*)&matTexture, 4 );
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->SetVertexShaderConstantF( 8, (float*)&vMeshArgs,  1 );
		ASSERT(SUCCEEDED(hr));

		bOk = pMesh -> DrawMesh(0, pVS->GetVertexShaderPtr());
		ASSERT(bOk);
		hr = pDevice->EndScene();
		ASSERT(SUCCEEDED(hr));
	}

	hr = pDevice->SetVertexShader(NULL);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShader( NULL );
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetTexture( 0, NULL );
	ASSERT(SUCCEEDED(hr));
	
	return true;  
}

CVideoBuffer* CRenderEngine::CreateRenderTargetBuffer()
{
	int nEditWidth, nEditHeight;
	GetTargetVideoSize(nEditWidth, nEditHeight);
	CVideoBufferManager* pBufMgr = GetVideoBufferManager();
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, nEditWidth, nEditHeight, 0, 0};
	CVideoBuffer* pMask = pBufMgr->CreateVideoBuffer(mediaBI);
	return pMask;
}

bool CRenderEngine::SetTargetVideoSize(int nEditWidth, int nEditHeight)
{
	m_TargetVideoSize.cx = nEditWidth;
	m_TargetVideoSize.cy = nEditHeight;

	HRESULT hr;
	SAFE_RELEASE(m_pDepthNoMultiSample);
	if(FAILED(hr = m_pDevice->CreateDepthStencilSurface( nEditWidth, nEditHeight,
		D3DFMT_D24S8,D3DMULTISAMPLE_NONE,0,	FALSE,	&m_pDepthNoMultiSample,NULL )))
	{
	}
	return SUCCEEDED(hr);
}

bool CRenderEngine::SetDepthStencilBuffer(bool bUseDepthBuffer)
{
	HRESULT hr = E_FAIL;
	if(bUseDepthBuffer)
		hr = m_pDevice->SetDepthStencilSurface( m_pDepthNoMultiSample );
	else
		hr = m_pDevice->SetDepthStencilSurface(NULL);
	return SUCCEEDED(hr);
}

//轨间特技的合成规则：
//从下到上，两两合成，合成结果再和更上一层合成，根据上层的Blend模式决定是进行轨间合成还是 Alpha 混合
//Alpha 混合时，CGBLEND 只考虑自身 ALPHA，不考虑全局 Alpha,全局 Alpha 按正常模式进行

bool CRenderEngine::BlendCompose( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB)
{
	RESET_RENDER_TARGET(this);
	//SET_DEPTH_STENCIL(this);

	int num = 2;	//pSrcA and pSrcB
	//TP_VBufferDef *vBufferDst  = m_pResManager->GetBufferDef( hTarget ); 
	const VideoBufferInfo& biDst = pDest->GetVideoBufferInfo();
	CBaseMesh *pMesh           = m_pResMgr->CreateQuadMesh(m_pDevice);
	ASSERT(NULL != pMesh);

	//GAutoLock l(&m_D3Dcs);
	D3DXVECTOR4 cMode;
	float bAlphaOK[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float vAlphaValues[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	float fBlendValues[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	D3DXMATRIX matTexture;
	GenerateMatrix(pSrcA, &matTexture, mat_Identity);

	//TP_VBufferDef *vBufferTemp = NULL; handle_tpr pTempHandle;
	//if( num>2 )
	//{
	//	pTempHandle = m_pResManager->NewRTBuffer(0,0,m_pResManager->isMode16()?vBufferDst->GetImageWidth()*2:vBufferDst->GetImageWidth(),vBufferDst->GetImageHeight());
	//	vBufferTemp = m_pResManager->GetBufferDef( pTempHandle );
	//	vBufferTemp->bDiscardAlpha = vBufferDst->bDiscardAlpha; 
	//}    

	m_pDevice->SetRenderState( D3DRS_CULLMODE           ,D3DCULL_NONE);
	m_pDevice->SetRenderState( D3DRS_ZENABLE            ,FALSE );
	m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE    ,FALSE );
	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE    ,FALSE );    

	m_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER ,D3DTEXF_POINT);
	m_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER ,D3DTEXF_POINT);
	m_pDevice->SetSamplerState(1,D3DSAMP_MAGFILTER ,D3DTEXF_POINT);
	m_pDevice->SetSamplerState(1,D3DSAMP_MINFILTER ,D3DTEXF_POINT);
	m_pDevice->SetSamplerState(2,D3DSAMP_MAGFILTER ,D3DTEXF_POINT);
	m_pDevice->SetSamplerState(2,D3DSAMP_MINFILTER ,D3DTEXF_POINT);
	m_pDevice->SetSamplerState(3,D3DSAMP_MAGFILTER ,D3DTEXF_POINT);
	m_pDevice->SetSamplerState(3,D3DSAMP_MINFILTER ,D3DTEXF_POINT);


	//TP_VBufferDef *vBufferSrc0 = NULL,*vBufferSrc1 = NULL;
	int base = 1;
	//vBufferSrc0    = m_pResManager->GetBufferDef( pSrcHandles[0] );  
	//vBufferSrc1    = m_pResManager->GetBufferDef( pSrcHandles[1] );
	const VideoBufferInfo& vBufferSrc0 = pSrcA->GetVideoBufferInfo();
	const VideoBufferInfo& vBufferSrc1 = pSrcB->GetVideoBufferInfo();
	float fbCGBlended = 0.0f;

	//for(int i=0;i<num-1;i++)
	{		

		//if(NULL == vBufferSrc0)	
		//{
		//	vBufferSrc0    = vBufferDst;  
		//	std::swap(vBufferTemp,vBufferDst);
		//}

		//if(NULL == vBufferSrc1) vBufferSrc1    = m_pResManager->GetBufferDef( pSrcHandles[++base] );

		//fbCGBlended = vBufferSrc1->bIsCG_BlenedBuffer?1.0f:0.0f;


		m_pDevice->SetTexture( 0, pSrcA->GetTexture());
		m_pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );		
		m_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		//只有在自身存在Alpha并且CG_BlenedBuffer、ParticleBlend都不存在的情况下，才考虑自身Alpha
		//if( vBufferSrc0->pAlpha && !(/*vBufferSrc0->bIsCG_BlenedBuffer||*/vBufferSrc0->bParticleBlend))
		//{
		//	bAlphaOK[0] = 1.0;
		//	SetSourceTexture( 1, vBufferSrc0->pAlpha);
		//}
		//else 
		{
			bAlphaOK[0] = 0.0f;
			m_pDevice->SetTexture( 1, NULL );	
		}	

		m_pDevice->SetTexture( 2, pSrcB->GetTexture());
		m_pDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );		
		m_pDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 0 );
		//if(vBufferSrc1->pAlpha && !(/*vBufferSrc1->bIsCG_BlenedBuffer||*/vBufferSrc1->bParticleBlend))
		//{
		//	bAlphaOK[1] = 1.0;
		//	SetSourceTexture( 3, vBufferSrc1->pAlpha);
		//}
		//else 
		{
			bAlphaOK[1] = 0.0f;
			m_pDevice->SetTexture( 3, NULL ); 	
		}	
		vAlphaValues[0] = 1.f;//vBufferSrc0->fAlphaValue;
		vAlphaValues[1] = 1.f;//vBufferSrc1->fAlphaValue;

		fBlendValues[0] =  1.f;//vBufferSrc1->fBlendValueExtra;
		fBlendValues[1] =  1.f;//vBufferSrc1->fBlendValue;	

		//vBufferDst->bClearFullTarget    = true;
		//vBufferDst->bRenderToFullTarget = true;
		//vBufferDst->OffsetX             = 0;
		//vBufferDst->OffsetY             = 0;
		//vBufferDst->rcImage.left        = 0;
		//vBufferDst->rcImage.right       = vBufferDst->BaseWidth;
		//vBufferDst->rcImage.top         = 0;
		//vBufferDst->rcImage.bottom      = vBufferDst->BaseHeight;
		//if(NULL== vBufferDst->pAlpha&&vBufferDst->bDiscardAlpha == false)
		//{
		//	vBufferDst->pAlpha = AllocateVideoAlphaRT();
		//}
		SetRenderTarget(pDest);

		BlendMode vBufferSrc1_dwBlendOp = BLENDMODE_ALPHA;

		D3DXMATRIX *matView = NULL, *matProj= NULL;
		m_pResMgr->GetPerspectiveMatrix( &matView, &matProj);
		D3DXMATRIXA16 matCombine = *matView * *matProj;	

		m_pDevice->SetVertexShaderConstantF(0,(float*)matCombine,4);
		m_pDevice->SetVertexShaderConstantF(4,(float*)&matTexture,4);
		m_pDevice->SetPixelShaderConstantF(0,bAlphaOK,1);	
		m_pDevice->SetPixelShaderConstantF(1,vAlphaValues,1);
		m_pDevice->SetPixelShaderConstantF(2,fBlendValues,1);
		CPixelShader* pShader = m_pResMgr->CreateBlendPixelShader(m_pDevice, vBufferSrc1_dwBlendOp);
		ASSERT(NULL != pShader);
		if(vBufferSrc1_dwBlendOp==BLENDMODE_ALPHA)
		{
			m_pDevice->SetPixelShaderConstantF(3,&fbCGBlended,1);
		}
		m_pDevice->SetPixelShader(pShader->GetPixelShaderPtr());

		if(SUCCEEDED(m_pDevice -> BeginScene())) 
		{
			CVertexShader* pVertexShader = m_pResMgr->CreateVertexShader(m_pDevice, _T("Shaders/VS_DirectOutV3.vsh"));
			ASSERT(NULL != pVertexShader);
			pMesh -> DrawMesh(0, pVertexShader->GetVertexShaderPtr());
			m_pDevice -> EndScene(); 
		}

		//D3DXSaveSurfaceToFile(_T("./BlendCompose_Src_A.dds"), D3DXIFF_DDS, pSrcA->GetSurface(), NULL, NULL);
		//D3DXSaveSurfaceToFile(_T("./BlendCompose_Src_B.dds"), D3DXIFF_DDS, pSrcB->GetSurface(), NULL, NULL);
		//D3DXSaveSurfaceToFile(_T("./BlendCompose_Dest.dds"), D3DXIFF_DDS, pDest->GetSurface(), NULL, NULL);
	}
	for( int i = 0; i < num * 2; i++)
	{
		m_pDevice->SetTexture( i, NULL );
	}	 
	m_pDevice->SetRenderTarget(1, NULL);
	m_pDevice->SetVertexShader(NULL); 
	m_pDevice->SetPixelShader(NULL); 

	return true;
}
