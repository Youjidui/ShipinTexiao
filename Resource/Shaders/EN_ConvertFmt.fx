#define _FX
#include ".\ColorSpaceFunc.INC"

//#define FMT_10BIT

bool g_bChangeRB;				//10bit上行和下行需要交换r和b通道
float4x4 g_matWorldViewProj;
float4x4 g_matTex;
float4x4 g_matAlpha;
float4x4 g_mat_HD_SD;

float4	g_vMisc;
#define g_vSizeTex			g_vMisc.xy
#define g_fAlpha			g_vMisc.z
#define g_bCGBlend			(g_vMisc.w > 0.5f)

texture g_txColor;
texture g_txAlpha;

sampler g_samColor=
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samAlpha=
sampler_state
{	
	Texture = <g_txAlpha>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
struct VS_INPUT
{
	float4 vPos:POSITION;				
	float2 vTex:TEXCOORD0;	
};
struct PS_INPUT
{
	float4 vPos:POSITION;				
	float4 vTex:TEXCOORD0;	
};

PS_INPUT VS(VS_INPUT In,uniform bool bKey)
{
	PS_INPUT Out = (PS_INPUT)0;
	Out.vPos = mul(float4(In.vPos.xy,0.0f,1.0f),g_matWorldViewProj);	
	Out.vTex.xy = mul(float3(In.vTex,1.0f),g_matTex);	
	
	if(bKey)
		Out.vTex.zw = mul(float3(In.vTex,1.0f),g_matAlpha);	
	else
		Out.vTex.zw = 0;
	
	if(In.vPos.z > 1.0f)
		Out.vTex.x = g_matTex._41;
	else if(In.vPos.z > 0.0f)
		Out.vTex.y = g_matTex._42;			
	
	return Out;
}
//ps
//------------------------------YUYV---YUVA----------------------------------------------//
 void PS_YUYV2YUVA(PS_INPUT In,
				uniform bool bAverage,
				uniform bool bKey,
				out float4 oColor:COLOR0
		)
 {	
	float4 sColor = tex2D(g_samColor,In.vTex);
#ifdef FMT_10BIT
	sColor.rb = sColor.br;
#endif
 	if(g_bChangeRB)
 		sColor.rb = sColor.br;
	
	float sAlpha = 1.0f;
	//带Key 上行，KEY连续存放，一个像素ALPHA占一个字节，一个DWORD放四个像素ALPHA
	if(bKey)
	{
		float4 vAlpha = tex2D(g_samAlpha,In.vTex.zw);		
		int nItem = floor(frac(In.vTex.z * g_vMisc.z) * 4.0f);
		vAlpha = vAlpha.bgra;
		sAlpha = vAlpha[nItem];
	}
	
	float x = frac(In.vTex.x * g_vSizeTex.x + 0.1f - g_vSizeTex.y);
	if( x > 0.3f )	
	{
		if(bAverage)
		{
			float2 vTex = In.vTex + float2(1.0f / g_vSizeTex.x,0.0f);
			float4 sColor1 = tex2D(g_samColor,vTex);
				
			oColor = float4(sColor.r,lerp(sColor.ga,sColor1.ga,0.5f),sAlpha); 	
		}
		else
			oColor = float4(sColor.rga,sAlpha); 	
	}
	else	    
		oColor = float4(sColor.bga,sAlpha);  
	
	oColor = float4(mul(float4(oColor.rgb,1.0f),g_mat_HD_SD).xyz,oColor.a); 	
 }
 void PS_YUVA2YUYV_AP(PS_INPUT In,				
		out float4 oColor:COLOR0,
		out float4 oAlpha:COLOR1
		)
{
	float4 sColor0 = tex2D(g_samColor,In.vTex);
	float4 sColor1 = tex2D(g_samColor,In.vTex + g_vSizeTex);  
	
	sColor0 = float4(mul(float4(sColor0.rgb,1.0f),g_mat_HD_SD).xyz,sColor0.a);
	sColor1 = float4(mul(float4(sColor1.rgb,1.0f),g_mat_HD_SD).xyz,sColor1.a);
	
	if(g_bCGBlend > 0.0f && g_fAlpha > 0.0f)
	{
		if(sColor0.a > 0.0f)
		{		
			sColor0.rgb = lerp(cYUVAMode,sColor0,1.0f / sColor0.a);
		}
		if(sColor1.a > 0.0f)
		{
			sColor1.rgb = lerp(cYUVAMode,sColor1,1.0f / sColor1.a);
		}
	}
	
	sColor0.a *= g_fAlpha;
	sColor1.a *= g_fAlpha;
	oColor = float4(sColor1.x,sColor0.y,sColor0.x,sColor0.z); 
	oAlpha = float4(sColor1.a,sColor1.a,sColor0.a,sColor0.a);
	
#ifdef FMT_10BIT
 	oColor.rb = oColor.br;
#endif
 	if(g_bChangeRB)
 		oColor.rb = oColor.br;
}
 void PS_YUVA2YUYV_NA(PS_INPUT In,	
		uniform bool bColor,
		uniform bool bDiscardAlpha,			
		out float4 oColor:COLOR0		
		)
{
	float4 sColor0 = tex2D(g_samColor,In.vTex);
	float4 sColor1 = tex2D(g_samColor,In.vTex + g_vSizeTex);  
	
	sColor0 = float4(mul(float4(sColor0.rgb,1.0f),g_mat_HD_SD).xyz,sColor0.a);
	sColor1 = float4(mul(float4(sColor1.rgb,1.0f),g_mat_HD_SD).xyz,sColor1.a);
		
	if(bColor)
	{	
		if(bDiscardAlpha)
		{
			float4 crBlack = CS_YUVA_OFFSET;
			crBlack.r = 16.0f / 255.0f;
			if(g_bCGBlend > 0.0f)
			{
				sColor0.rgb = lerp(crBlack,sColor0.rgb,g_fAlpha);
				sColor1.rgb = lerp(crBlack,sColor1.rgb,g_fAlpha);				
				
			}
			else
			{
				sColor0.rgb = lerp(crBlack,sColor0.rgb,sColor0.a * g_fAlpha);
				sColor1.rgb = lerp(crBlack,sColor1.rgb,sColor1.a * g_fAlpha);
			}
		}
		else
		{
			if(g_bCGBlend > 0.0f)
			{
				if(sColor0.a > 0.0f)
				{					
					sColor0.rgb = lerp(cYUVAMode,sColor0,1.0f / sColor0.a);
				}
				if(sColor1.a > 0.0f)
				{
					sColor1.rgb = lerp(cYUVAMode,sColor1,1.0f / sColor1.a);
				}
			}
		}	
		
		oColor = float4(sColor1.x,sColor0.y,sColor0.x,sColor0.z); 	
	}
	else
	{
		sColor0.a *= g_fAlpha;
		sColor1.a *= g_fAlpha;
		oColor = float4(sColor1.a,sColor1.a,sColor0.a,sColor0.a);
	}
#ifdef FMT_10BIT
	oColor.rb = oColor.br;
#endif		
 	if(g_bChangeRB)
		oColor.rb = oColor.br;
}
//Key One Pixel 2 byte 2 One Pixel 1 byte
 void PS_KEY_2_1 (PS_INPUT In,out float4 oAlpha:COLOR0)
{
	float4 sAlpha0 = tex2D(g_samColor,In.vTex);
	float4 sAlpha1 = tex2D(g_samColor,In.vTex + g_vSizeTex); 

	oAlpha = float4(sAlpha0.z,sAlpha0.x,sAlpha1.z,sAlpha1.x);
	oAlpha.rb = oAlpha.br;	
}
//------------------------------YUVA-TO--RGBA----------------------------------------------//
float4 PS_YUVA2RGBA(PS_INPUT In ,uniform bool bNeedBlend): COLOR
{   
	float4 oColor = tex2D(g_samColor,In.vTex); 
	     
	oColor.xyz -= CS_YUVA_OFFSET;
		
	if(bNeedBlend)
	{		
		if(g_bCGBlend && oColor.a > 0.0f )
			oColor.xyz /= oColor.a;
		
		oColor.a *= g_fAlpha;
	}
	else
	{		
		float a =  oColor.a;		
		if(g_bCGBlend)
			a = sign(oColor.a);		
		
		a *= g_fAlpha;
		
		oColor.rgb = lerp(float3(16.0f / 255.0f ,0.0f,0.0f),oColor.rgb,a);
		oColor.a = 1.0f;	
		
		//oColor.r = LUM_LIMIT_16_235(oColor.r);	
	}	 
	 
  oColor.xyz = ColorSpace_YUVToRGB(oColor.xyz ); 
	 
  return oColor;
}
//------------------------------RGBA-TO--YUVA----------------------------------------------//
float4 PS_RGBA2YUVA(PS_INPUT In): COLOR
{   
	float4 oColor = tex2D(g_samColor,In.vTex); 		

	if(g_bChangeRB)
		oColor.xyz = ColorSpace_RGBToYUV(oColor.zyx ); 
	else
		oColor.xyz = ColorSpace_RGBToYUV(oColor.xyz ); 
		
	oColor += CS_YUVA_OFFSET;
  	
	return oColor;
}
//------------------------------YUVA-TO--YUVA----------------------------------------------//
float4 PS_YUVA2YUVA(PS_INPUT In): COLOR
{   
	float4 oColor = tex2D(g_samColor,In.vTex).bgra; 		 
	oColor = float4(mul(float4(oColor.rgb,1.0f),g_mat_HD_SD).xyz,oColor.a); 	
	return oColor;
}
//------------------------------technique----------------------------------------------//
//technique
technique YUYV_YUVA
{
	//yuyv 2 yuva
 	PASS YUYV2YUVA_NA//P0
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUYV2YUVA(0,0);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	PASS YUYV2YUVA_AP//P1
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUYV2YUVA(1,0);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	//yuva 2 yuyv
 	PASS YUVA2YUYV_AP//P2
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUVA2YUYV_AP();
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	PASS YUVA2YUYV_NA_ALPHA//P3
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUVA2YUYV_NA(0,1);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	} 
 	
 	PASS YUVA2YUYV_NA_COLOR_DiscardAlpha//P4
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUVA2YUYV_NA(1,1);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	PASS YUVA2YUYV_NA_COLOR_KeepAlpha//P5
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUVA2YUYV_NA(1,0);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	} 
 	//yuyv 2 yuva
 	PASS YUYV2YUVA_NA//P6
 	{
 		VertexShader = compile vs_3_0 VS(1);
 		PixelShader = compile  ps_3_0 PS_YUYV2YUVA(0,1);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	PASS YUYV2YUVA_AP//P7
 	{
 		VertexShader = compile vs_3_0 VS(1);
 		PixelShader = compile  ps_3_0 PS_YUYV2YUVA(1,1);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}	
 	PASS KEY_2_1//P8
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_KEY_2_1();
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}		
}

technique YUVA2RGBA
{
	// no blend and Discard Alpha
 	PASS YUVA2RGBA//P0
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUVA2RGBA(0);
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	// need blend and Discard Alpha
 	PASS YUVA2RGBA_NeedBlend//P1
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUVA2RGBA(1);
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	// need blend and Keep Alpha
 	PASS YUVA2RGBA_NeedBlend_Keep_Alpha //P2
 	{
 		VertexShader = compile vs_3_0 VS(0);
 		PixelShader = compile  ps_3_0 PS_YUVA2RGBA(1);
 		
 		AlphaBlendEnable = False; 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
}
technique RGBA2YUVA
{	
 	PASS RGBA2YUVA//P0
 	{
 		VertexShader = compile vs_2_0 VS(0);
 		PixelShader = compile  ps_2_0 PS_RGBA2YUVA();
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
 	PASS YUVA2YUVA//P1
 	{
 		VertexShader = compile vs_2_0 VS(0);
 		PixelShader = compile  ps_2_0 PS_YUVA2YUVA();
 		
 		AlphaBlendEnable = False;
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat; 
 	}
}