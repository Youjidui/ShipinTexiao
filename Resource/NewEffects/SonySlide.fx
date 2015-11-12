float4x4 g_matWorldViewProj;
float4x4 g_matTexture;

texture g_txColor;
texture g_txAlpha;
float   g_fAlpha;

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
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};


void VS(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	
	oPos=mul(float4(vPos,1.f),g_matWorldViewProj);
	oTex=mul(float4(vTex,1.0f,0.0f),g_matTexture);		
}	


float4 PS_YUVA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 fColor=(float4)0;
	
		
	fColor=tex2D(g_samColor,Tex);
	fColor.a *= g_fAlpha;
	return fColor;
}

float4 PS_YUYV_NA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 fColor=(float4)0;	
		
	fColor=tex2D(g_samColor,Tex);	
	return fColor;
}
void  PS_YUYV_AP(float2 Tex:TEXCOORD0,out float4 oColor:COLOR0,out float4 oAlpha:COLOR1,uniform bool bHasAlpha)
{		
	oColor=tex2D(g_samColor,Tex);
	if(bHasAlpha)
		oAlpha=tex2D(g_samAlpha,Tex);
	else 
		oAlpha = 1.0f;	
	oAlpha *= g_fAlpha;
}


technique Picture
{
	PASS RGBA//YUVA_RGBA//P0
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUVA();
		CullMode=None;			
	}
	PASS  YUYV_NA//P1
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUYV_NA();
		CullMode=None;			
	}
	PASS  YUYV_AP_SRC_HAS_ALPHA//P2
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUYV_AP(true);
		CullMode=None;			
	}
	PASS  YUYV_AP_SRC_NO_ALPHA//P3
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUYV_AP(false);
		CullMode=None;				
	}
}
