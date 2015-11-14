#include"..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTexture;

texture g_txColor;
texture g_txAlpha;

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

sampler g_samAlpha = 
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
	float3 vPos:POSITION;
	float2 vTexCoord:TEXCOORD0;
};

struct PS_INPUT
{
	float4 vPos:POSITION;
	float2 vTex:TEXCOORD0;
};


///////////////////////////Picture//////////////////////////////////
//Vs
PS_INPUT VS(VS_INPUT input)
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
void PS_ONE(PS_INPUT input,out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samColor,input.vTex);		
}
void PS_TWO(PS_INPUT input,out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{	
	oColor = tex2D(g_samColor,input.vTex);		
	oAlpha = tex2D(g_samAlpha,input.vTex);		
}

///////////////////////////Generate Mask ZBuffer//////////////////////////////////
//Vs
PS_INPUT VS_MASK(VS_INPUT input,uniform float fZ)
{	
	PS_INPUT output  =  (PS_INPUT) 0;
	output.vPos = mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexture);
	output.vPos.z = fZ * output.vPos.w;
	return output;
}

//Ps
void PS_MASK(PS_INPUT input,uniform bool bInvert,out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samColor,input.vTex);		
	if(bInvert)
	{
		if(any(oColor))
			discard;
	}
	else
	{
		if((!any(oColor.rgb)) && oColor.a > 0.0f)					
			discard;
	}	
}
///////////////////////////////Clear CG_Blend///////////////////////////////////////////////////////
PS_INPUT VS_CGBLEND(VS_INPUT input)
{	
	PS_INPUT output  =  (PS_INPUT) 0;
	output.vPos = mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexture);
	return output;
}

//Ps
void PS_CGBLEND(PS_INPUT input,uniform int nFmt, out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samColor,input.vTex);		
	if(nFmt == 0) //yuyv
	{		
		 oColor -= cYUYVMode;
		float4 vAlpha = tex2D(g_samAlpha,input.vTex);		
		if(vAlpha.r > 0.0f)
			oColor.r /= vAlpha.r;
		if(vAlpha.b > 0.0f)
		{
			oColor.gba /= vAlpha.b;
		}
		oColor += cYUYVMode;
	}
	else if(nFmt == 1) //yuva
	{
		 oColor -= cYUVAMode;
		
		if(oColor.a > 0.0f)
		{
			oColor.rgb /= oColor.a;
		}
		oColor += cYUVAMode;
	}
	else if(nFmt == 2) //rgba
		oColor.rgb /= oColor.a;
}

technique Picture
{
	PASS One //P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS_ONE();
		CullMode = None;			
		ZEnable  =  False;
	}	
	PASS Two //P1
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PS_TWO();
		CullMode = None;			
		ZEnable  =  False;
	}
}
technique Clear_CGBlend
{
	PASS YUYV //P0
	{
		VertexShader = compile vs_2_0 VS_CGBLEND();
		PixelShader = compile  ps_2_0 PS_CGBLEND(0);
		CullMode = None;			
		ZEnable  =  False;
	}
	PASS YUVA //P1
	{
		VertexShader = compile vs_2_0 VS_CGBLEND();
		PixelShader = compile  ps_2_0 PS_CGBLEND(1);
		CullMode = None;			
		ZEnable  =  False;
	}
	PASS RGBA //P2
	{
		VertexShader = compile vs_2_0 VS_CGBLEND();
		PixelShader = compile  ps_2_0 PS_CGBLEND(2);
		CullMode = None;			
		ZEnable  =  False;
	}
}
technique Generate_Mask_Zbuffer
{
	PASS Mask //P0
	{
		VertexShader = compile vs_2_0 VS_MASK(1.0f);
		PixelShader = compile  ps_2_0 PS_MASK(0);
		CullMode = None;			
		ZEnable  =  True;
		ZFunc = Always;
		ZWriteEnable = True;
		ColorWriteEnable = 0;
	}
	PASS Invert_Mask //P1
	{
		VertexShader = compile vs_2_0 VS_MASK(0.1f);
		PixelShader = compile  ps_2_0 PS_MASK(1);
		CullMode = None;			
		ZEnable  =  True;
		ZFunc = Always;
		ZWriteEnable = True;
		ColorWriteEnable = 0;
	} 	
}