matrix		g_matWVP;
float4		g_color;
texture		g_texColor;
float2		g_texOffset;

sampler g_samColor=
sampler_state
{	
	Texture = <g_texColor>;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};


void	VSmain_Tex1(float3 iPos:POSITION, 
				float2 texCoord:TEXCOORD0,
				out float4 oPos:POSITION,
				out float2 oTexCoord:TEXCOORD0)
{
	oPos		= mul( float4(iPos,1.f), g_matWVP );
	oTexCoord	= texCoord + g_texOffset;
}

float4	PSmain_A8(float4 iColor:COLOR,
				float2 texCoord:TEXCOORD0):COLOR
{
	float grayColor = tex2D(g_samColor,texCoord).a;
	return float4(grayColor,grayColor,grayColor,1.f);
}

technique DrawTexA8
{
	pass P0
	{
		VertexShader = compile vs_3_0	VSmain_Tex1();
		PixelShader = compile ps_3_0	PSmain_A8();	
		
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}

////////////////////////////////////////////////////////////////

void	VSmain_Tex0(float3 iPos:POSITION, out float4 oPos:POSITION)
{
	oPos	= mul( float4(iPos,1.f), g_matWVP );
}

float4	PSmain_ColorAll():COLOR
{
	return g_color;
}

technique DrawColorAll
{
	pass P0
	{
		VertexShader = compile vs_3_0	VSmain_Tex0();
		PixelShader = compile ps_3_0	PSmain_ColorAll();	
		
		ZEnable = FALSE;
		AlphaBlendEnable = TRUE;
	}
}

////////////////////////////////////////////////////////////////
matrix		g_matText0;
float		g_alphaValue;

void	VSmain_Tex1Mat(float3 iPos:POSITION, 
				float2 texCoord:TEXCOORD0,
				out float4 oPos:POSITION,
				out float2 oTexCoord:TEXCOORD0)
{
	oPos		= mul( float4(iPos,1.f), g_matWVP );
	oTexCoord	= mul( float3(texCoord,1.f), g_matText0 );
}

float4	PSmain_COLOR32(float4 iColor:COLOR,
				float2 texCoord:TEXCOORD0):COLOR
{
	float4 color = tex2D(g_samColor,texCoord);
	color.a *= g_alphaValue;
	return color;
}

technique DrawTexColor32
{
	pass P0
	{
		VertexShader = compile vs_3_0	VSmain_Tex1Mat();
		PixelShader = compile ps_3_0	PSmain_COLOR32();	
		
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}

////////////////////////////////////////////////////////////////
bool		g_bSrcAlpha;
texture		g_texAlpha;

sampler g_samAlpha=
sampler_state
{	
	Texture = <g_texAlpha>;
	MinFilter = Linear;
	MagFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};

float4 PSmain_YUYV(float4 iColor:COLOR,
				float2 texCoord:TEXCOORD0):COLOR
{
	return tex2D(g_samColor,texCoord);
}

void PSmain_YUYVA(float4 iColor:COLOR,
				float2 texCoord:TEXCOORD0,
				out float4 oColor:COLOR0,
				out float4 oAlpha:COLOR1)
{
	oColor = tex2D(g_samColor,texCoord);
	if( g_bSrcAlpha )
		oAlpha = tex2D(g_samAlpha,texCoord);
	else
		oAlpha = 1.f;
	oAlpha *= g_alphaValue;
}

technique DrawTexYUYV
{
	pass P0
	{
		VertexShader = compile vs_3_0	VSmain_Tex1Mat();
		PixelShader = compile ps_3_0	PSmain_YUYV();	
		
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}

technique DrawTexYUYVA
{
	pass P0
	{
		VertexShader = compile vs_3_0	VSmain_Tex1Mat();
		PixelShader = compile ps_3_0	PSmain_YUYVA();	
		
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}
