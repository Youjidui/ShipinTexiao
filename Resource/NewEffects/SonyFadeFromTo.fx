float4x4 g_matWorldViewProj;
float4x4 g_matTexture;
float4 g_FadeColor;
float g_FadeScale;
texture g_txColor;
texture g_txAlpha;
float	g_fAlpha;
float4  g_colorBlack;

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
	float4 srcColor=(float4)0;
	
		
	srcColor=tex2D(g_samColor,Tex) ;
	if(srcColor.a == 0)
		srcColor.rgb = g_colorBlack;
	srcColor.a *= g_fAlpha;
	
	srcColor.rgb = lerp(g_colorBlack,srcColor,srcColor.a);
	float4 oColor = lerp(srcColor,g_FadeColor,g_FadeScale);
	oColor.a = lerp(srcColor.a,1,g_FadeScale);
	if(oColor.a > 0.5f / 255.0f)
		oColor.rgb = lerp(g_colorBlack,oColor.rgb,1.0f / oColor.a);
	
	return oColor;
}

float4 PS_YUYV_NA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 srcColor=(float4)0;	
		
	srcColor=tex2D(g_samColor,Tex) ;	
	
	return lerp(srcColor,g_FadeColor,g_FadeScale);
}

void  PS_YUYV_AP(float2 Tex:TEXCOORD0,uniform bool bSrcAlpha, out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{		
   float4 srcColor=tex2D(g_samColor,Tex);
   float4 srcAlpha = 1.0f;
   
   if(bSrcAlpha)
		srcAlpha=tex2D(g_samAlpha,Tex);

   srcAlpha *= g_fAlpha;	
   oColor = lerp(srcColor,g_FadeColor,g_FadeScale);
   oAlpha = lerp(srcAlpha,1.0f,g_FadeScale);
}
			

technique Render
{
	PASS RGBA_YUVA //P0  
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUVA();
		CullMode=None;
		AlphaBlendEnable = False;			
	}
	
	PASS YUYV_NA //P1
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUYV_NA();
		CullMode=None;
		AlphaBlendEnable = False;			
	}
	PASS YUYV_AP //P2
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUYV_AP(1);
		CullMode=None;
		AlphaBlendEnable = False;			
	}
	PASS YUYV_AP //P3
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS_YUYV_AP(0);
		CullMode=None;
		AlphaBlendEnable = False;			
	}
}

