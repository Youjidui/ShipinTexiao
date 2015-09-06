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

