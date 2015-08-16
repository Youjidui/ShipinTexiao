// VideoEffect.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "../D3D9Render/VideoBufferManager.h"
#include "../D3D9Render/RenderEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRenderEngine* g_pRenderEngine = NULL;
CVideoBufferManager* g_pVideoBufferManager = NULL;

IVideoBufferManager* CreateVideoBufferManager()
{
	if(!g_pVideoBufferManager && g_pRenderEngine && g_pRenderEngine->GetDevice())
		g_pVideoBufferManager = new CVideoBufferManager(g_pRenderEngine->GetDevice());
	return g_pVideoBufferManager;
}

void ReleaseVideoBufferManager(IVideoBufferManager* p)
{
	delete p;
}

bool InitEffectModule(HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight )
{
	bool bOK = false;
	if(!g_pRenderEngine)
	{
		g_pRenderEngine = new CRenderEngine;
		HRESULT hr = E_FAIL;
		if(g_pRenderEngine)
			hr = g_pRenderEngine->Create(hDeviceWnd, nBackBufferWidth, nBackBufferHeight);
		bOK = SUCCEEDED(hr);
	}
	return bOK;
}

void UninitEffectModule()
{
	if(g_pRenderEngine)
	{
		delete g_pRenderEngine;
	}
}
