#pragma once

#include <d3dx9math.h>

enum Buffer_Color_Format
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

class ColorConvertor
{
public:
	static void RGBA2YUVA(const D3DXCOLOR *rgba, D3DXCOLOR *yuva);
	static void YUVA2RGBA(const D3DXCOLOR *yuva, D3DXCOLOR *rgba);

	static void RGBA2YUYV(const D3DXCOLOR *rgba, D3DXCOLOR *yuyv);
	static void RGBA2UYVY(const D3DXCOLOR *rgba, D3DXCOLOR *uyvy);

	static void YUYV2RGBA(const D3DXCOLOR *yuyv, D3DXCOLOR *rgba);
	static void UYVY2RGBA(const D3DXCOLOR *uyvy, D3DXCOLOR *rgba);

	static void RGBA2(const Buffer_Color_Format fmt,D3DXCOLOR *crSrc,D3DXCOLOR *crDest);

	static const D3DXMATRIXA16* GetMatrix_YUVA2RGBA();

	static const D3DXMATRIXA16* GetMatrix_RGBA2YUVA();
};


extern const D3DXMATRIXA16 g_matYUVA2RGBA_HD;
extern const D3DXMATRIXA16 g_matRGBA2YUVA_HD;
extern const D3DXMATRIXA16 g_matYUVA2RGBA_SD;
extern const D3DXMATRIXA16 g_matRGBA2YUVA_SD;

#define CS_UV_OFFSET		128.f / 255.f

#define cYUYVMode			D3DXVECTOR4(0.0f, CS_UV_OFFSET, 0.0f, CS_UV_OFFSET)
#define cYUVAMode			D3DXVECTOR4(0.0f, CS_UV_OFFSET, CS_UV_OFFSET, 0.0f)
#define CS_YUYV_OFFSET		cYUYVMode
#define CS_YUVA_OFFSET		cYUVAMode

