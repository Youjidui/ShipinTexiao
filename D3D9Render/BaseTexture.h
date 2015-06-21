#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

enum MAT_STYLE{ mat_Image,mat_HalfHW,mat_HalfH,mat_HalfW,mat_DoubleW,mat_DoubleFW,mat_Tci,mat_Identity,mat_ImageP,mat_None };

enum Buffer_Format
{
	FMT_RGBA32,
	FMT_YUVA32, 
	FMT_RGB24,   
	FMT_UYVY,
	FMT_YUYV,      
	FMT_A8,
	FMT_YUYV16,
	FMT_UYVY16,
};

enum MEMTYPE{ MEM_VIDEO, MEM_SYSTEM };

struct TImageInfo
{
	UINT			Base_Width;
	UINT			Base_Height;
	Buffer_Format	Base_Format;
	bool			bFree;
	MEMTYPE			eMemType;

	TImageInfo( int width,int height )
	{
		Base_Width  = width;
		Base_Height = height;
		Base_Format = FMT_RGBA32;		
		bFree       = true; 
		eMemType	= MEM_VIDEO;
	};

	TImageInfo()
	{
		Base_Width      = 720;
		Base_Height     = 576;
		bFree           = true; 
		eMemType		= MEM_VIDEO;
		Base_Format		= FMT_RGBA32;
	};
};



class  CBaseTexture
{
public:
	CBaseTexture(void);
	virtual ~CBaseTexture(void); 
public:
  //创建纹理
  HRESULT Create( LPDIRECT3DDEVICE9 pDevice,
	              UINT               nWidth,
                  UINT               nHeight,
				  DWORD              dwUsage, 
                  D3DFORMAT          format,
				  D3DPOOL            pool,
                  LPCTSTR            pszResID);

  HRESULT Create( LPDIRECT3DDEVICE9 pDevice,
                  TCHAR*             szFileName,
                  UINT               nWidth,
                  UINT               nHeight,
                  DWORD              dwUsage, 
                  D3DFORMAT          format,
                  D3DPOOL            pool,
                  LPCTSTR            pszResID);
  
  //恢复纹理
  
  
  HRESULT         Unlock();

    TImageInfo*             GetImagesInfo(){ return  m_imageInfo;};         
  D3DFORMAT               GetFormat() { return   m_d3dFormat;}; 
  HDC                     GetDC();
  void                    FreeDC();
  LPDIRECT3DTEXTURE9	  GetTexture();
  LPDIRECT3DSURFACE9      GetSurface();
  const D3DXMATRIX*       GetMatrix( MAT_STYLE matStyle,const int index = 0 );
  void                    GetTextureSize( int& nWidth,int& nHeight )
                           { nWidth  = m_imageInfo->Base_Width;
                             nHeight = m_imageInfo->Base_Height;};

  UINT                    GetTextureWidth(){ return m_imageInfo->Base_Width; };
  UINT                    GetTextureHeight(){ return m_imageInfo->Base_Height;};
  UINT                    GetSizeInBytes(){ return m_uSizeInBytes;}

protected:
  HRESULT         OnLost();
  HRESULT         OnRestore();
  HRESULT         Destroy();   
  UINT            GetPixelSizeInBitsByFormat(D3DFORMAT d3dPool);	// 根据类型获取一个像素点的位大小

protected:
	LPDIRECT3DDEVICE9 m_pDevice;
	std::wstring m_strResID;
  TImageInfo*               m_imageInfo;
  D3DFORMAT                 m_d3dFormat;
  LPDIRECT3DTEXTURE9    m_pTexture;  
  LPDIRECT3DSURFACE9        m_pDCSurface;
  HDC                       m_hDC;
  DWORD                     m_dwUsage;
  D3DPOOL                   m_d3dPool;
  UINT						m_uSizeInBytes;	// 纹理所占显存的大小(单位:字节)
};

//void GenerateMatrix(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX* matDummy, MAT_STYLE matStyle);
//void GenerateMatrix( LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX* matDummy, MAT_STYLE matStyle);
