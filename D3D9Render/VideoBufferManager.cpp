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

CVideoBuffer* CVideoBufferManager::CreateVideoBuffer( const VideoBufferInfo &info )
{
	CVideoBuffer* pBuffer = new CVideoBuffer(m_pDevice, info);
	CSimpleCriticalSection::Lock l(m_cs);
	m_BufferPool.insert(pBuffer);
	return pBuffer;
}

BOOL CVideoBufferManager::ReleaseVideoBuffer( CVideoBuffer*& pBuffer )
{
	CSimpleCriticalSection::Lock l(m_cs);
	std::set<CVideoBuffer*>::iterator i = m_BufferPool.find(pBuffer);
	if(m_BufferPool.end() != i)
	{
		delete *i;
		m_BufferPool.erase(i);
	}
	return TRUE;
}
