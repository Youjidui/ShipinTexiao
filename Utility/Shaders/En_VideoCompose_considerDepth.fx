float4x4 g_matWorldViewProj[8];
float4x4 g_matTex[8];

float3 g_vMisc0;

texture g_txYUVA0;
texture g_txYUVA1;
texture g_txDepth_v;
texture g_txDepth_cg;

#define _FX
#include ".\ColorSpaceFunc.INC"

sampler g_samYUVA0=
sampler_state
{	
	Texture = <g_txYUVA0>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samYUVA1=
sampler_state
{	
	Texture = <g_txYUVA1>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samDepth_v=
sampler_state
{	
	Texture = <g_txDepth_v>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samDepth_cg=
sampler_state
{	
	Texture = <g_txDepth_cg>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};


void VS(float4 vPos:POSITION,				
				float2 vTex:TEXCOORD0,												
				out float4 oPos:POSITION,				
				out float3 oTex:TEXCOORD0
				)
{	
	int nIndex = round(vPos.z);
	
	oPos=mul(float4(vPos.xy,0.0f,1.0f),g_matWorldViewProj[nIndex]);	
			
	oTex.xy=mul(float3(vTex.xy,1.0f),g_matTex[nIndex]);
	
	oTex.z = nIndex;
}
void PS_Compose(float3 Tex:TEXCOORD0,out float4 oColor[1]:COLOR0)
{	
	float4 vColor0 = tex2D(g_samYUVA0,Tex.xy);		
	float4 vColor1 = tex2D(g_samYUVA1,Tex.xy);
	
	float fDepth0 = tex2D(g_samDepth_v,Tex.xy).a;
	float fDepth1 = tex2D(g_samDepth_cg,Tex.xy).a;
	
	if(fDepth1 <= fDepth0 && vColor1.a > 0.5f / 255.0f)
	{
		oColor[0] = vColor1;
		//oColor[1] = fDepth1;
	}
	else
	{
		oColor[0] = vColor0;
		//oColor[1] = fDepth0;
	}
	
}

//ps

void PS_ALPHA(in float3 vMisc,inout float4 oColor:COLOR0)
{
	if( vMisc.y > 0.0f &&  oColor.a > 0.0f)
	{
		oColor -= cYUVAMode;
		oColor.rgb /= oColor.a;
		oColor += cYUVAMode;
	}
		
	oColor.a *= vMisc.x;
}

float Interpose2d(float x0, float y0, float x1, float y1, float x)
{
	float fk = (y1-y0)/(x1-x0);
	float fb = y0-fk*x0;
	return fk*x + fb;
}

float GetDepth_v(float3 Tex)
{
	float4 f4Depth_v = tex2D(g_samDepth_v, Tex.xy*0.5f);
	return 1.f-f4Depth_v.w;
}

float GetDepth_cg(float3 Tex)
{
	float4 f4Depth_v = tex2D(g_samDepth_cg, Tex.xy) *1024.f;
	return Interpose2d(-20.f, 0.f, 80.f, 1.f, f4Depth_v.w);
}

void PS1(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samYUVA0,Tex.xy);		

	PS_ALPHA(g_vMisc0,oColor);
	
	float fzDepth_cg = GetDepth_cg(Tex);
	float fzDepth_v = GetDepth_v(Tex);
	
	if (oColor.a > 0 && fzDepth_v < fzDepth_cg)			// using video
		oColor.a = 0.f;
	else if (oColor.a > 0 && fzDepth_cg < fzDepth_v)	// using cg
		oColor.a = 1.f;
}
 
 void PS_VDEPTH(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
 {	
	float fzDepth_v = GetDepth_v(Tex);
	oColor = float4(1.0f,0.5f, 0.5f,fzDepth_v);
 }
 
 void PS_CGDEPTH(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
 {
	float fzDepth_cg = GetDepth_cg(Tex);
	oColor = float4(1.0f, 0.5f, 0.5f, fzDepth_cg);
 }
 
void PS_BLENDDEPTH(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	float fzDepth_cg = GetDepth_cg(Tex);
	float fzDepth_v = GetDepth_v(Tex);
	if (fzDepth_v < fzDepth_cg)	// using video
		oColor = float4(1.0f, 0.5f, 0.5f, fzDepth_v);
	else						// using cg
		oColor = float4(1.0f, 0.5f, 0.5f, fzDepth_cg);
}

//
technique FullRender
{
	PASS Layer_1//P0
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS_Compose();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = False;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 	}
 	
 	PASS Layer_1//P0
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS1();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 	}
 	PASS V_DEPTH
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS_VDEPTH();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 	}
 	PASS CG_DEPTH
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS_CGDEPTH();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 	}
 	PASS BLEND_DEPTH
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS_BLENDDEPTH();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 	}	
}