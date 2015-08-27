#include "StdAfx.h"
#include "RenderEngine.h"
#include "../Utility/mathmacros.h"

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

	RECT rcClient;
	GetClientRect(GetDesktopWindow(),&rcClient);
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
