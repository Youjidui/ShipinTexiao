#pragma once

#include "../VideoBuffer.h"
#include <d3d9.h>

class CVideoBuffer
{
public:
	CVideoBuffer(IDirect3DDevice9* pDevice, const VideoBufferInfo& info);
	~CVideoBuffer(void);

private:
	bool Create(IDirect3DDevice9* pDevice, const VideoBufferInfo& info);
	bool Delete(IDirect3DDevice9* pDevice);

public:
	virtual const VideoBufferInfo& GetVideoBufferInfo() const;

	virtual void* LockBuffer(int &Pitch);
	virtual BOOL UnLockBuffer();

	LPDIRECT3DSURFACE9 GetSurface()
	{
		return m_pSurface;
	}

private:
	IDirect3DDevice9* m_pDevice;
	VideoBufferInfo m_BufferInfo;
	IDirect3DSurface9* m_pSurface;
};
