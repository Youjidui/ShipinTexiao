///////////////////////////BackGround//////////////////////////////////
float4x4 g_matWorldViewProj;
float4x4 g_matTexBG;
texture g_txBG;
float	g_fAlpha;

sampler g_samBG =
sampler_state
{	
	Texture=<g_txBG>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VS_INPUT_BG
{
	float3 vPos:POSITION;
	float2 vTexCoord:TEXCOORD0;
};

struct PS_INPUT_BG
{
	float4 vPos:POSITION;
	float2 vTex:TEXCOORD0;
};



//Vs
PS_INPUT_BG VS_BG(VS_INPUT_BG input)
{	
	PS_INPUT_BG output  =  (PS_INPUT_BG) 0;
	output.vPos = mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexBG);
			
	return output;
}

//Ps
void PS_BG(PS_INPUT_BG input,uniform bool bBlend, out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samBG,input.vTex);	
	oColor.a *= g_fAlpha;	
	
	if(bBlend)
		oColor.rgb *= oColor.a;
}

technique BG
{
	PASS P0 //P0
	{
		VertexShader = compile vs_2_0 VS_BG();
		PixelShader = compile  ps_2_0 PS_BG(0);
		CullMode = None;			
		ZEnable  =  False;
		StencilEnable = True;
		StencilFunc = Greater;
		StencilRef = 1;
	}
	PASS P1 //P0
	{
		VertexShader = compile vs_2_0 VS_BG();
		PixelShader = compile  ps_2_0 PS_BG(1);
		CullMode = None;			
		ZEnable  =  False;
		AlphaBlendEnable = True;
		SrcBlend = One;
		DestBlend = InvSrcAlpha;
	}
}