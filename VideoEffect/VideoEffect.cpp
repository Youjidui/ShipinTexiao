// VideoEffect.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../D3D9Render/VideoBufferManager.h"
#include "../D3D9Render/RenderEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CVideoBufferManager* CreateVideoBufferManager(CRenderEngine* pEngine)
{
	CVideoBufferManager* p = new CVideoBufferManager(pEngine->GetDevice());
	pEngine->SetVideoBufferManager(p);
	return p;
}

void ReleaseVideoBufferManager(CVideoBufferManager* p)
{
	delete p;
}

CRenderEngine* InitEffectModule(HWND hDeviceWnd, UINT nBackBufferWidth, UINT nBackBufferHeight )
{
	CRenderEngine* p = new CRenderEngine;
	HRESULT hr = E_FAIL;
	if(p)
	{
		hr = p->Create(hDeviceWnd, nBackBufferWidth, nBackBufferHeight);
		if(SUCCEEDED(hr))
		{
			CResourceManager* pRM = new CResourceManager;
			if(pRM)
			{
				p->SetResourceManager(pRM);
			}
		}
		else
		{
			delete p;
			p = NULL;
		}
	}
	return p;
}

void UninitEffectModule(CRenderEngine* pEngine)
{
	if(pEngine)
	{
		CResourceManager* pRM = pEngine->GetResourceManager();
		pEngine->SetResourceManager(NULL);
		delete pRM;
	}
	delete pEngine;
}
