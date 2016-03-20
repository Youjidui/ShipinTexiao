#include "..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTexture;
float4x4 g_matMask;

float4	 g_vMisc;
float4   g_vBorderColor;

texture g_txColor;
texture g_txAlpha;
texture g_txMask;

sampler g_samColor=
sampler_state
{	
	Texture=<g_txColor>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};

sampler g_samAlpha=
sampler_state
{	
	Texture=<g_txAlpha>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};
sampler g_samMask=
sampler_state
{	
	Texture=<g_txMask>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
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
PS_INPUT VS_COLOR(VS_INPUT input)
{	
	PS_INPUT output = (PS_INPUT) 0;
	output.vPos=mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vTex=mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexture);		
	output.vMask=mul(float4(input.vTexCoord,1.0f,0.0f),g_matMask);			
	return output;
}

//
void PS_YUYV_AP(PS_INPUT input,
				out float4 oColor:COLOR0,
				out float4 oAlpha:COLOR1)
{
	float4 vMask = tex2D(g_samMask,input.vMask);
	float4 vAlpha = tex2D(g_samAlpha,input.vTex);
	float4 vColor = tex2D(g_samColor,input.vTex);
	
	if(any(g_vMisc.w) && (any(vMask.rgb) == 0 || !any(g_vMisc.y)))
		vMask.a = 1.0f- vMask.a;
		
	oColor = CS_YUYV_OFFSET;
	oAlpha = vAlpha;
	oAlpha *= vMask.a;	
	if(any(oAlpha)) // vMask.r Border / vMask.g Soft Inside / vMask.b Soft Outside 
	{
		oColor = vColor;
		if(g_vMisc.y > 0.0f)
		{
			float4 vBorderColor = float4(g_vBorderColor.r,g_vBorderColor.g,g_vBorderColor.r,g_vBorderColor.b);
			if(any(vMask.rgb))
				oColor = vBorderColor;
			if(any(g_vMisc.w)  && any(vMask.g) )
			{
				oColor = lerp(vColor,vBorderColor,vMask.a);
				//if(!any(vMask.b))
					oAlpha = any(vMask.r)? vMask.r: vAlpha;
			}
			else if(any(g_vMisc.w) == 0 && any(vMask.b) )
			{
				oColor = lerp(vColor,vBorderColor,vMask.a);
				//if(!any(vMask.g))
					oAlpha = any(vMask.r)? vMask.r: vAlpha;
			} 
			else if(any(vMask.r))
			{
				oColor = lerp(vColor,vBorderColor,vMask.r);
			}	
		}		
	}	
}
void PS_YUYV_NA(PS_INPUT input,
				out float4 oColor:COLOR0,
				out float4 oAlpha:COLOR1)
{
	float4 vMask = tex2D(g_samMask,input.vMask);	
	float4 vColor = tex2D(g_samColor,input.vTex);
	if(any(g_vMisc.w) && (any(vMask.rgb) == 0 || !any(g_vMisc.y)))
		vMask.a = 1.0f- vMask.a;
	oColor = CS_YUYV_OFFSET;
	oAlpha = vMask.a;	
	if(any(oAlpha))
	{
		oColor = vColor;
		if(g_vMisc.y > 0.0f)
		{
			float4 vBorderColor = float4(g_vBorderColor.r,g_vBorderColor.g,g_vBorderColor.r,g_vBorderColor.b);
			if(any(vMask.rgb))
				oColor = vBorderColor;
			if(any(g_vMisc.w)  && any(vMask.g) )
			{
				oColor = lerp(vColor,vBorderColor,vMask.a);
				oAlpha = any(vMask.r)? vMask.r: 1.0f;
			}
			else if(any(g_vMisc.w) == 0 && any(vMask.b) )
			{
				oColor = lerp(vColor,vBorderColor,vMask.a);
				oAlpha = any(vMask.r)? vMask.r: 1.0f;
			} 	
			else if(any(vMask.r))
			{
				oColor = lerp(vColor,vBorderColor,vMask.r);
			}		
		}		
	}	
}
void PS_YUVA(PS_INPUT input,
			 uniform float4 vMode,
			 out float4 oColor:COLOR0)
{
	float4 vMask = tex2D(g_samMask,input.vMask);	
	float4 vColor = tex2D(g_samColor,input.vTex);
	
	if(any(g_vMisc.w) && (any(vMask.rgb) == 0 || !any(g_vMisc.y)))
		vMask.a = 1.0f- vMask.a;
		
	oColor = vMode;
	oColor.a = vMask.a * vColor.a;	
	if(any(oColor.a))
	{
		oColor.rgb = vColor.rgb;
		if(g_vMisc.y > 0.0f)
		{
			float4 vBorderColor = g_vBorderColor;
			if(any(vMask.rgb))
				oColor.rgb = vBorderColor.rgb;
			if(any(g_vMisc.w)  && any(vMask.g) )
			{
				oColor = lerp(vColor,vBorderColor,vMask.a);
				oColor.a = any(vMask.r)? vMask.r: vColor.a;
			}
			else if(any(g_vMisc.w) == 0 && any(vMask.b) )
			{
				oColor = lerp(vColor,vBorderColor,vMask.a);
				oColor.a = any(vMask.r)? vMask.r: vColor.a;
			} 
			else if(any(vMask.r))
			{
				oColor = lerp(vColor,vBorderColor,vMask.r);
			}	
		}		
	}	
}


technique YUYV_AP
{
	PASS P0
	{
		VertexShader=compile vs_2_0 VS_COLOR();
		PixelShader=compile  ps_2_0 PS_YUYV_AP();
		CullMode=None;			
		ZEnable = False;
	}
}
technique YUYV_NA
{
	PASS P0
	{
		VertexShader=compile vs_2_0 VS_COLOR();
		PixelShader=compile  ps_2_0 PS_YUYV_NA();
		CullMode=None;			
		ZEnable = False;
	}
}
technique YUVA
{
	PASS P0
	{
		VertexShader=compile vs_2_0 VS_COLOR();
		PixelShader=compile  ps_2_0 PS_YUVA(CS_YUVA_OFFSET);
		CullMode=None;			
		ZEnable = False;
	}
}
technique RGBA
{
	PASS P0
	{
		VertexShader=compile vs_2_0 VS_COLOR();
		PixelShader=compile  ps_2_0 PS_YUVA(float4(0.0f,0.0f,0.0f,0.0f));
		CullMode=None;			
		ZEnable = False;
	}
}