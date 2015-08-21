#include ".\ColorSpaceFunc.INC"

float4x4 g_matWVP;

float4x4 g_matTexLeft;
float4x4 g_matTexRight;

texture g_txColorLeft;
sampler g_samColorLeft =
sampler_state
{	
	Texture = <g_txColorLeft>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

texture g_txColorRight;
sampler g_samColorRight =
sampler_state
{
	Texture = <g_txColorRight>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

void VS(float3 vPos:POSITION,				
		float2 vTex:TEXCOORD0,				
		out float4 oPos:POSITION,				
		out float4 oTex:TEXCOORD0)
{	
	oPos = mul(float4(vPos,1.f),g_matWVP);
	oTex.xy = mul(float3(vTex,1.0f),g_matTexLeft);
	oTex.zw = mul(float3(vTex,1.0f),g_matTexRight);
}	

float4 PS_LeftRedRightBlue(float4 inTex:TEXCOORD0):COLOR0
{
	float4 fColor = (float4)0;	
	
	float2 TexLeft = inTex.xy;
	float2 TexRight = inTex.zw;
		
	float4 fColorLeft = tex2D(g_samColorLeft, TexLeft);
	fColorLeft.xyz -= CS_YUVA_OFFSET;
	fColorLeft.xyz = ColorSpace_YUVToRGB(fColorLeft.xyz );
	
	float4 fColorRight = tex2D(g_samColorRight, TexRight);
	fColorRight.xyz -= CS_YUVA_OFFSET;
	fColorRight.xyz = ColorSpace_YUVToRGB(fColorRight.xyz );
	
	//红蓝
	fColor.r = fColorLeft.x;	// 左纹理亮度赋值R通道
	fColor.g = fColorRight.y;
	fColor.b = fColorRight.z;	// 右纹理亮度赋值B通道	
	fColor.a = (fColorLeft.a + fColorRight.a) / 2.0f;

	// 将颜色从RGB格式转换为YUVA
	fColor = float4(CS_RGB2YUV(fColor.xyz), fColor.w);
	fColor += CS_YUVA_OFFSET;

	return fColor;
}

technique LeftRedRightBlue
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_LeftRedRightBlue();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
}


float4 PS_LeftGreenRightPink(float4 inTex:TEXCOORD0):COLOR0
{
	float4 fColor = (float4)0;	
	
	float2 TexLeft = inTex.xy;
	float2 TexRight = inTex.zw;
		
	float4 fColorLeft = tex2D(g_samColorLeft, TexLeft);
	fColorLeft.xyz -= CS_YUVA_OFFSET;
	fColorLeft.xyz = ColorSpace_YUVToRGB(fColorLeft.xyz );
	
	float4 fColorRight = tex2D(g_samColorRight, TexRight);
	fColorRight.xyz -= CS_YUVA_OFFSET;
	fColorRight.xyz = ColorSpace_YUVToRGB(fColorRight.xyz );	
	
	//绿红
	fColor.r = fColorRight.x;
	fColor.g = fColorLeft.y;
	fColor.b = fColorRight.z;	
	fColor.a = (fColorLeft.a + fColorRight.a) / 2.0f;

	// 将颜色从RGB格式转换为YUVA
	fColor = float4(CS_RGB2YUV(fColor.xyz), fColor.w);
	fColor += CS_YUVA_OFFSET;

	return fColor;
}

technique LeftGreenRightPink
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_LeftGreenRightPink();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
}

float4 PS_LeftBrownRightPurple(float4 inTex:TEXCOORD0):COLOR0
{
	float4 fColor = (float4)0;	
	
	float2 TexLeft = inTex.xy;
	float2 TexRight = inTex.zw;
		
	float4 fColorLeft = tex2D(g_samColorLeft, TexLeft);
	fColorLeft.xyz -= CS_YUVA_OFFSET;
	fColorLeft.xyz = ColorSpace_YUVToRGB(fColorLeft.xyz );
	
	float4 fColorRight = tex2D(g_samColorRight, TexRight);
	fColorRight.xyz -= CS_YUVA_OFFSET;
	fColorRight.xyz = ColorSpace_YUVToRGB(fColorRight.xyz );	
	
	//棕紫
	fColor.r = fColorLeft.x;
	fColor.g = fColorLeft.y;
	fColor.b = fColorRight.z;	
	fColor.a = (fColorLeft.a + fColorRight.a) / 2.0f;

	// 将颜色从RGB格式转换为YUVA
	fColor = float4(CS_RGB2YUV(fColor.xyz), fColor.w);
	fColor += CS_YUVA_OFFSET;

	return fColor;
}

technique LeftBrownRightPurple
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_LeftBrownRightPurple();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
}

//Video Comparison
float4x4 g_matComparisonWVP;
float4x4 g_matComparisonTex;

float	 g_fPosition;

texture g_txSrc1;
texture g_txSrc2;

sampler g_samSrc1=
sampler_state
{	
	Texture=<g_txSrc1>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};

sampler g_samSrc2=
sampler_state
{	
	Texture=<g_txSrc2>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};

void VS_COMPARISON(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	
	oPos=mul(float4(vPos,1.f),g_matComparisonWVP);
	oTex=mul(float4(vTex,1.0f,0.0f),g_matComparisonTex).xy;	
}


float4  PS_COMPARISON(float4 Pos:POSITION, float2 Tex:TEXCOORD0, uniform bool bLeftAndRight):COLOR0
{
	float fTex = bLeftAndRight ? Tex.x : Tex.y;		
	if(fTex < g_fPosition)
		return tex2D(g_samSrc1,Tex);
	else
		return tex2D(g_samSrc2,Tex);
}

technique VideoComparison
{
	PASS LeftAndRight //P0  左右显示
	{
		VertexShader=compile vs_3_0 VS_COMPARISON();
		PixelShader=compile  ps_3_0 PS_COMPARISON(true);
		CullMode = None;			
		ZEnable  =  False;
	}
	PASS UpAndDown   //P1  上下显示
	{
		VertexShader=compile vs_3_0 VS_COMPARISON();
		PixelShader=compile  ps_3_0 PS_COMPARISON(false);
		CullMode = None;			
		ZEnable  =  False;
	}
}