#pragma once

#include <d3dx9math.h>

class ColorConvertor
{
public:
	ColorConvertor(void);
	~ColorConvertor(void);

	void ColorConv_RGBA_YUVA(const D3DXCOLOR *rgba, D3DXCOLOR *yuva);
	void ColorConv_YUVA_RGBA(const D3DXCOLOR *yuva, D3DXCOLOR *rgba);

	void ColorConv_RGBA_YUYV(const D3DXCOLOR *rgba, D3DXCOLOR *yuyv);
	void ColorConv_RGBA_UYVY(const D3DXCOLOR *rgba, D3DXCOLOR *uyvy);

	void ColorConv_YUYV_RGBA(const D3DXCOLOR *yuyv, D3DXCOLOR *rgba);
	void ColorConv_UYVY_RGBA(const D3DXCOLOR *uyvy, D3DXCOLOR *rgba);

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

