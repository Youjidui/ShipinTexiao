#include"..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTexture;
float4x4 g_matMask;

texture g_txColor;
texture g_txMask;


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
sampler g_samMask = 
sampler_state
{	
	Texture = <g_txMask>;
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
	float2 vMask:TEXCOORD1;
};


///////////////////////////Picture//////////////////////////////////
//Vs
PS_INPUT VS(VS_INPUT input)
{	
	PS_INPUT output  =  (PS_INPUT) 0;
	output.vPos = mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexture);
	output.vMask = mul(float4(input.vTexCoord,1.0f,0.0f),g_matMask);
			
	return output;
}

//Ps
void PS(PS_INPUT input,uniform int nType,uniform bool bMask,out float4 oColor:COLOR0)
{	
	float4 srcColor = tex2D(g_samColor,input.vTex);
	oColor = srcColor;
	
	if(nType == 0) //rgba
	{
		oColor.rgb = 1.0f - oColor.rgb;
	}
	else if(nType == 1) //yuva
	{
		float3 cMode = CS_YUVA_OFFSET; 
		oColor.rgb -= cMode;
		oColor.rgb = float3(1.0f - oColor.r,-oColor.g,-oColor.b);
		oColor.rgb += cMode;
	}
	else if(nType == 2) //yuyv
	{
		float4 cMode = CS_YUYV_OFFSET; 
		oColor -= cMode;
		oColor = float4(1.0f - oColor.r,-oColor.g,1.0f - oColor.b,-oColor.a);
		oColor += cMode;
	}
	if(bMask)
	{
		float4 vMask = tex2D( g_samMask,input.vMask );
		
		oColor = lerp(srcColor,oColor,vMask.a);
	}		
}


technique Picture
{
	PASS RGBA //P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS(0,0);
		CullMode = None;			
		//ZEnable  =  False;
	}	
	PASS YUVA //P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS(1,0);
		CullMode = None;			
		//ZEnable  =  False;
	}
	PASS YUYV //P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS(2,0);
		CullMode = None;			
		//ZEnable  =  False;
	}
	//mask
		PASS RGBA //P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS(0,1);
		CullMode = None;			
		//ZEnable  =  False;
	}	
	PASS YUVA //P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS(1,2);
		CullMode = None;			
		//ZEnable  =  False;
	}
	PASS YUYV //P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS(2,3);
		CullMode = None;			
		//ZEnable  =  False;
	}
}