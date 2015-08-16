#include "StdAfx.h"
#include "VideoBuffer.h"
#include "../Utility/SafeDelete.h"

CVideoBuffer::CVideoBuffer(IDirect3DDevice9* pDevice, const VideoBufferInfo& info)
: m_pDevice(pDevice)
, m_pSurface(NULL)
{
	memcpy(&m_BufferInfo, &info, sizeof(info));
	Create(pDevice, info);
}

CVideoBuffer::~CVideoBuffer(void)
{
	Delete(m_pDevice);
}

const VideoBufferInfo& CVideoBuffer::GetVideoBufferInfo() const
{
	return m_BufferInfo;
}

void* CVideoBuffer::LockBuffer( int &Pitch )
{
	D3DLOCKED_RECT rc;
	if(SUCCEEDED(m_pSurface->LockRect(&rc, NULL, 0)))	//D3DLOCK_DISCARD
	{
		Pitch = rc.Pitch;
		return rc.pBits;
	}
	return NULL;
}

BOOL CVideoBuffer::UnLockBuffer()
{
	return m_pSurface->UnlockRect();
}

bool CVideoBuffer::Create(IDirect3DDevice9* pDevice, const VideoBufferInfo& info )
{
	HRESULT hr = E_FAIL;
	switch(info.eType)
	{
	case VBT_SYSTEM_MEM:
		{
			hr = pDevice->CreateOffscreenPlainSurface( info.nWidth,
				info.nHeight,
				D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM,
				&m_pSurface,
				NULL );
		}
		break;
	case VBT_D3D9_MEM:
		{
			hr = pDevice->CreateRenderTarget( info.nWidth,
				info.nHeight,
				D3DFMT_A8R8G8B8,
				D3DMULTISAMPLE_NONE,
				0,
				TRUE,
				&m_pSurface,
				NULL );
		}
		break;
	default:
		break;
	}
	return SUCCEEDED(hr);
}

bool CVideoBuffer::Delete( IDirect3DDevice9* pDevice )
{
	SAFE_RELEASE(m_pSurface);
	return true;
}
