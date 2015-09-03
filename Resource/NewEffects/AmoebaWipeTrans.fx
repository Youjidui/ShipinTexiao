#include "..\..\..\..\TPEngine\src\Common\TPRenderX\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTex;

texture g_txColor;

sampler g_samNoise =
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
	AddressV = Wrap;
};
sampler g_samColor =
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};
texture g_txColor1;
sampler g_samColor1 =
sampler_state
{	
	Texture = <g_txColor1>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};
texture g_txKey;

sampler g_samKey =
sampler_state
{	
	Texture = <g_txKey>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};
texture g_txLight;

sampler g_samLight =
sampler_state
{	
	Texture = <g_txLight>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};


float4		g_vMisc;
float4		g_vAlpha;

float4      g_vLightDir;
float4      g_textureInfo;
#define     g_embossFactor g_textureInfo.z

float4      g_HighLight;
float4      g_Shadow;

void VS(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	
	oPos = mul(float4(vPos,1.f),g_matWorldViewProj);
	oTex = mul(float3(vTex,1.0f),g_matTex);		
}	


float4 PS(float2 Tex:TEXCOORD0):COLOR0
{
	float4 oColor = (float4)0;	
		
	oColor = tex2D(g_samNoise,Tex);	
		
	return oColor;
}

float4 Emboss(float fKey,float4 vLuDir)
{	
	float d = dot( vLuDir, g_vLightDir );		
		
	float fCo = smoothstep(g_vMisc.z,g_vMisc.w,fKey);	
	if(fCo < 1.0f)
		fCo *= abs(d);
	
	
	float4 retColor = (float4)0;	
	
	if(d > 0)
	   retColor = float4(g_HighLight.rgb,fCo);
	else		
		retColor = float4(g_Shadow.rgb,fCo);
		
	return retColor;
}
float4 PS_Light(float2 Tex:TEXCOORD0):COLOR0
{
	float4 fGray = tex2D(g_samKey,Tex);
	
	float4 L = fGray - tex2D(g_samKey,Tex + float2(-1.0f / g_textureInfo.x,0.0f));
	float4 R = fGray - tex2D(g_samKey,Tex + float2(1.0f / g_textureInfo.x,0.0f));
	float4 U = fGray - tex2D(g_samKey,Tex + float2(0.0f, 1.0f / g_textureInfo.y));
	float4 D = fGray - tex2D(g_samKey,Tex + float2(0.0f, -1.0f / g_textureInfo.y));
	
	return Emboss(fGray.r,float4(U.r,D.r,L.r,R.r));	
}

float4 PS_Amoeba(float2 Tex:TEXCOORD0):COLOR0
{	
	float4 srcColor1 = tex2D(g_samColor1,Tex);
	float4 srcColor = tex2D(g_samColor,Tex);
	
	srcColor.a  *= g_vAlpha.x;
	srcColor1.a *= g_vAlpha.y;
	
	float4 vLight = tex2D(g_samLight,Tex);	
		 
	float fAlpha = 1.0f - vLight.a;
	vLight.a = 1.0f;
	float4 retColor = lerp(srcColor,vLight,saturate(fAlpha * g_embossFactor));				
	
	float4 vKey = tex2D(g_samKey,Tex);	
	
	float fKey = smoothstep(g_vMisc.x,g_vMisc.y,vKey.r);
		
	float4 oColor = lerp(srcColor1,retColor,fKey);	
		
	return oColor;
}

technique Amoeba
{
	PASS Resize_Noise//P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS();
		CullMode = None;
		ZEnable = False;		
	}
	//
	PASS Light//P1
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_Light();
		CullMode = None;
		ZEnable = False;		
	}	
	PASS Out//P2
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_Amoeba();
		CullMode = None;
		ZEnable = False;	
		
		AlphaBlendEnable = False;		
	}
}

