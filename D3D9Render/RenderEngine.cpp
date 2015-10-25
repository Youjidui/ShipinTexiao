#include "StdAfx.h"
#include "RenderEngine.h"
#include "../Utility/mathmacros.h"
#include "VideoBuffer.h"
#include "../Utility/SafeDelete.h"

CRenderEngine::CRenderEngine(void)
: m_pD3D(NULL)
, m_pDevice(NULL)
, m_pResMgr(NULL)
, m_pBuffMgr(NULL)
{
	ZeroMemory(&m_DeviceSettings, sizeof(m_DeviceSettings));
	ZeroMemory(&m_DeviceSettings.pp, sizeof(m_DeviceSettings.pp));
}

CRenderEngine::~CRenderEngine(void)
{
}

HRESULT CRenderEngine::Create( HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight )
{
	HRESULT hr = E_FAIL;
	//Initialize Direct3D 9.0
	m_pD3D        = Direct3DCreate9( D3D_SDK_VERSION );

	m_DeviceSettings.pp.Windowed               = true;
	m_DeviceSettings.pp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

	m_DeviceSettings.pp.BackBufferWidth        = nBackBufferWidth ;
	m_DeviceSettings.pp.BackBufferHeight       = nBackBufferHeight;  

	//RECT rcClient;
	//GetClientRect(GetDesktopWindow(),&rcClient);
	//m_DeviceSettings.pp.BackBufferWidth = UINT(RECTWIDTH(rcClient) * 0.99f);
	//m_DeviceSettings.pp.BackBufferHeight = UINT(RECTHEIGHT(rcClient) * 0.8f);

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

	//m_pDevice->CreateAdditionalSwapChain(&m_DeviceSettings.pp,&m_pSwapChain);
	//m_pSwapChain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&m_pSurfBackBuffer);
	return hr;
}

bool CRenderEngine::SetVertexShader( LPCTSTR lpszShaderName )
{
	HRESULT hr = E_FAIL;
	CVertexShader* pShader = m_pResMgr->CreateVertexShader(m_pDevice, lpszShaderName);
	if(pShader)
	{
		hr = m_pDevice->SetVertexShader(pShader->GetVertexShaderPtr());
	}
	return SUCCEEDED(hr);
}

bool CRenderEngine::SetRenderTarget( CVideoBuffer* pDest )
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LPDIRECT3DSURFACE9 pRTSurface = NULL;
	if(pDest)
	{
		pRTSurface = pDest->GetSurface();
	}

	// set render target
	pDevice->SetRenderTarget(0, pRTSurface);
	//SAFE_RELEASE(pRTSurface);
	pDevice->SetRenderTarget(1, NULL);

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
		pDevice->SetViewport(&vp);

		pDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	}
	return !!pRTSurface;
}

void CRenderEngine::GetTargetVideoSize( int& nEditWidth, int& nEditHeight )
{
	nEditWidth = m_DeviceSettings.pp.BackBufferWidth;
	nEditHeight = m_DeviceSettings.pp.BackBufferHeight;
}

bool CRenderEngine::EffectVideoCopy( CVideoBuffer* pSrc, CVideoBuffer* pDst  )
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	CResourceManager* pResMan = GetResourceManager();
	//这个函数只用于特技缺省Copy ，由调用者决定是否使用Zbuffer
	//CZBufferManager zbuffer(this,DEPTHSTENCIL_NONE);

	//TP_VBufferDef *vBufferSrc = m_pResManager->GetBufferDef( hSrc );
	//TP_VBufferDef *vBufferDst = m_pResManager->GetBufferDef( hDst );
	//assert( vBufferSrc->bufferFormat == vBufferDst->bufferFormat );	
	CBaseMesh *pMesh          = GetResourceManager()->CreateQuadMesh(GetDevice());
	const VideoBufferInfo& srcBI = pSrc->GetVideoBufferInfo();
	const VideoBufferInfo& destBI = pDst->GetVideoBufferInfo();
	//GAutoLock l(&m_D3Dcs);
	//UINT PSShader,VSShader;
	//D3DXMATRIX matWorld,matScale;
	D3DXMATRIX 	matTexture;  
	float f_Offsetx;
	//long nWidth,nHeight;
	float vBufferSrcOffsetY = 0.0f;
	f_Offsetx = vBufferSrcOffsetY;
	int iTargetImageWidth  = srcBI.nWidth;
	int iTargetImageHeight = srcBI.nHeight;
	int RTTextureWidth     = destBI.nAllocWidth;
	int RTTextureHeight    = destBI.nAllocHeight;

	D3DXMATRIX  matWorld, *pMatView = NULL, *pMatProj = NULL;
	D3DXMatrixIdentity(&matWorld);
	GetResourceManager()->GetOrthoMatrix(&pMatView, &pMatProj);
	D3DXMATRIX matWVP = matWorld * (*pMatView)* (*pMatProj);
	D3DXMATRIX matCC = (*pMatView)*(*pMatProj);
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE ); 
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE ,FALSE ); 	

	D3DXVECTOR4 vMeshArgs(  f_Offsetx/(float)RTTextureWidth,
		vBufferSrcOffsetY/(float)RTTextureHeight,
		iTargetImageWidth/(float)RTTextureWidth,
		iTargetImageHeight/(float)RTTextureHeight);
	pDevice->SetTexture( 0,pSrc->GetTexture());
	//if(vBufferSrc->pAlpha )	SetSourceTexture( 1,vBufferSrc->pAlpha );
	GenerateMatrix( pSrc, &matTexture, mat_Image );	

	//vBufferDst->bRenderToFullTarget = true;
	//vBufferDst->bClearFullTarget    = true;
	//vBufferDst->bDiscardAlpha       = false;
	//if(vBufferSrc->pAlpha&&!vBufferDst->pAlpha)
	//{
	//	vBufferDst->pAlpha= AllocateVideoAlphaRT();
	//}
	//SetRenderTarget( 0, hDst,D3DCLEAR_TARGET,vBufferDst->COLOR_BLACK(), 0,1.0f,0 );	// --zms-- yuyv无论何种情况，都清除为黑色（而不是绿色）。
	SetRenderTarget(pDst);

	float bHaveAlpha     = 0.0f;//vBufferSrc->pAlpha?1.0:0.0;
	float bParticleBlend = 0.0f;//vBufferSrc->bParticleBlend?1.0:0.0;

	CVertexShader* pVS = pResMan->CreateVertexShader(pDevice, _T("Shaders/VS_SplitField.VSH"));
	CPixelShader* pPS = pResMan->CreatePixelShader(pDevice, _T("Shaders/PS_DirectOutEffect.PSH"));

	if(SUCCEEDED(pDevice -> BeginScene()))
	{				 
		pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
		pDevice->SetPixelShader(pPS->GetPixelShader());
		pDevice->SetPixelShaderConstantF(0,&bHaveAlpha,1);
		pDevice->SetPixelShaderConstantF(1,&bParticleBlend,1);
		pDevice->SetVertexShaderConstantF( 0, (float*)&matCC,      4 );
		pDevice->SetVertexShaderConstantF( 4, (float*)&matTexture, 4 );
		pDevice->SetVertexShaderConstantF( 8, (float*)&vMeshArgs,  1 );

		pMesh -> DrawMesh(0, pVS->GetVertexShaderPtr());
		pDevice->EndScene();
	}

	pDevice->SetPixelShader( NULL );
	pDevice->SetRenderTarget(0,NULL);
	
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

