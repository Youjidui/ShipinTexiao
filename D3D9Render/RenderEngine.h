#pragma once

#include "ResourceManager.h"
#include "VideoBufferManager.h"

#include <DxErr.h>
#pragma comment(lib, "DxErr")

struct D3DDeviceSettings
{
	UINT AdapterOrdinal;
	D3DDEVTYPE DeviceType;
	D3DFORMAT AdapterFormat;
	DWORD BehaviorFlags;
	D3DPRESENT_PARAMETERS pp;
};

//infrastructure and internal interfaces
class AFX_EXT_CLASS CRenderEngine
{
public:
	CRenderEngine(void);
	~CRenderEngine(void);

public:
	virtual HRESULT Create(HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight );

	void SetResourceManager(CResourceManager* pResMgr)	{m_pResMgr = pResMgr;}
	CResourceManager* GetResourceManager()	{return m_pResMgr;}
	void SetVideoBufferManager(CVideoBufferManager* pBuffMgr) {m_pBuffMgr = pBuffMgr; }
	CVideoBufferManager* GetVideoBufferManager()	{return m_pBuffMgr;}
	LPDIRECT3DDEVICE9 GetDevice()	{return m_pDevice;}
	void GetTargetVideoSize(int& nEditWidth, int& nEditHeight);

	//virtual bool UploadConvert(const handle_tpr hColor, const handle_tpr hKey, const handle_tpr hDst, BOOL bHiTrans, long lSourceWidth, long lSourceHeight, NX3DDepthUploadInfo* pDepthInfo = NULL){assert(0);return FALSE;};
	//virtual BOOL DownloadConvert(const handle_tpr hSrc,const handle_tpr hDst, const handle_tpr hKey){assert(0);return FALSE;};
	//bool SetVertexShader(LPCTSTR lpszShaderName);
	CVideoBuffer* CreateRenderTargetBuffer();
	CVideoBuffer* CreateSourceBuffer(int nWidth, int nHeight);

private:
	bool SetDepthBuffer(bool bUseDepthBuffer);
public:
	bool SetRenderTarget( CVideoBuffer* pDest );
	struct ResetRenderTarget
	{
		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DSURFACE9 m_pOldRT;
		ResetRenderTarget(CRenderEngine* pEngine)
			: m_pDevice(pEngine->GetDevice()), m_pOldRT(NULL)
		{
			HRESULT hr = m_pDevice->GetRenderTarget(0, &m_pOldRT);
			ASSERT(SUCCEEDED(hr));
		}
		~ResetRenderTarget()
		{
			HRESULT hr = m_pDevice->SetRenderTarget(0, m_pOldRT);
			ASSERT(SUCCEEDED(hr));
		}
	};
	struct SetDepthStencil
	{
		LPDIRECT3DDEVICE9 m_pDevice;
		LPDIRECT3DSURFACE9 m_pOldDepth;
		SetDepthStencil(CRenderEngine* pEngine)
			: m_pDevice(pEngine->GetDevice()), m_pOldDepth(NULL)
		{
			HRESULT hr = m_pDevice->GetDepthStencilSurface(&m_pOldDepth);
			//ASSERT(SUCCEEDED(hr));
			//if(FAILED(hr))
			//{
			//	LPCTSTR pszErrorString = DXGetErrorString(hr);
			//	LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
			//	TRACE(pszErrorString);
			//	TRACE(pszErrorDesc);
			//}
			bool bOK = pEngine->SetDepthBuffer(true);
			ASSERT(bOK);
		}
		//SetDepthStencil(CRenderEngine* pEngine, CVideoBuffer* pDepth)
		//	: m_pDevice(pEngine->GetDevice()), m_pOldDepth(NULL)
		//{
		//	HRESULT hr = m_pDevice->GetDepthStencilSurface(&m_pOldDepth);
		//	ASSERT(SUCCEEDED(hr));
		//	hr = m_pDevice->SetDepthStencilSurface(pDepth->GetSurface());
		//	ASSERT(SUCCEEDED(hr));
		//}
		~SetDepthStencil()
		{
			HRESULT hr = m_pDevice->SetDepthStencilSurface(m_pOldDepth);
			ASSERT(SUCCEEDED(hr));
		}
	};

public:
	bool EffectVideoCopy( CVideoBuffer* pSrc, CVideoBuffer* pDst  );
	bool BlendCompose( CVideoBuffer* pDest, CVideoBuffer* pSrcA, CVideoBuffer* pSrcB);

private:
	LPDIRECT3D9                 m_pD3D;
	LPDIRECT3DDEVICE9           m_pDevice;    
	D3DDeviceSettings			m_DeviceSettings;
	LPDIRECT3DSURFACE9			m_pDepthNoMultiSample;

	CResourceManager*			m_pResMgr;
	CVideoBufferManager*		m_pBuffMgr;
};


#define RESET_RENDER_TARGET(pEngine) CRenderEngine::ResetRenderTarget _ResetRenderTarget##__LINE__(pEngine)
//#define SET_DEPTH_STENCIL(pEngine,pBuffer) CRenderEngine::SetDepthStencil _##pEngine##pBuffer(pEngine,pBuffer)
#define SET_DEPTH_STENCIL(pEngine) CRenderEngine::SetDepthStencil _SetDepthStencil##__LINE__(pEngine)


