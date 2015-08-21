//
float4x4 g_matWorldViewProj;
float4x4 g_matTexture;

texture g_txColor0;
texture g_txColor1;

float4  g_vAlphaValue;
float4  g_vBlendValues;
#define bCGBlended g_vBlendValues.z

#define _FX
#include "..\shaders\ColorSpaceFunc.INC"

sampler g_samColor0=
sampler_state
{	
	Texture = <g_txColor0>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Border;
	AddressV = Border;
	BorderColor =  0x00008080;
};
sampler g_samColor1=
sampler_state
{	
	Texture = <g_txColor1>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Border;
	AddressV = Border;
	BorderColor =  0x00008080;
};


const float4 cMode = CS_YUVA_OFFSET;  

void VS(float4 vPos:POSITION,				
				float2 vTex:TEXCOORD0,												
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0				
				)
{	
	oPos = mul(vPos,g_matWorldViewProj);				
	oTex = mul(float3(vTex.xy,1.0f),g_matTexture);		
}
void SourceColor_YUVA(float2 vTex,out float4 sColor0,out float4 sColor1,out float sAlpha0,out float sAlpha1)
{		
	sColor0 = tex2D(g_samColor0,vTex) - cMode;
   sAlpha0 = g_vAlphaValue[0] * sColor0.a;    
   sColor0.rgb *= sAlpha0*g_vBlendValues[0];
        
   sColor1 = tex2D(g_samColor1,vTex) - cMode;
   sAlpha1 = g_vAlphaValue[1] * sColor1.a;  
   sColor1.rgb *= sAlpha1 * g_vBlendValues[1];       
}
void SourceColor_RGBA(float2 vTex,out float3 rgba0,out float3 rgba1,out float sAlpha0,out float sAlpha1)
{
	float4 sColor0 = 0.0f,sColor1 = 0.0f;
   SourceColor_YUVA(vTex,sColor0,sColor1,sAlpha0,sAlpha1);
   	   
   rgba1 = ColorSpace_YUVToRGB(sColor0.rgb);
   rgba0 = ColorSpace_YUVToRGB(sColor1.rgb);  
}
void DestColor(float sAlpha0,float sAlpha1,inout float4 oColor)
{
	oColor.a   = sAlpha0 + sAlpha1 * (1.0f - sAlpha0);   
     
   oColor.rgb = ColorSpace_RGBToYUV(saturate(oColor.rgb));
   
   oColor += cMode;   
}
void PSMul(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
   //
  oColor.rgb  = multiplyKernel(rgba0.rgb, rgba1.rgb);  
  //  
  DestColor(sAlpha0,sAlpha1,oColor);
}
void PSAdd(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{      
   float4 sColor0 = 0.0f,sColor1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_YUVA(vTex,sColor0,sColor1,sAlpha0,sAlpha1);   
   
   oColor.rgb    = sColor0  + sColor1 + cMode;        
   oColor.a  =  sAlpha0 + sAlpha1*(1.0f - sAlpha0);     
}
void PSAlpha(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{	
		
   float4 sColor0 = tex2D(g_samColor0,vTex)-cMode;
   float sAlpha0 = g_vAlphaValue[0] * sColor0.a;
   sColor0.rgb  *= sAlpha0*g_vBlendValues[0];  
      
   float4 sColor1 = tex2D(g_samColor1,vTex)-cMode;
   float sAlpha1 = sColor1.a;
   sColor1       *= sign(sAlpha1)*g_vBlendValues[1];  
   float4 t         = bCGBlended?sColor1*sign(sAlpha1):sColor1*sAlpha1;
   //自身 Alpha 考虑 CGBlend,全局 Alpha 不考虑 CGBlend,否则效果不对
   sAlpha1*=g_vAlphaValue[1];
   oColor.a  =  sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;           
   
   oColor.rgb=any(oColor.a)?sColor0*(1.0-g_vAlphaValue[1]*sAlpha1)+ t*g_vAlphaValue[1]:0.0;
   
   oColor  =  oColor+cMode;
}
void PSBurn(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{     
   float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1); 
    
   oColor.rgb =  (1.0f - rgba0.rgb) / (rgba1.rgb+1.0f/255.0f);
   oColor.rgb =  1.0f - saturate(oColor.rgb);  
     
   DestColor(sAlpha0,sAlpha1,oColor);
}
void PSDodge(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
    float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
      
   oColor.rgb = rgba0.rgb / (1.0 - rgba1.rgb);   
   oColor.rgb = min(oColor.rgb, 1.0);   
      
   DestColor(sAlpha0,sAlpha1,oColor);
}
void PSDarken(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
      
   oColor.rgb = min(rgba0,rgba1); 
      
   DestColor(sAlpha0,sAlpha1,oColor);    
}
void PSDifference(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
      
   oColor.rgb = abs(rgba0 - rgba1);
      
   DestColor(sAlpha0,sAlpha1,oColor);    
}
void PSDiv(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
      
   oColor.rgb = rgba0 / (0.10f/255.0f +rgba1);
      
   DestColor(sAlpha0,sAlpha1,oColor);    
}
float hardlight(float c0 ,float c1)
{
   float rC = 0.0f; 
   if(c1 > 0.5)   
       rC = min(1.0 - (1.0 - c0) * (1.0 - (c1 - 0.5) * 2), 1.0);   
   else   
      rC = min(c0 * c1 * 2, 1.0);   
  
   return rC;
}
void PSHardLight(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
      
   oColor.r = hardlight(rgba0.r,rgba1.r);
   oColor.g = hardlight(rgba0.g,rgba1.g);
   oColor.b = hardlight(rgba0.b,rgba1.b);
      
   DestColor(sAlpha0,sAlpha1,oColor);    
}
void PSLighten(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
      
   oColor.rgb = max(rgba0,rgba1); 
      
   DestColor(sAlpha0,sAlpha1,oColor);    
}
void PSOverlay(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
   //
  oColor.rgb  = multiplyKernel(rgba0, rgba0 +  multiplyKernel(2 * rgba1, 1.0 - rgba0));  
  //  
  DestColor(sAlpha0,sAlpha1,oColor);
}
void PSScreen(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
   //
  oColor.rgb  = 1.0 - multiplyKernel(1.0 - rgba0, 1.0 - rgba1);  
  //  
  DestColor(sAlpha0,sAlpha1,oColor);
}
void PSSoftLight(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
   //
  float3 tempm1 = multiplyKernel(rgba0, rgba1);
   float3 temps1 = 1.0 - multiplyKernel(1.0 - rgba0, 1.0 - rgba1);
   oColor.rgb = multiplyKernel(1.0 - rgba0, tempm1) + multiplyKernel(rgba0, temps1);
  //  
  DestColor(sAlpha0,sAlpha1,oColor);
}
void PSSub(float2 vTex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
   SourceColor_RGBA(vTex,rgba0,rgba1,sAlpha0,sAlpha1);   
      
   oColor.rgb = rgba0 - rgba1;
      
   DestColor(sAlpha0,sAlpha1,oColor);    
}
technique Mul
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSMul();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Add
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSAdd();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Alpha
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSAlpha();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Burn
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSBurn();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Dodge
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSDodge();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Darken
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSDarken();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Difference
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSDifference();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Div
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSDiv();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique HardLight
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PSHardLight();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Lighten
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSLighten();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique OverLay
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSOverlay();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Screen
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSScreen();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique SoftLight
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSSoftLight();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}
technique Sub
{
	PASS P0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 PSSub();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;		
	}	
}