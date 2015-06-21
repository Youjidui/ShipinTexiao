#include "StdAfx.h"
#include "VideoMemoryInfo.h"

CVideoMemoryInfo::CVideoMemoryInfo(void)
{
}

CVideoMemoryInfo::~CVideoMemoryInfo(void)
{
}

// return the available video memory resides in display card (local)
DWORD CVideoMemoryInfo::GetVideoMemSize()
{
	DDSCAPS2    ddsCaps2; 
	DWORD       dwLocalFree, dwLocalTotal;

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
	ddsCaps2.dwCaps = DDSCAPS_LOCALVIDMEM|DDSCAPS_VIDEOMEMORY;
	m_ddraw.GetDDraw()->GetAvailableVidMem(&ddsCaps2, &dwLocalTotal, &dwLocalFree);

	return dwLocalFree ;	
}

// return the available video memory resides in AGP system (non-local)
DWORD CVideoMemoryInfo::GetAGPMemSize()
{
	DDSCAPS2    ddsCaps2; 
	DWORD       dwNonLocalFree, dwNonLocalTotal;

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
	ddsCaps2.dwCaps = DDSCAPS_NONLOCALVIDMEM|DDSCAPS_VIDEOMEMORY;
	m_ddraw.GetDDraw()->GetAvailableVidMem(&ddsCaps2, &dwNonLocalTotal, &dwNonLocalFree);

	return dwNonLocalFree ;
}
