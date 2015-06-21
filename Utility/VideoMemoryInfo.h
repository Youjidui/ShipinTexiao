#pragma once

#include "DDrawInterface.h"


class CVideoMemoryInfo
{
public:
	CVideoMemoryInfo(void);
	~CVideoMemoryInfo(void);

	DWORD GetAGPMemSize();
	DWORD GetVideoMemSize();

private:
	DDrawInterface m_ddraw;
};
