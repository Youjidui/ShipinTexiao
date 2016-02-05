#include "StdAfx.h"
#include <assert.h>
#include <d3d9.h>
#include ".\basetexture.h"
#include "VideoBuffer.h"


bool GenerateMatrix(CVideoBuffer* pBuffer, D3DXMATRIX* matDummy , MAT_STYLE matStyle)
{
	if (pBuffer == NULL)
		return false;

	const VideoBufferInfo& bi = pBuffer->GetVideoBufferInfo();
	bool bReverse = false;

	D3DXMatrixIdentity( matDummy );
	float m_fu =  0.5f/(float)(bi.nAllocWidth);
	float m_fv =  0.5f/(float)(bi.nAllocHeight);      	
	float fv = bReverse ? -m_fv:m_fv;
	switch(matStyle)
	{
	case mat_ImageP:
		{
			float fxZoom  =  (bi.nWidth-1.0f) /((float)bi.nAllocWidth);
			float fyZoom  =  (bi.nHeight-1.0f)/((float)bi.nAllocHeight);
			D3DXMatrixScaling ( matDummy, fxZoom, fyZoom, 1.0f );
			//matDummy->_31 = m_fu + pBufferDef->rcImage.left/((float)pBufferDef->BaseWidth);
			//matDummy->_32 = fv   + pBufferDef->rcImage.top/((float)pBufferDef->BaseHeight); 
			matDummy->_31 = m_fu + 0;
			matDummy->_32 = fv   + 0; 

			if( bReverse ) 
			{
				matDummy->_32 +=  matDummy->_22;
				matDummy->_22  = -matDummy->_22;             
			}			
		}
		break;
	case  mat_Image:
		{
			float fxZoom  =  (bi.nWidth - 0.0f) /((float)bi.nAllocWidth);
			float fyZoom  =  (bi.nHeight - 0.0f) /((float)bi.nAllocHeight);
			D3DXMatrixScaling ( matDummy, fxZoom, fyZoom, 1.0f );	
			//matDummy->_31 = m_fu + pBufferDef->rcImage.left/((float)pBufferDef->BaseWidth);
			//matDummy->_32 = fv + pBufferDef->rcImage.top/((float)pBufferDef->BaseHeight); 
			matDummy->_31 = m_fu + 0;
			matDummy->_32 = fv + 0; 

			if( bReverse ) 
			{
				matDummy->_32 +=  matDummy->_22;
				matDummy->_22  = -matDummy->_22;             
			}			
		}
		break;
	case mat_Tci:
		{
			static D3DXMATRIX matScale;
			float fxZoom  = bi.nWidth/((float)bi.nAllocWidth);
			float fyZoom  = bi.nHeight/((float)bi.nAllocHeight);
			D3DXMatrixScaling ( &matScale, fxZoom, fyZoom, 1.0f );	

			matDummy->_11 = 1.0f;         matDummy->_12 =  0.0f;
			matDummy->_21 = 0.0f;         matDummy->_22 =  bReverse?1.0f:-1.0f; 
			matDummy->_31 = 0.0;          matDummy->_32 =  0.0;
			matDummy->_41 = 0.5f + m_fu ;
			matDummy->_42 = 0.5f + fv;
			matDummy->_33 = 0.0f;         matDummy->_44 =  0.0f;    
			*matDummy     =  (*matDummy)*matScale;

		}
		break;

	case mat_Identity:
		{
			matDummy->_31 = m_fu;	 matDummy->_32 = fv;
			if( bReverse )
			{
				matDummy->_32 +=  matDummy->_22;
				matDummy->_22  = -matDummy->_22;
			}	

		}
		break;
	case mat_HalfHW:
		{
			//float fxZoom  =  pBufferDef->GetImageWidth() /((float)pBufferDef->BaseWidth)*0.5;
			//float fyZoom  =  pBufferDef->GetImageHeight() /((float)(pBufferDef->BaseHeight))*0.5;
			float fxZoom  =  bi.nWidth /((float)bi.nAllocWidth)*0.5f;
			float fyZoom  =  bi.nHeight /((float)(bi.nAllocHeight))*0.5f;
			D3DXMatrixScaling ( matDummy, fxZoom, fyZoom, 1.0f );	
			//matDummy->_31 = m_fu + pBufferDef->rcImage.left/((float)pBufferDef->BaseWidth);
			//matDummy->_32 = fv   + pBufferDef->rcImage.top/((float)(pBufferDef->BaseHeight)); 
			matDummy->_31 = m_fu + 0;
			matDummy->_32 = fv   + 0; 
			if( bReverse ) 
			{
				matDummy->_32 +=  matDummy->_22;
				matDummy->_22  = -matDummy->_22;             
			}
		}
		break;
	case mat_HalfH:
		{
			float fxZoom  = bi.nWidth/((float)bi.nAllocWidth);
			float fyZoom  = bi.nHeight/((float)(bi.nAllocHeight*2.0));
			D3DXMatrixScaling ( matDummy, fxZoom, fyZoom, 1.0f );	
			//matDummy->_31 = m_fu + pBufferDef->rcImage.left/((float)pBufferDef->BaseWidth);
			//matDummy->_32 = fv   + pBufferDef->rcImage.top/((float)(pBufferDef->BaseHeight)); 
			matDummy->_31 = m_fu + 0;
			matDummy->_32 = fv   + 0; 

			if( bReverse ) 
			{
				matDummy->_32 +=  matDummy->_22;
				matDummy->_22  = -matDummy->_22;             
			}
		}
		break;
	case mat_HalfW:
		{
		    float fxZoom  = (bi.nWidth - 1.0f)/((float)bi.nAllocWidth)*0.5f;
			float fyZoom  = bi.nHeight/((float)(bi.nAllocHeight));
			D3DXMatrixScaling ( matDummy, fxZoom, fyZoom, 1.0f );
			//matDummy->_31 = m_fu + pBufferDef->rcImage.left/((float)pBufferDef->BaseWidth);
			//matDummy->_32 = fv   + pBufferDef->rcImage.top/((float)(pBufferDef->BaseHeight)); 
			matDummy->_31 = m_fu + 0;
			matDummy->_32 = fv   + 0; 

			if( bReverse ) 
			{
				matDummy->_32 +=  matDummy->_22;
				matDummy->_22  = -matDummy->_22;             
			}
		}
		break;
	case mat_DoubleW:
		{
			float fu = 0.5f/bi.nAllocWidth; float fv = 0.5f/ bi.nAllocHeight;
            float fxZoom  = bi.nWidth/((float)bi.nAllocWidth)*2.0f;
			float fyZoom  = bi.nHeight/((float)(bi.nAllocHeight));
			D3DXMatrixScaling ( matDummy, fxZoom, fyZoom, 1.0f );
			//matDummy->_31 = fu + pBufferDef->rcImage.left/((float)pBufferDef->BaseWidth);
			//matDummy->_32 = fv + pBufferDef->rcImage.top/((float)(pBufferDef->BaseHeight));
			matDummy->_31 = fu + 0;
			matDummy->_32 = fv + 0;
			if( bReverse ) 
			{
				matDummy->_32 +=  matDummy->_22;
				matDummy->_22  = -matDummy->_22;             
			}
		}
		break;
    case mat_DoubleFW:
        {
			float fu = 0.5f/bi.nAllocWidth;
			float fv = 0.5f/ bi.nAllocHeight;
            float fxZoom  = 2.0f;   float fyZoom  = 1.0f;
            D3DXMatrixScaling ( matDummy, fxZoom, fyZoom, 1.0f );	
            matDummy->_31 = fu;     matDummy->_32 = fv; 			
            if( bReverse ) 
            {
                matDummy->_32 +=  matDummy->_22;
                matDummy->_22  = -matDummy->_22;             
            }
        }
        break;
        
	}
	return true;
}

LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 pDevice, CVideoBuffer* pSrc)
{
	const VideoBufferInfo& srcBuffInfo = pSrc->GetVideoBufferInfo();
	LPDIRECT3DTEXTURE9 lpTex = NULL;
	if(SUCCEEDED(pDevice->CreateTexture(srcBuffInfo.nWidth, srcBuffInfo.nHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &lpTex, NULL)))
	{
		LPDIRECT3DSURFACE9 lpTexSurf = NULL;
		if(SUCCEEDED(lpTex->GetSurfaceLevel(0, &lpTexSurf)))
		{
			if(SUCCEEDED(pDevice->UpdateSurface(pSrc->GetSurface(), NULL, lpTexSurf, NULL)))
			{
			}
			lpTexSurf->Release();
		}
	}
	return lpTex;
}


CBaseTexture::CBaseTexture(void)
:m_pTexture( NULL )
,m_dwUsage(0 )
,m_d3dPool( D3DPOOL_DEFAULT )
,m_d3dFormat( D3DFMT_A8R8G8B8)
,m_pDCSurface(NULL)
,m_hDC(NULL)
{	
	//m_imageInfo    = new TImageInfo;
	//m_imageInfo->Base_Width   = m_imageInfo->Base_Height = 0;	
	m_uSizeInBytes = 0;
}


CBaseTexture::~CBaseTexture(void)
{	 
	//delete m_imageInfo;
	//m_imageInfo = NULL;
}

