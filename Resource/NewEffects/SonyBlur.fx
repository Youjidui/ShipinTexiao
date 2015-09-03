#include "..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTexture;
float2	 g_vAlphaBlend;

texture g_txColor;
texture g_txAlpha;

sampler g_samColor=
sampler_state
{	
	Texture=<g_txColor>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Clamp;
	AddressV=Clamp;
};

sampler g_samAlpha=
sampler_state
{	
	Texture=<g_txAlpha>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Clamp;
	AddressV=Clamp;
};

float2	g_pixelOffset;			// 在外部计算不同pass所需要的采样间隔
float4  g_texBound;			// left,right; top,bottom;
float3	g_coeff;

void VertScene(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	
	oPos=mul(float4(vPos,1.f),g_matWorldViewProj);
	oTex=mul(float4(vTex,1.0f,0.0f),g_matTexture);		
}	

// 取三个点做blur
float4 blurImage1RT(float2 left, float2 ori, float2 right)
{
	float4 colorLeft = tex2D(g_samColor,left);
	float4 colorOri = tex2D(g_samColor,ori);
	float4 colorRight = tex2D(g_samColor,right);
	
	if(g_vAlphaBlend.x > 0.5f)
	{
		colorLeft.rgb = lerp(cYUVAMode,colorLeft,colorLeft.a).rgb;
		colorOri.rgb = lerp(cYUVAMode,colorOri,colorOri.a).rgb;
		colorRight.rgb = lerp(cYUVAMode,colorRight,colorRight.a).rgb;
	}
	
	float4 oColor = (colorLeft*g_coeff.x + colorOri*g_coeff.y + colorRight*g_coeff.z);
	
	if(g_vAlphaBlend.y > 0.5f)
	{
		if(oColor.a  < 1.0f / 255.0f)
			oColor = cYUVAMode;
		else
			oColor.rgb = lerp(cYUVAMode,oColor,1.0f / oColor.a);
	}
	
	return oColor;
}

void blurImage2RT(float2 left, float2 ori, float2 right,
					out float4 color, out float4 alpha)
{
	float4 colorLeft = tex2D(g_samColor,left);
	float4 colorOri = tex2D(g_samColor,ori);
	float4 colorRight = tex2D(g_samColor,right);
	color = (colorLeft*g_coeff.x + colorOri*g_coeff.y + colorRight*g_coeff.z) + 0.f/255.f;
	
	float4 alphaLeft = tex2D(g_samAlpha,left);
	float4 alphaOri = tex2D(g_samAlpha,ori);
	float4 alphaRight = tex2D(g_samAlpha,right);
	alpha = (alphaLeft*g_coeff.x + alphaOri*g_coeff.y + alphaRight*g_coeff.z) + 0.f/255.f;
}


float4 PixSceneONERTX(float2 Tex:TEXCOORD0):COLOR0
{
	float3 coordX = float3(-g_pixelOffset.x,0.f,g_pixelOffset.x) + Tex.x;
	coordX = clamp(coordX,g_texBound.x,g_texBound.y);
	return blurImage1RT( float2(coordX.x,Tex.y), Tex, float2(coordX.z,Tex.y) );
}

float4 PixSceneONERTY(float2 Tex:TEXCOORD0):COLOR0
{
	float3 coordY = float3(-g_pixelOffset.y,0.f,g_pixelOffset.y) + Tex.y;
	coordY = clamp(coordY,g_texBound.z,g_texBound.w);
	return blurImage1RT( float2(Tex.x,coordY.x), Tex, float2(Tex.x,coordY.z) );
}

void  PixSceneTWORTX(float2 Tex:TEXCOORD0,out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{		
	float3 coordX = float3(-g_pixelOffset.x,0.f,g_pixelOffset.x) + Tex.x;
	coordX = clamp(coordX,g_texBound.x,g_texBound.y);
	blurImage2RT( float2(coordX.x,Tex.y), Tex, float2(coordX.z,Tex.y), oColor, oAlpha );
}

void  PixSceneTWORTY(float2 Tex:TEXCOORD0,out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{		
	float3 coordY = float3(-g_pixelOffset.y,0.f,g_pixelOffset.y) + Tex.y;
	coordY = clamp(coordY,g_texBound.z,g_texBound.w);
	blurImage2RT( float2(Tex.x,coordY.x), Tex, float2(Tex.x,coordY.z), oColor, oAlpha  );
}

technique RenderONERT
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneONERTX();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}
	PASS P1
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneONERTY();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}	
}

technique RenderTWORT
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneTWORTX();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}
	PASS P1
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneTWORTY();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}		
}

////////////////////////////////////////////////////////////////////////////////

float2	pixelSize;

float4 PixSceneDownScaleONERTX(float2 Tex:TEXCOORD0):COLOR0
{
	float4 colorOri		= tex2D(g_samColor,Tex);
	float4 colorRight	= tex2D(g_samColor,Tex+float2(pixelSize.x,0.f));
	return (colorOri + colorRight)/2.f;
}

float4 PixSceneDownScaleONERTY(float2 Tex:TEXCOORD0):COLOR0
{
	float4 colorOri		= tex2D(g_samColor,Tex);
	float4 colorDown	= tex2D(g_samColor,Tex+float2(0.f,pixelSize.y));
	return (colorOri + colorDown)/2.f;
}

void PixSceneDownScaleTWORTX(float2 Tex:TEXCOORD0,
							out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{
	float2 TexRight = Tex+float2(pixelSize.x,0.f);
	
	float4 colorOri		= tex2D(g_samColor,Tex);
	float4 colorRight	= tex2D(g_samColor,TexRight);
	oColor = (colorOri + colorRight)/2.f;
	
	float4 alphaOri		= tex2D(g_samAlpha,Tex);
	float4 alphaRight	= tex2D(g_samAlpha,TexRight);
	oAlpha = (alphaOri + alphaRight)/2.f;
}

void PixSceneDownScaleTWORTY(float2 Tex:TEXCOORD0,
							out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{
	float2 TexDown = Tex+float2(0.f,pixelSize.y);
	
	float4 colorOri		= tex2D(g_samColor,Tex);
	float4 colorDown	= tex2D(g_samColor,TexDown);
	oColor = (colorOri + colorDown)/2.f;
	
	float4 alphaOri		= tex2D(g_samAlpha,Tex);
	float4 alphaDown	= tex2D(g_samAlpha,TexDown);
	oAlpha = (alphaOri + alphaDown)/2.f;
}

float4 PixSceneUpScaleONERT(float2 Tex:TEXCOORD0):COLOR0
{
	Tex = clamp(Tex,g_texBound.xz,g_texBound.yw);
	return tex2D(g_samColor,Tex);
}

void PixSceneUpScaleTWORT(float2 Tex:TEXCOORD0,
						out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{
	Tex = clamp(Tex,g_texBound.xz,g_texBound.yw);
	oColor = tex2D(g_samColor,Tex);
	oAlpha = tex2D(g_samAlpha,Tex);
}


technique RenderDownScaleONERT
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneDownScaleONERTX();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}
	PASS P1
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneDownScaleONERTY();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}	
}

technique RenderDownScaleTWORT
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneDownScaleTWORTX();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}
	PASS P1
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneDownScaleTWORTY();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}		
}

technique RenderUpScaleONERT
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneUpScaleONERT();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}	
}

technique RenderUpScaleTWORT
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene();
		PixelShader=compile  ps_3_0 PixSceneUpScaleTWORT();
		CullMode=None;
		AlphaBlendEnable = False;
		//ZEnable = False;
	}

}