#include "..\shaders\ColorSpaceFunc.INC"

#define cMode CS_YUVA_OFFSET

// 以下函数传入的参数inBlendValue0与inBlendValue1已经没有意义,默认值为1.0f;bCGBlended也没有意义,默认值为0.0f(false)

void ColorBlend_SourceColor_YUVA(	float4 inColor0, float4 inColor1, 
						float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
						out float4 sColor0, out float4 sColor1, out float sAlpha0, out float sAlpha1)
{
	sColor0 = inColor0 - cMode;
	sAlpha0 = inAlphaValue0 * sColor0.a;
	sColor0.rgb *= sAlpha0 * inBlendValue0;
	    
	sColor1 = inColor1 - cMode;
	sAlpha1 = inAlphaValue1 * sColor1.a;
	sColor1.rgb *= sAlpha1 * inBlendValue1;
}

void ColorBlend_SourceColor_RGBA(	float4 inColor0, float4 inColor1, 
						float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
						out float3 rgba0, out float3 rgba1, out float sAlpha0, out float sAlpha1)
{
	float4 sColor0 = 0.0f, sColor1 = 0.0f;
	ColorBlend_SourceColor_YUVA(	inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
						sColor0, sColor1, sAlpha0 ,sAlpha1);
	   	
	rgba1 = ColorSpace_YUVToRGB(sColor0.rgb);
	rgba0 = ColorSpace_YUVToRGB(sColor1.rgb);  
}

void ColorBlend_DestColor(float sAlpha0,float sAlpha1,inout float4 oColor)
{
	oColor.a   = sAlpha0 + sAlpha1 * (1.0f - sAlpha0);   
	    
	oColor.rgb = ColorSpace_RGBToYUV(saturate(oColor.rgb));

	oColor += cMode;   
}

void ColorBlend_PSMul(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f, rgba1 = 0.0f;   
	float sAlpha0 = 1.0f, sAlpha1 = 1.0f;  
	oColor = 0.0f;
	ColorBlend_SourceColor_RGBA(	inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
						rgba0, rgba1, sAlpha0, sAlpha1);
	//
	oColor.rgb  = multiplyKernel(rgba0.rgb, rgba1.rgb);  
	//
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);
}

void ColorBlend_PSAdd(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{      
	float4 sColor0 = 0.0f,sColor1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_YUVA(	inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
						sColor0, sColor1, sAlpha0, sAlpha1);   

	oColor.rgb    = sColor0  + sColor1 + cMode;
	oColor.a  =  sAlpha0 + sAlpha1*(1.0f - sAlpha0);
}

void ColorBlend_PSAlpha(	float4 inColor0, float4 inColor1, 
				float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, float bCGBlended, 
				out float4 oColor:COLOR0)
{	
   float4 sColor0 = inColor0 - cMode;
   float sAlpha0 = inAlphaValue0 * sColor0.a;
   sColor0.rgb  *= sAlpha0 * inBlendValue0;  
   
   float4 sColor1 = inColor1 - cMode;
   float sAlpha1 = sColor1.a;
   sColor1       *= sign(sAlpha1) * inBlendValue1;
   float4 t         = bCGBlended?sColor1*sign(sAlpha1):sColor1*sAlpha1;
   //自身 Alpha 考虑 CGBlend,全局 Alpha 不考虑 CGBlend,否则效果不对
   sAlpha1*=inAlphaValue1;
   oColor.a  =  sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;           
   
   oColor.rgb=any(oColor.a)?sColor0*(1.0-inAlphaValue1*sAlpha1)+ t*inAlphaValue1:0.0;
   
   oColor  =  oColor+cMode;
}

void ColorBlend_PSBurn(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{     
	float3 rgba0 = 0.0f, rgba1 = 0.0f;   
	float sAlpha0 = 1.0f, sAlpha1 = 1.0f;  
	oColor = 0.0f;
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1); 

	oColor.rgb =  (1.0f - rgba0.rgb) / (rgba1.rgb+1.0f/255.0f);
	oColor.rgb =  1.0f - saturate(oColor.rgb);  
	    
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);
}

void ColorBlend_PSDodge(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
		
	oColor.rgb = rgba0.rgb / (1.0 - rgba1.rgb);   
	oColor.rgb = min(oColor.rgb, 1.0);   
		
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);
}

void ColorBlend_PSDarken(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	    
	oColor.rgb = min(rgba0,rgba1); 
	    
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);
}

void ColorBlend_PSDifference(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	    
	oColor.rgb = abs(rgba0 - rgba1);
	    
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);    
}

void ColorBlend_PSDiv(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	    
	oColor.rgb = rgba0 / (0.10f/255.0f +rgba1);
	    
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);    
}

float ColorBlend_hardlight(float c0 ,float c1)
{
   float rC = 0.0f; 
   if(c1 > 0.5)   
       rC = min(1.0 - (1.0 - c0) * (1.0 - (c1 - 0.5) * 2), 1.0);   
   else   
      rC = min(c0 * c1 * 2, 1.0);   
  
   return rC;
}

void ColorBlend_PSHardLight(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	    
	oColor.r = ColorBlend_hardlight(rgba0.r,rgba1.r);
	oColor.g = ColorBlend_hardlight(rgba0.g,rgba1.g);
	oColor.b = ColorBlend_hardlight(rgba0.b,rgba1.b);
	    
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);    
}

void ColorBlend_PSLighten(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	    
	oColor.rgb = max(rgba0,rgba1); 
	    
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);    
}

void ColorBlend_PSOverlay(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	//
	oColor.rgb  = multiplyKernel(rgba0, rgba0 +  multiplyKernel(2 * rgba1, 1.0 - rgba0));  
	//  
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);
}

void ColorBlend_PSScreen(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	//
	oColor.rgb  = 1.0 - multiplyKernel(1.0 - rgba0, 1.0 - rgba1);  
	//  
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);
}

void ColorBlend_PSSoftLight(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	//
	float3 tempm1 = multiplyKernel(rgba0, rgba1);
	float3 temps1 = 1.0 - multiplyKernel(1.0 - rgba0, 1.0 - rgba1);
	oColor.rgb = multiplyKernel(1.0 - rgba0, tempm1) + multiplyKernel(rgba0, temps1);
	//  
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);
}

void ColorBlend_PSSub(	float4 inColor0, float4 inColor1, 
			float inAlphaValue0, float inAlphaValue1, float inBlendValue0, float inBlendValue1, 
			out float4 oColor:COLOR0)
{
	float3 rgba0 = 0.0f,rgba1 = 0.0f;   
	float sAlpha0 = 1.0f,sAlpha1 = 1.0f;  
	oColor = 0.0f;	
	ColorBlend_SourceColor_RGBA(inColor0, inColor1, inAlphaValue0, inAlphaValue1, inBlendValue0, inBlendValue1, 
								rgba0,rgba1,sAlpha0,sAlpha1);   
	    
	oColor.rgb = rgba0 - rgba1;
	    
	ColorBlend_DestColor(sAlpha0,sAlpha1,oColor);    
}