HRESULT CBaseTexture::Create( LPDIRECT3DDEVICE9 pDevice,
							 LPCTSTR            szFileName,
							 UINT               nWidth,
							 UINT               nHeight,
							 DWORD              dwUsage, 
							 D3DFORMAT          format,
							 D3DPOOL            pool,
							 LPCTSTR            pszResID)
{
	m_pDevice =  pDevice;
	m_strResID                  =  pszResID;

	m_dwUsage                 = dwUsage;  

	//m_imageInfo->Base_Width   = nWidth;
	//m_imageInfo->Base_Height  = nHeight;
	//m_imageInfo->bFree        = true;

	m_d3dFormat               = format;

	m_d3dPool = pool;

	//if(pool == D3DPOOL_SYSTEMMEM)
	//{
	//	m_imageInfo->eMemType = MEM_SYSTEM;
	//}

	{
		if(SUCCEEDED( D3DXCreateTextureFromFileEx( m_pDevice,
			szFileName,
			nWidth > 0 ? nWidth : D3DX_DEFAULT_NONPOW2,
			nHeight > 0 ? nHeight : D3DX_DEFAULT_NONPOW2,
			1,
			m_dwUsage,
			m_d3dFormat,
			pool,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			0,
			NULL,
			NULL,
			&m_pTexture)))
		{
			D3DSURFACE_DESC desc;
			(m_pTexture)->GetLevelDesc(0,&desc);
			//m_imageInfo->Base_Width   = desc.Width;
			//m_imageInfo->Base_Height  = desc.Height;
			m_d3dFormat = desc.Format;
			m_uSizeInBytes = desc.Width * desc.Height * GetPixelSizeInBitsByFormat(m_d3dFormat)/8;
			return S_OK;
		}
	}

	return E_FAIL;
}

HDC  CBaseTexture::GetDC()
{
  FreeDC();
  m_pDCSurface = GetSurface();
  m_pDCSurface->GetDC(&m_hDC);
  return m_hDC;
}

void  CBaseTexture::FreeDC()
{
	if(m_hDC) 
	{
		m_pDCSurface -> ReleaseDC(m_hDC);
		m_hDC = NULL;
	}
	if(m_pDCSurface)
	{
		m_pDCSurface -> Release();
		m_pDCSurface = NULL;
	}

}

HRESULT CBaseTexture::Create( LPDIRECT3DDEVICE9 pDevice,
							 UINT              nWidth,
							 UINT              nHeight,
							 DWORD             dwUsage,
							 D3DFORMAT         format,
							 D3DPOOL           pool,
							 LPCTSTR            pszResID)
{
	m_pDevice					=  pDevice;
	m_strResID                  =  pszResID;

	m_dwUsage                 = dwUsage;  

	//m_imageInfo->Base_Width   = nWidth;
	//m_imageInfo->Base_Height  = nHeight;
	//m_d3dFormat               = format;
	//m_imageInfo->bFree        = true;
   
	m_d3dPool        = pool;

	//if(pool == D3DPOOL_SYSTEMMEM)
	//{
	//	m_imageInfo->eMemType = MEM_SYSTEM;
	//}

	if(SUCCEEDED( m_pDevice->CreateTexture( nWidth,
		nHeight,
		(m_dwUsage&D3DUSAGE_AUTOGENMIPMAP)?0:1,
		m_dwUsage,
		m_d3dFormat,
		m_d3dPool,
		&m_pTexture,
		NULL)))
	{
		if(m_dwUsage&D3DUSAGE_AUTOGENMIPMAP)
			m_pTexture->SetAutoGenFilterType( D3DTEXF_ANISOTROPIC);

		//强制纹理分配内存,避免Upload出错
		if(m_dwUsage == D3DUSAGE_DYNAMIC)
		{
			LPDIRECT3DTEXTURE9 pTexture = m_pTexture;
			D3DLOCKED_RECT lr;
			pTexture->LockRect(0,&lr,NULL,0);
			pTexture->UnlockRect(0);
		}
		m_uSizeInBytes = nWidth * nHeight * GetPixelSizeInBitsByFormat(m_d3dFormat)/8;
		return S_OK;
	}

	return E_FAIL;
}


HRESULT   CBaseTexture::OnLost()
{
	FreeDC();
	if(m_d3dPool == D3DPOOL_DEFAULT && m_pTexture)
	{
		HRESULT hr = m_pTexture->Release();
		m_pTexture = NULL;
		return hr;
	}
	return S_OK;
}

HRESULT  CBaseTexture::Destroy()
{
	FreeDC();
	if(m_pTexture)
	{
		ULONG  nRef = m_pTexture->Release();
		m_pTexture = NULL;
		if(!nRef) return S_OK;
		return E_FAIL;
	}
	return S_OK;
}



