#include "StdAfx.h"
#include "VideoBuffer.h"
#include "../Utility/SafeDelete.h"
#include <DxErr.h>
#pragma comment(lib, "DxErr")

CVideoBuffer::CVideoBuffer(IDirect3DDevice9* pDevice, const VideoBufferInfo& info)
: m_pDevice(pDevice)
, m_pSurface(NULL)
, m_pTexture(NULL)
{
	memcpy(&m_BufferInfo, &info, sizeof(info));
	Create(pDevice, info);
	m_BufferInfo.nAllocWidth = info.nWidth;
	m_BufferInfo.nAllocHeight = info.nHeight;
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
	switch(info.eUsage)
	{
	case  VideoBufferInfo::_IN:
		{
			{
				hr = pDevice->CreateTexture( info.nWidth,
					info.nHeight,
					1,
					D3DUSAGE_DYNAMIC,// | D3DUSAGE_WRITEONLY,	//D3DLOCK_DISCARD
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&m_pTexture,
					NULL );
				if(SUCCEEDED(hr))
				{
					hr = m_pTexture->GetSurfaceLevel(0, &m_pSurface);
				}
				else
				{
					LPCTSTR pszErrorString = DXGetErrorString(hr);
					LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
					TRACE(pszErrorString);
					TRACE(pszErrorDesc);
				}
			}
		}
		break;
	case  VideoBufferInfo::_IN_OUT:
	default:
		{
			{
				hr = pDevice->CreateTexture( info.nWidth,
					info.nHeight,
					1,
					D3DUSAGE_RENDERTARGET,// | D3DUSAGE_WRITEONLY,
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&m_pTexture,
					NULL );
				if(SUCCEEDED(hr))
				{
					hr = m_pTexture->GetSurfaceLevel(0, &m_pSurface);
				}
				else
				{
					LPCTSTR pszErrorString = DXGetErrorString(hr);
					LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
					TRACE(pszErrorString);
					TRACE(pszErrorDesc);
				}
			}
		}
		break;
	case  VideoBufferInfo::_OUT:
		{
			switch(info.eType)
			{
			case VideoBufferInfo::SYSTEM_MEM:
				{
					m_pTexture = NULL;
					hr = pDevice->CreateOffscreenPlainSurface( info.nWidth,
						info.nHeight,
						D3DFMT_A8R8G8B8,
						D3DPOOL_SYSTEMMEM,
						&m_pSurface,
						NULL );
					if(FAILED(hr))
					{
						LPCTSTR pszErrorString = DXGetErrorString(hr);
						LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
						TRACE(pszErrorString);
						TRACE(pszErrorDesc);
					}
				}
				break;
			case VideoBufferInfo::VIDEO_MEM:
			default:
				{
					m_pTexture = NULL;
					hr = pDevice->CreateRenderTarget( info.nWidth,
						info.nHeight,
						D3DFMT_A8R8G8B8,
						D3DMULTISAMPLE_NONE,
						0,
						TRUE,
						&m_pSurface,
						NULL );
					if(FAILED(hr))
					{
						LPCTSTR pszErrorString = DXGetErrorString(hr);
						LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
						TRACE(pszErrorString);
						TRACE(pszErrorDesc);
					}
				}
				break;
			}
		}
		break;
	}
	ASSERT(NULL != m_pSurface);
	return SUCCEEDED(hr);
}

bool CVideoBuffer::Delete( IDirect3DDevice9* pDevice )
{
	SAFE_RELEASE(m_pSurface);
	SAFE_RELEASE(m_pTexture);
	return true;
}
