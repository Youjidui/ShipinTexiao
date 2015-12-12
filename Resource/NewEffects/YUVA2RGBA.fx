#include"..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTexture;

texture g_txColor;

sampler g_samColor = 
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VS_INPUT
{
	float3 vPos:POSITION;
	float2 vTexCoord:TEXCOORD0;
};

struct PS_INPUT
{
	float4 vPos:POSITION;
	float2 vTex:TEXCOORD0;
};

//Vs
PS_INPUT vs_main(VS_INPUT input)
{	
	PS_INPUT output  =  (PS_INPUT) 0;
	output.vPos = mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexture);
	if(input.vPos.z > 1.0f)
		output.vTex.x -= g_matTexture._41;
	else if(input.vPos.z > 0.0f)
		output.vTex.y -= g_matTexture._42;
	return output;
}

//Ps
void ps_rgba2yuva(float2 vTex:TEXCOORD0, out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samColor, vTex);
	oColor = CS_RGBA2YUVA(oColor);
}

void ps_yuva2rgba(float2 vTex:TEXCOORD0, out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samColor, vTex);
	oColor = CS_YUVA2RGBA(oColor);
}

technique RGBA2YUVA
{
	PASS zero //P0
	{
		VertexShader = compile vs_2_0 vs_main();
		PixelShader = compile  ps_2_0 ps_rgba2yuva();
		CullMode = None;			
		ZEnable  =  False;
	}
}
technique YUVA2RGBA
{
	PASS zero //P0
	{
		VertexShader = compile vs_2_0 vs_main();
		PixelShader = compile  ps_2_0 ps_yuva2rgba();
		CullMode = None;			
		ZEnable  =  False;
	}
}
