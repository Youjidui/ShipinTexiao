#include"..\..\..\..\TPEngine\src\Common\TPRenderX\Shaders\ColorSpaceFunc.INC"

// For Sony Mask
float4x4 g_matWorldViewProj;
float4x4 g_matSrcTexture;
float4x4 g_matEffectTexture;
float4x4 g_matMask;

float4	 g_vMisc;
#define  g_nMaskType round(g_vMisc.x)
#define  g_bBorder g_vMisc.y
#define  g_fAlpha  g_vMisc.z
#define  g_fArea   g_vMisc.w

float4   g_vAlpha;
#define  g_fSrcAlpha g_vAlpha.x
#define  g_fEffectAlpha g_vAlpha.y
#define  g_bKey         g_vAlpha.z // ø€œÛ¥¶¿Ì

float4   g_vBorderColor;

texture g_txSrcColor;
texture g_txEffectColor;
texture g_txMask;
texture g_txSrcAlpha;
texture g_txEffectAlpha;

sampler g_samSrcColor = 
sampler_state
{	
	Texture = <g_txSrcColor>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samEffectColor = 
sampler_state
{	
	Texture = <g_txEffectColor>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samSrcAlpha = 
sampler_state
{	
	Texture = <g_txSrcAlpha>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samEffectAlpha = 
sampler_state
{	
	Texture = <g_txEffectAlpha>;
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
	float2 vSrcTex:TEXCOORD0;
	float2 vEffectTex:TEXCOORD1;
	float2 vMask:TEXCOORD2;
};


///////////////////////////Picture//////////////////////////////////
//Vs
PS_INPUT VS(VS_INPUT input)
{	
	PS_INPUT output  =  (PS_INPUT) 0;
	output.vPos = mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vSrcTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matSrcTexture);		
	output.vEffectTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matEffectTexture);		
	output.vMask = (output.vPos.xy / output.vPos.w + 1.0f) / 2.0f;
	output.vMask.y = 1.0f - output.vMask.y;
    output.vMask = mul(float4(output.vMask,1.0f,0.0f),g_matMask);	
			
	return output;
}
void Compose(float4 vMask,float4 vSrcColor,float4 vEffectColor,float4 vBorderColor,float4 cMode,out float4 oColor)
{	 	
	//vSrcColor = lerp(cMode,vSrcColor,g_fSrcAlpha);
	//vEffectColor = lerp(cMode,vEffectColor,g_fEffectAlpha);
	vSrcColor.a *= g_fSrcAlpha;
	vEffectColor.a *= g_fEffectAlpha;
	
	int nArea = round(g_fArea);		
		
	if(g_nMaskType == 0) //Berize
	{
		if(nArea == 1) //Mask
		{		
			if(vMask.a < 1.0f)
				vEffectColor.a *= g_fAlpha;		
		}
		else if(nArea == 2) //Invert
		{			
			if(vMask.a > 0.0f)
				vSrcColor.a *= g_fAlpha;	
		}		
	}
	else // g_nMaskType = 1 Fix Shape
	{
		if(nArea == 1) //Mask
		{
			if(vMask.b > 0.0f || vMask.a <= 0.0f || all(vMask.rg))					
				vEffectColor.a *= g_fAlpha;		
		}
		else if(nArea == 2) //Invert
		{
			if(vMask.g > 0.0f || (vMask.a > 0.0f  && vMask.r <= 0.0f) || all(vMask.rb))						
				vSrcColor.a *= g_fAlpha;	
		}		
	}	
	
	if(vMask.a > 0.0f ) // vMask.r Border / vMask.g Soft Inside / vMask.b Soft Outside 
	{			
		if(g_bBorder > 0.0f && any(vMask.rgb))
		{			
			oColor = vBorderColor;			
			
			if(vMask.g > 0.0f)
			{
				float4 vMastColor = lerp(vSrcColor,vBorderColor,vMask.a);
				float4 vSubColor = lerp(vEffectColor,vBorderColor,vMask.r);
				oColor = lerp(vMastColor,vSubColor,vMask.r);
			}
			else if(vMask.b > 0.0f)					
			{
				float4 vMastColor = lerp(vEffectColor,vBorderColor,vMask.a);
				float4 vSubColor = lerp(vSrcColor,vBorderColor,vMask.r);
				oColor = lerp(vMastColor,vSubColor,vMask.r);					
			}			
		}		
		else // Only Soft		
			oColor = lerp(vEffectColor,vSrcColor,vMask.a);		
	}
	else
		oColor = vEffectColor; 

	if (nArea == 0)		
		oColor.a *= g_fAlpha;
}
//Ps
void PS_NO_ALPHA(PS_INPUT input,		
		uniform int nType,
		out float4 oColor:COLOR0)
{
	float4 vSrcColor = 0.0f,vEffectColor = 0.0f;
	
	float4 vMask = tex2D(g_samMask,input.vMask);
	if(any(vMask))
	{
		if(any(vMask.rgb))
		{
			vSrcColor = tex2D(g_samSrcColor,input.vSrcTex);
			vEffectColor = tex2D(g_samEffectColor,input.vEffectTex);		
		}
		else
		{
			vSrcColor = tex2D(g_samSrcColor,input.vSrcTex);	
		}
	}	
	else
	{
		vEffectColor = tex2D(g_samEffectColor,input.vEffectTex);												
	}	
		
	float4 vBorderColor = g_vBorderColor;
	if(nType == 0) //yuyv
	{			
		float4 cMode = CS_YUVA_OFFSET;
		float4 YUV1,YUV2;
		
		Compose(vMask,float4(vSrcColor.rga,1.0f),float4(vEffectColor.rga,1.0f),vBorderColor,cMode,YUV1);
		Compose(vMask,float4(vSrcColor.bga,1.0f),float4(vEffectColor.bga,1.0f),vBorderColor,cMode,YUV2);	
		
		oColor = float4(YUV1.r,YUV1.g,YUV2.r,YUV1.b);
	}
	else if(nType == 1) //yuva
	{			
		float4 cMode = CS_YUVA_OFFSET;		
		Compose(vMask,vSrcColor,vEffectColor,vBorderColor,cMode,oColor);			
	}
	else if(nType == 2) //rgba
	{			
		float4 cMode = float4(0.0f,0.0f,0.0f,0.0f);		
		Compose(vMask,vSrcColor,vEffectColor,vBorderColor,cMode,oColor);			
	}
}

void PS_YUYV_NA(PS_INPUT input,		
		out float4 oColor:COLOR0,
		out float4 oAlpha:COLOR1)
{
	float4 vMask = tex2D(g_samMask,input.vMask);	
	float4 vSrcColor = tex2D(g_samSrcColor,input.vSrcTex);
	float4 vEffectColor = tex2D(g_samEffectColor,input.vEffectTex);
	
	float4 vBorderColor = g_vBorderColor;
	
	float4 cMode = CS_YUVA_OFFSET;
	float4 YUV1,YUV2;
	
	Compose(vMask,float4(vSrcColor.rga,1.0f),float4(vEffectColor.rga,1.0f),vBorderColor,cMode,YUV1);
	Compose(vMask,float4(vSrcColor.bga,1.0f),float4(vEffectColor.bga,1.0f),vBorderColor,cMode,YUV2);	
	
	oColor = float4(YUV1.r,YUV1.g,YUV2.r,YUV1.b);
	oAlpha = float4(YUV1.a,YUV1.a,YUV2.a,YUV2.a);
}
void PS_YUYV_AP(PS_INPUT input,	
			uniform int nNumAlpha,	
		out float4 oColor:COLOR0,
		out float4 oAlpha:COLOR1)
{
	float4 vMask = tex2D(g_samMask,input.vMask);	
	float4 vSrcColor = tex2D(g_samSrcColor,input.vSrcTex);
	float4 vEffectColor = tex2D(g_samEffectColor,input.vEffectTex);
	float4 vEffectAlpha = tex2D(g_samEffectAlpha,input.vEffectTex);
	float4 vSrcAlpha = vEffectAlpha;
	if(nNumAlpha == 1 && g_bKey)
		vSrcAlpha = 1;
	else if(nNumAlpha == 2)
		vSrcAlpha = tex2D(g_samSrcAlpha,input.vSrcTex);

		
	float4 vBorderColor = g_vBorderColor;
	
	float4 cMode = CS_YUVA_OFFSET;
	float4 YUV1,YUV2;
	
	Compose(vMask,float4(vSrcColor.rga,vSrcAlpha.r),float4(vEffectColor.rga,vEffectAlpha.r),vBorderColor,cMode,YUV1);
	Compose(vMask,float4(vSrcColor.bga,vSrcAlpha.b),float4(vEffectColor.bga,vEffectAlpha.b),vBorderColor,cMode,YUV2);	
	
	oColor = float4(YUV1.r,YUV1.g,YUV2.r,YUV1.b);
	oAlpha = float4(YUV1.a,YUV1.a,YUV2.a,YUV2.a);
}
technique Picture
{
	PASS RGBA_YUYV //P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_NO_ALPHA(0);
		CullMode = None;			
		//ZEnable  =  False;
	}
	PASS RGBA_YUVA //P1
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_NO_ALPHA(1);
		CullMode = None;			
		//ZEnable  =  False;
	}	
	PASS RGBA_RGBA //P2
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_NO_ALPHA(2);
		CullMode = None;			
		//ZEnable  =  False;
	}
 	// out Alpha	
	PASS RGBA_YUYV_NA //P3
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_YUYV_NA();
		CullMode = None;			
		//ZEnable  =  False;
	}
	PASS RGBA_YUYV_1AP //P4
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_YUYV_AP(1);
		CullMode = None;			
		//ZEnable  =  False;
	}
	PASS RGBA_YUYV_2AP //P5
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_YUYV_AP(2);
		CullMode = None;			
		//ZEnable  =  False;
	}
}