HRESULT CBaseTexture::OnRestore()
{
	//if( m_d3dPool == D3DPOOL_DEFAULT && m_pDevice )
	//{
	//	HRESULT hr = S_OK;
	//	if(FAILED(hr =  m_pDevice->CreateTexture( m_imageInfo->Base_Width,
	//		m_imageInfo->Base_Height,
	//		(m_dwUsage&D3DUSAGE_AUTOGENMIPMAP)?0:1,
	//		m_dwUsage,
	//		m_d3dFormat,
	//		m_d3dPool,
	//		(LPDIRECT3DTEXTURE9*)&m_pTexture,
	//		NULL)))
	//	{				
	//		if(hr == D3DERR_INVALIDCALL)
	//		{
	//			MessageBox(NULL,L"INVALIDCALL",L"D3D ERROR",MB_OK|MB_ICONERROR);
	//		}
	//		else if(hr == D3DERR_OUTOFVIDEOMEMORY)
	//		{
	//			MessageBox(NULL,L"OUTOFVIDEOMEMORY",L"D3D ERROR",MB_OK|MB_ICONERROR);
	//		}
	//		else if(hr == E_OUTOFMEMORY)
	//		{
	//			MessageBox(NULL,L"OUTOFMEMORY",L"D3D ERROR",MB_OK|MB_ICONERROR);
	//		}	
	//		else
	//		{
	//			MessageBox(NULL,L"UNKNOW ERROR",L"D3D ERROR",MB_OK|MB_ICONERROR);
	//		}
	//		return E_FAIL;
	//	}

	//	if(m_dwUsage&D3DUSAGE_AUTOGENMIPMAP) m_pTexture->SetAutoGenFilterType(  D3DTEXF_ANISOTROPIC);
	//}
	return S_OK;
}

HRESULT CBaseTexture::Unlock()
{		
	(m_pTexture)->UnlockRect(0); 
	return S_OK;
}

LPDIRECT3DSURFACE9 CBaseTexture::GetSurface()
{
	LPDIRECT3DSURFACE9 pSurf;
	(m_pTexture)->GetSurfaceLevel(0,&pSurf);
	return pSurf;

}
LPDIRECT3DTEXTURE9 CBaseTexture::GetTexture()
{
	
	return m_pTexture;
}

// 根据类型获取一个像素点的位大小
UINT CBaseTexture::GetPixelSizeInBitsByFormat(D3DFORMAT d3dPool)
{
	UINT uPixelSizeInBits = 32;
	switch (d3dPool)
	{
	// Buffer Formats
	case D3DFMT_D16_LOCKABLE:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_D32:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_D15S1:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_D24S8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_D24X8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_D24X4S4:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_D32F_LOCKABLE:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_D24FS8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_D32_LOCKABLE:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_S8_LOCKABLE:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_D16:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_INDEX16:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_INDEX32:
		uPixelSizeInBits = 32;
		break;
	// DXTn Compressed Texture Formats
	case D3DFMT_DXT1:
		uPixelSizeInBits = 4;
		break;
	case D3DFMT_DXT2:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_DXT3:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_DXT4:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_DXT5:
		uPixelSizeInBits = 8;
		break;
	// Floating-Point Formats
	case D3DFMT_R16F:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_G16R16F:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_A16B16G16R16F:
		uPixelSizeInBits = 64;
		break;
	// IEEE Formats
	case D3DFMT_R32F:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_G32R32F:
		uPixelSizeInBits = 64;
		break;
	case D3DFMT_A32B32G32R32F:
		uPixelSizeInBits = 128;
		break;
	// Mixed Formats
	case D3DFMT_L6V5U5:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_X8L8V8U8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_A2W10V10U10:
		uPixelSizeInBits = 32;
		break;
	// Signed Formats
	case D3DFMT_V8U8:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_Q8W8V8U8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_V16U16:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_Q16W16V16U16:
		uPixelSizeInBits = 64;
		break;
	case D3DFMT_CxV8U8:
		uPixelSizeInBits = 16;
		break;
	// Unsigned Formats
	case D3DFMT_R8G8B8:
		uPixelSizeInBits = 24;
		break;
	case D3DFMT_A8R8G8B8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_X8R8G8B8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_R5G6B5:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_X1R5G5B5:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_A1R5G5B5:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_A4R4G4B4:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_R3G3B2:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_A8:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_A8R3G3B2:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_X4R4G4B4:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_A2B10G10R10:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_A8B8G8R8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_X8B8G8R8:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_G16R16:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_A2R10G10B10:
		uPixelSizeInBits = 32;
		break;
	case D3DFMT_A16B16G16R16:
		uPixelSizeInBits = 64;
		break;
	case D3DFMT_A8P8:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_P8:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_L8:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_L16:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_A8L8:
		uPixelSizeInBits = 16;
		break;
	case D3DFMT_A4L4:
		uPixelSizeInBits = 8;
		break;
	case D3DFMT_A1:
		uPixelSizeInBits = 1;
		break;
	default:
		assert(FALSE);
		break;
	}

	return uPixelSizeInBits;
}
