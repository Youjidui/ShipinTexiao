float4x4 g_matWorldViewProj;
float4x4 g_matTexture;
texture g_txColor;
float2 fBaseCoef;
float4 g_vMisc;

float4 g_vSoft;

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

texture	tableCoef8;
sampler samCoef8 = 
sampler_state
{
	Texture = <tableCoef8>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};


void VertScene(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	

	
	oPos=mul(float4(vPos,1.f),g_matWorldViewProj);
	oPos.y += g_vMisc.x * oPos.w;
	oTex=mul(float4(vTex,1.0f,0.0f),g_matTexture);		
}	


float4 PixScene(float2 Tex:TEXCOORD0):COLOR0
{
	float4 fColor=(float4)0;		
	fColor=tex2D(g_samColor,Tex);		
	return fColor;
}
//AntiAlias
void VS(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0,
				out float2 oSrcTex:TEXCOORD1)
{	

	
	oPos=mul(float4(vPos,1.f),g_matWorldViewProj);	
	oTex=mul(float4(vTex,1.0f,0.0f),g_matTexture);
	oSrcTex = vTex;		
}	
float4 PS(float2 Tex:TEXCOORD0,float2 oSrcTex:TEXCOORD1):COLOR0
{
	float4 fColor=(float4)0;		
	fColor=tex2D(g_samColor,Tex);	
		
	float2 fTex = 0.5f - abs(oSrcTex - 0.5f);
	fTex /= g_vSoft.xy;
	fTex = saturate(fTex);
	float a = fTex.x * fTex.y;
	
	fColor.a *= a;
	
	return fColor;
}

technique DME
{
	PASS P0
	{
		VertexShader=compile vs_1_1 VertScene();
		PixelShader=compile  ps_2_0 PixScene();
		CullMode=None;
		AlphaBlendEnable = False;		
	}
	PASS P1
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS();
		CullMode=None;
		AlphaBlendEnable = False;		
	}
}

void VS_Imai8Tap(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	
	oPos=mul(float4(vPos,1.f),g_matWorldViewProj);
	oTex=mul(float4(vTex,1.0f,0.0f),g_matTexture);		
}	

float4 PS_Imai8Tap(float2 Tex:TEXCOORD0):COLOR0
{
	//return tex2D(g_samColor, Tex);
	float4 fColor=(float4)0;
	for(int i=-3; i<=4; ++i)
	{
		for(int j=-3; j<=4; ++j)
		{
            fColor += tex2D(g_samColor, Tex + float2(j,i) * fBaseCoef.xy) * (tex2D(samCoef8, float2(j+3.f, i+3.f)/8).r);
		}
	}
	return fColor;
}

technique Imai8Tap
{
	pass P0
	{
		VertexShader = compile vs_3_0	VS_Imai8Tap();
		PixelShader = compile ps_3_0	PS_Imai8Tap();
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
		//FillMode = WireFrame;
	}
}


