#pragma once

#include "../VideoBuffer.h"
#include <set>
#include "../Utility/CriticalSection.h"
#include <d3d9.h>

//private (non-shared) resources, system memory buffer and video memory buffer
class AFX_EXT_CLASS CVideoBufferManager
{
public:
	CVideoBufferManager(IDirect3DDevice9* pDevice);
	~CVideoBufferManager(void);

public:
	virtual CVideoBuffer* CreateVideoBuffer(const VideoBufferInfo &info);
	virtual BOOL ReleaseVideoBuffer(CVideoBuffer*& pBuffer);

private:
	IDirect3DDevice9* m_pDevice;
	CSimpleCriticalSection m_cs;
	std::set<CVideoBuffer*> m_BufferPool;
};
