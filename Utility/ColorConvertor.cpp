#include "StdAfx.h"
#include "ColorConvertor.h"
#include <assert.h>


///////////////////////////////////////////////////////////////////////////////
// YUVA 与 RGBA 之间的转换

// 高清定义（HD）
const D3DXMATRIXA16 g_matYUVA2RGBA_HD(
	 1.f,	    0.f,		    1.574722f,	    0.f ,
	 1.f,	    -0.187314f,	    -0.468207f, 	0.f ,
	 1.f,	    1.855615f,	    0.f,	    	0.f ,
	 0.f,	    0.f,	    	0.f,	    	1.f 
);

const D3DXMATRIXA16 g_matRGBA2YUVA_HD(
	 0.212639f,	0.715169f,  	0.072192f,	    0.f ,
	 -0.114592f,	-0.385409f,	    0.5f,		    0.f ,
	 0.5f,		    -0.454156f,	    -0.045844f,	    0.f ,
	 0.f,		    0.f,		    0.f,	    	1.f 
);

// 标清定义（SD）（索尼认为数字信号下 PAL 与 NTSC 相同）
const D3DXMATRIXA16 g_matYUVA2RGBA_SD(
	 1.f,  	0.f,		    1.401978f,	    0.f ,
	 1.f,  	-0.344116f,	    -0.714111f, 	0.f ,
	 1.f,  	1.771973f,  	0.f,		    0.f ,
	 0.f,  	0.f,		    0.f,		    1.f 
);

const D3DXMATRIXA16 g_matRGBA2YUVA_SD(
	 0.29895f, 	0.587036f,	    0.114014f,  	0.f ,
	 -0.168701f,	-0.331299f,     0.5f,		    0.f ,
	 0.5f,	    	-0.418701f,	    -0.081299f, 	0.f ,
	 0.f,	    	0.f,		    0.f,		    1.f 
);




//////////////////////////////////////////////////////////////////////////


void ColorConvertor::RGBA2YUVA(const D3DXCOLOR *rgba, D3DXCOLOR *yuva)
{	
	D3DXMATRIXA16 matConvert;
	D3DXMatrixTranspose(&matConvert,GetMatrix_RGBA2YUVA());
	D3DXVECTOR4 vRGBA(rgba->r,rgba->g,rgba->b,rgba->a);
	D3DXVec4Transform(&vRGBA,&vRGBA,&matConvert);
	vRGBA += D3DXVECTOR4(0.0f,128.0f /255.0f,128.0f /255.0f,0.0f);
	*yuva = D3DXCOLOR((float*)vRGBA);
}

void ColorConvertor::RGBA2YUYV(const D3DXCOLOR *rgba, D3DXCOLOR *yuyv)
{
	D3DXCOLOR yuva;
	RGBA2YUVA(rgba, &yuva);
	yuyv->r = yuva.r;
	yuyv->g = yuva.g;
	yuyv->b = yuva.r;
	yuyv->a = yuva.b;
}

void ColorConvertor::RGBA2UYVY(const D3DXCOLOR *rgba, D3DXCOLOR *uyvy)
{
	D3DXCOLOR yuva;
	RGBA2YUVA(rgba, &yuva);
	uyvy->r = yuva.g;
	uyvy->g = yuva.r;
	uyvy->b = yuva.b;
	uyvy->a = yuva.r;
} 



void ColorConvertor::YUVA2RGBA(const D3DXCOLOR *yuva, D3DXCOLOR *rgba)
{
	D3DXMATRIXA16 matConvert;
	D3DXMatrixTranspose(&matConvert,GetMatrix_YUVA2RGBA());
	D3DXVECTOR4 vYUVA(yuva->r,yuva->g - 128.0f / 255.0f,yuva->b - 128.0f / 255.0f,yuva->a);
	D3DXVec4Transform(&vYUVA,&vYUVA,&matConvert);	
	*rgba = D3DXCOLOR((float*)vYUVA);
}

void ColorConvertor::YUYV2RGBA(const D3DXCOLOR *yuyv, D3DXCOLOR *rgba)
{	
	const D3DXCOLOR vYUVA(yuyv->b,yuyv->g,yuyv->a,1.0f);
	YUVA2RGBA(&vYUVA,rgba);
}

void ColorConvertor::UYVY2RGBA(const D3DXCOLOR *uyvy, D3DXCOLOR *rgba)
{
	const D3DXCOLOR vYUVA(uyvy->g,uyvy->r,uyvy->b,1.0f);
	YUVA2RGBA(&vYUVA,rgba);
}

const D3DXMATRIXA16* ColorConvertor::GetMatrix_YUVA2RGBA() 
{
	if(0)
		return &g_matYUVA2RGBA_SD; 
	else
		return &g_matYUVA2RGBA_HD; 

}
const D3DXMATRIXA16* ColorConvertor::GetMatrix_RGBA2YUVA() 
{
	if(0)
		return &g_matRGBA2YUVA_SD; 
	else
		return &g_matRGBA2YUVA_HD; 
}


void ColorConvertor::RGBA2(const Buffer_Color_Format fmt,D3DXCOLOR *crSrc,D3DXCOLOR *crDest)
{
	switch(fmt)
	{
	case FMT_RGBA32:*crDest = *crSrc;break;
	case FMT_YUVA32:
		RGBA2YUVA(crSrc,  crDest);		
		break;
	case FMT_YUYV:
		RGBA2YUYV(crSrc,  crDest);		
		break;
	default:
		assert(0);
		break;
	}	
}

