#include "StdAfx.h"
#include "VideoBufferManager.h"
#include "VideoBuffer.h"

CVideoBufferManager::CVideoBufferManager(IDirect3DDevice9* pDevice)
: m_pDevice(pDevice)
{
}

CVideoBufferManager::~CVideoBufferManager(void)
{
}

IVideoBuffer* CVideoBufferManager::CreateVideoBuffer( const VideoBufferInfo &info )
{
	IVideoBuffer* pBuffer = new CVideoBuffer(m_pDevice, info);
	CSimpleCriticalSection::Lock l(m_cs);
	m_BufferPool.insert(pBuffer);
	return pBuffer;
}

BOOL CVideoBufferManager::ReleaseVideoBuffer( IVideoBuffer*& pBuffer )
{
	CSimpleCriticalSection::Lock l(m_cs);
	std::set<IVideoBuffer*>::iterator i = m_BufferPool.find(pBuffer);
	if(m_BufferPool.end() != i)
	{
		delete *i;
		m_BufferPool.erase(i);
	}
	return TRUE;
}
