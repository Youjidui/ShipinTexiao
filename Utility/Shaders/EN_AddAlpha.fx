//
float4x4 g_matColorWorldViewProj;
float4x4 g_matColorTex;

float4x4 g_matKeyWorldViewProj;
float4x4 g_matKeyTex;

texture g_txColor;
texture g_txKey;

float4 g_vKeyMisc;
float4 g_vSrcMisc;

#include ".\ColorSpaceFunc.INC"

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

sampler g_samKey=
sampler_state
{	
	Texture = <g_txKey>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};


void VSColor(float4 vPos:POSITION,				
				float2 vTex:TEXCOORD0,												
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0
				)
{	
	oPos=mul(vPos,g_matColorWorldViewProj);				
	oTex.xy=mul(float3(vTex.xy,1.0f),g_matColorTex);	
	oPos.z = 0.9f * oPos.w;
}


void PSColor(float2 Tex:TEXCOORD0
			,out float4 oColor:COLOR0
			)
{	
	oColor = tex2D( g_samColor,Tex );    
	if(g_vSrcMisc.x > 0.0f)
	{
		oColor -= cYUVAMode;
		if(oColor.a > 0.0f)
			oColor.rgb /= oColor.a;
		oColor += cYUVAMode;
	}
	oColor.a *= g_vSrcMisc.y;
}

void VSKey(float4 vPos:POSITION,				
				float2 vTex:TEXCOORD0,												
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0
				)
{	
	oPos=mul(vPos,g_matKeyWorldViewProj);				
	oTex.xy=mul(float3(vTex.xy,1.0f),g_matKeyTex);	
}


void PSKey_YUVA(float2 Tex:TEXCOORD0
		,out float4 oColor:COLOR0
		,out float  oDepth:DEPTH )
{
	 oColor  = tex2D( g_samKey, Tex );  	
	
	if(g_vKeyMisc.w > 0.0f)
		oColor.rgb *= sign(oColor.a);
	else
		oColor.rgb *= oColor.a;
	
	oColor *= g_vKeyMisc.x;		
	
	oColor += cYUVAMode;
	
	if(any(g_vKeyMisc.z))
		oColor = 1.0f - oColor;	
	
	oColor =  oColor[int(g_vKeyMisc.y)];	
	
	if(oColor.a > 0.0f)
		oDepth = 0.5f;
	else
		discard;
}

void PSKey_RGBA(float2 Tex:TEXCOORD0
		,out float4 oColor:COLOR0
		,out float  oDepth:DEPTH )
{
	 oColor  = tex2D( g_samKey, Tex );     
	 oColor.rgb = ColorSpace_YUVToRGB(oColor.rgb - cYUVAMode.rgb);
	 
	if(g_vKeyMisc.w > 0.0f)
		oColor.rgb *= sign(oColor.a);
	else
		oColor.rgb *= oColor.a;
	
	oColor *= g_vKeyMisc.x;		
	
	if(any(g_vKeyMisc.z))
		oColor = 1.0f - oColor;
	
	oColor =  oColor[int(g_vKeyMisc.y)];	
		
	if(oColor.a > 0.0f)
		oDepth = 0.5f;
	else
		discard;
}
float4 PS_AddAlpha(float2 Tex:TEXCOORD0):COLOR0
{
	return  tex2D( g_samKey, Tex ).r;  	
}
technique AddAlpha
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VSKey();
		PixelShader = compile  ps_2_0 PS_AddAlpha();
		
		CullMode = None;		
		ColorWriteEnable = Alpha;	
	}
}
technique Render_YUVA
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VSKey();
		PixelShader = compile  ps_2_0 PSKey_YUVA();
		
		CullMode = None;
		ZEnable = True;
		ZFunc = Less;
		ZWriteEnable = True;
		ShadeMode = Flat;	
		ColorWriteEnable = Alpha;	
		
		ScissorTestEnable = True;
	}
	
	PASS P1
	{
		VertexShader = compile vs_3_0 VSColor();
		PixelShader = compile  ps_3_0 PSColor();
		
		CullMode = None;
		ZEnable = True;
		ZFunc = Greater;
		ZWriteEnable = False;
		
		ShadeMode = Flat;		
		
		ColorWriteEnable = Red|Green|Blue|Alpha;	
		AlphaBlendEnable = True;
		SrcBlend = One;
		DestBlend = Zero;			
	}
	
}
technique Render_RGBA
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VSKey();
		PixelShader = compile  ps_2_0 PSKey_RGBA();
		
		CullMode = None;
		ZEnable = True;
		ZFunc = Less;
		ZWriteEnable = True;
		ShadeMode = Flat;	
		ColorWriteEnable = Alpha;	
		
		ScissorTestEnable = True;
	}
	
	PASS P1
	{
		VertexShader = compile vs_3_0 VSColor();
		PixelShader = compile  ps_3_0 PSColor();
		
		CullMode = None;
		ZEnable = True;
		ZFunc = Greater;
		ZWriteEnable = False;
		
		ShadeMode = Flat;		
		
		ColorWriteEnable = Red|Green|Blue|Alpha;	
		AlphaBlendEnable = True;
		SrcBlend = One;
		DestBlend = Zero;			
	}
}
