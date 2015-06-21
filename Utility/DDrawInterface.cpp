#include "StdAfx.h"
#include <tchar.h>
#include "DDrawInterface.h"

DDrawInterface::DDrawInterface(void)
: g_hDDrawLib(NULL)
, m_pddraw7(NULL)
{
	_util_initialize_ddraw7();
}

DDrawInterface::~DDrawInterface(void)
{
	_util_finalize_ddraw7();
}

void  DDrawInterface::_util_initialize_ddraw7()
{
	g_hDDrawLib = LoadLibrary(_T("ddraw.dll"));
	if (g_hDDrawLib)
	{
		HRESULT hr = S_OK;
		typedef HRESULT (WINAPI* DDRAWCREATE)(GUID FAR* lpGuid, LPVOID* lplpDD, REFIID iid, IUnknown FAR* pUnkOuter);
		DDRAWCREATE pfDDrawCreate = (DDRAWCREATE)GetProcAddress(g_hDDrawLib, "DirectDrawCreateEx");
		if (pfDDrawCreate)
		{
			hr = pfDDrawCreate(NULL, (void**)&m_pddraw7, IID_IDirectDraw7, NULL);
			m_pddraw7->SetCooperativeLevel(NULL, DDSCL_NORMAL);
		}
	}
}

void DDrawInterface::_util_finalize_ddraw7()
{
	if (m_pddraw7)
	{
		m_pddraw7->Release();
		m_pddraw7 = NULL;
	}
	if (g_hDDrawLib)
	{
		FreeLibrary(g_hDDrawLib);
		g_hDDrawLib = NULL;
	}
}
