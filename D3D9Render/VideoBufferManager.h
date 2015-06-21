#pragma once

#include "../VideoBuffer.h"
#include <set>
#include "../Utility/CriticalSection.h"

class CVideoBufferManager : public IVideoBufferManager
{
public:
	CVideoBufferManager(IDirect3DDevice9* pDevice);
	~CVideoBufferManager(void);

public:
	virtual IVideoBuffer* CreateVideoBuffer(const VideoBufferInfo &info);
	virtual BOOL ReleaseVideoBuffer(IVideoBuffer*& pBuffer);

private:
	IDirect3DDevice9* m_pDevice;
	CSimpleCriticalSection m_cs;
	std::set<IVideoBuffer*> m_BufferPool;
};
