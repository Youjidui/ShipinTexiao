#pragma once

#include "ResourceManager.h"

struct D3DDeviceSettings
{
	UINT AdapterOrdinal;
	D3DDEVTYPE DeviceType;
	D3DFORMAT AdapterFormat;
	DWORD BehaviorFlags;
	D3DPRESENT_PARAMETERS pp;
};


class AFX_EXT_CLASS CRenderEngine
{
public:
	CRenderEngine(void);
	~CRenderEngine(void);

public:
	virtual HRESULT Create(HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight );
	void SetResourceManager(CResourceManager* pResMgr)	{m_pResMgr = pResMgr;}
	CResourceManager* GetResourceManager()	{return m_pResMgr;}
	LPDIRECT3DDEVICE9 GetDevice()	{return m_pDevice;}
	//virtual bool UploadConvert(const handle_tpr hColor, const handle_tpr hKey, const handle_tpr hDst, BOOL bHiTrans, long lSourceWidth, long lSourceHeight, NX3DDepthUploadInfo* pDepthInfo = NULL){assert(0);return FALSE;};
	//virtual BOOL DownloadConvert(const handle_tpr hSrc,const handle_tpr hDst, const handle_tpr hKey){assert(0);return FALSE;};
	bool SetVertexShader(LPCTSTR lpszShaderName);

private:
	LPDIRECT3D9                  m_pD3D;
	LPDIRECT3DDEVICE9            m_pDevice;    
	D3DDeviceSettings			 m_DeviceSettings;

	CResourceManager*			m_pResMgr;
};
