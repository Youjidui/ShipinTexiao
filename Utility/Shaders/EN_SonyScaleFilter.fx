#include ".\ColorSpaceFunc.INC"

float4x4 g_matWVP;
float4x4 g_matTex;

float  g_fOffset;
float4 g_txSize;

texture g_txColor;

sampler g_samColor =
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

texture g_txCoef;
sampler g_samCoef =
sampler_state
{	
	Texture = <g_txCoef>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};
void VS(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float4 oTex:TEXCOORD0)
{	
	oPos = mul(float4(vPos,1.f),g_matWVP);
	oTex.xy = mul(float3(vTex,1.0f),g_matTex);		
	oTex.zw = vTex;	
}	

float4 VBlur(float2 inTex : TEXCOORD0) : COLOR0
{
	return tex2D(g_samColor, inTex);
	float4 color = (float4)0;
	float coef[] = {0.0066461414f, 0.19422872f, 0.59826660f, 0.19422872f, 0.0066461414f};
	for(int i = -2; i <= 2; ++i)
		color += tex2D(g_samColor, clamp(inTex + float2(0, i) * g_txSize.zw, float2(g_matTex._41, g_matTex._42), float2(g_matTex._43, g_matTex._44))) * coef[i + 2];
	return color;
}

float4 HBlur(float2 inTex : TEXCOORD0) : COLOR0
{
	return tex2D(g_samColor, inTex);
	float4 color = (float4)0;
	float coef[] = {0.0066461414f, 0.19422872f, 0.59826660f, 0.19422872f, 0.0066461414f};
	for(int i = -2; i <= 2; ++i)
		color += VBlur(clamp(inTex + float2(i, 0) * g_txSize.zw, float2(g_matTex._41, g_matTex._42), float2(g_matTex._43, g_matTex._44))) * coef[i + 2];
	return color;
}

float4 PS_P4_H(float4 inTex:TEXCOORD0):COLOR0
{
	float4 fColor = (float4)0;	
	
	float2 Tex = inTex.xy;
	float fX = Tex.x * g_txSize.x + 1e-3;
	float4 vCoef = tex2D(g_samCoef, float2(frac(fX), 0));		

	Tex.x = int(fX) * g_txSize.z;
	Tex += 0.5f * g_txSize.zw;
			
	for(int i = -1; i <= 2; ++i)
		fColor += VBlur(Tex + float2(i, 0) * g_txSize.zw) * vCoef[i + 1];
				
	return fColor;
}

float4 PS_P4_V(float4 inTex:TEXCOORD0):COLOR0
{
	float4 fColor = (float4)0;	
	float2 Tex = inTex.xy;
	float fY = Tex.y * g_txSize.y;
	float4 vCoef = tex2D(g_samCoef, float2(frac(fY), 0));
		
	Tex.y = int(fY) * g_txSize.w;
	Tex += 0.5f * g_txSize.zw;
	
	for(int i = -1; i <= 2; ++i)
		fColor += VBlur(Tex + float2(0, i) * g_txSize.zw) * vCoef[i + 1];
			
	return fColor;
}

float4 PS_P8_H_422(float4 inTex:TEXCOORD0):COLOR0
{
	float4 fColor = (float4)0;	
	float2 Tex = inTex.xy;
	float fX = Tex.x * g_txSize.x + 1e-3;
	float4 vCoef0 = tex2D(g_samCoef,float2(frac(fX) + 0.1f / 16.0f,0.1f / 2.0f));	
	float4 vCoef1 = tex2D(g_samCoef,float2(frac(fX) + 0.1f / 16.0f,1.1f / 2.0f));	
	float  vCoefY[8] = {vCoef0,vCoef1};
	
 	float fXuv = Tex.x * g_txSize.x / 2.0f + 1e-3;
 	vCoef0 = tex2D(g_samCoef,float2(frac(fXuv) + 0.1f / 16.0f,0.1f / 2.0f));	
 	vCoef1 = tex2D(g_samCoef,float2(frac(fXuv) + 0.1f / 16.0f,1.1f / 2.0f));	
 	float  vCoefUV[8] = {vCoef0,vCoef1};		

	Tex.x = int(fX) * g_txSize.z;
	Tex.xy += 0.5f * g_txSize.zw;
		
	int nPixel[12] = {-6,-4,-3,-2,-1,0,1,2,3,4,6,8};	
	int nUVPixel[8] = {0,1,3,5,7,9,10,11};	
	
	int nPixel1[12] = {-7,-5,-3,-2,-1,0,1,2,3,4,5,7};
	int nUVPixel1[8] = {0,1,2,4,6,8,10,11};
	
	if(frac(fXuv) >= 0.5f)
	{
		for(int i = 0; i < 8;i ++)
		   nUVPixel[i] = nUVPixel1[i];	
		
		for(int i = 0; i < 12;i ++)		
			nPixel[i] = nPixel1[i];	
	}
	
	int nYPixel[8] = {2,3,4,5,6,7,8,9};
	float4 vPixel[12];
	for(int i = 0;i < 12;i ++)
	{
		vPixel[i] = tex2D(g_samColor,clamp(Tex + float2(nPixel[i],0.0f) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44)));
	}
	
	float2 YA = 0;
	float2 UV = 0;
	for(int i = 0;i < 8 ;i ++)
	{
		YA  += vPixel[nYPixel[i]].xw   * vCoefY[i];
		UV += vPixel[nUVPixel[i]].yz * vCoefUV[i];
	}
	fColor = float4(YA.x,UV,YA.y);
	
	return fColor;
}
float4 PS_P8_H_444(float4 inTex:TEXCOORD0):COLOR0
{
	float4 fColor = (float4)0;	
	float2 Tex = inTex.xy;
	float fX = Tex.x * g_txSize.x + 1e-3;
	float4 vCoef0 = tex2D(g_samCoef,float2(frac(fX) + 0.1f / 16.0f,0.1f / 2.0f));	
	float4 vCoef1 = tex2D(g_samCoef,float2(frac(fX) + 0.1f / 16.0f,1.1f / 2.0f));	
		
	Tex.x = int(fX) * g_txSize.z;
	Tex += 0.5f * g_txSize.zw;
	
	for(int i = -3 ;i < 1;i ++)		
		fColor += tex2D(g_samColor,clamp(Tex + float2(i,0.0f) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef0[i + 3];
	
	for(int i = 1 ;i < 5;i ++)		
		fColor += tex2D(g_samColor,clamp(Tex + float2(i,0.0f) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef1[i - 1];
		
	return fColor;
}
float4 PS_P8_V(float4 inTex:TEXCOORD0):COLOR0
{
	float2 Tex = inTex.xy;
		
	float fEdge = (0.5f - abs(inTex.w - 0.5f)) * g_txSize.y;
	fEdge = smoothstep(0.0f,1.0f,fEdge);	
		
	float fY = 	Tex.y * g_txSize.y + g_fOffset * fEdge + 1e-3;
	float4 vCoef0 = tex2D(g_samCoef,float2(frac(fY) + 0.1f / 16.0f,0.1f / 2.0f));	
	float4 vCoef1 = tex2D(g_samCoef,float2(frac(fY) + 0.1f / 16.0f,1.1f / 2.0f));		
		
	Tex.y = int(fY) * g_txSize.w;
	
	Tex += 0.5f * g_txSize.zw;	
		
	float4 fColor = (float4)0;	
	for(int i = -3 ;i < 1;i ++)		
		fColor += tex2D(g_samColor,clamp(Tex + float2(0.0f,i) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef0[i + 3];
	
	for(int i = 1 ;i < 5;i ++)		
		fColor += tex2D(g_samColor,clamp(Tex + float2(0.0f,i) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef1[i - 1];		
	
	return fColor;
}

technique P4
{
	PASS P0//Hor
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_P4_H();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
	PASS P1//Hor
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_P4_H();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
	PASS P2//Vert
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_P4_V();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
}
technique P8
{
	PASS P0//Hor
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_P8_H_444();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
	PASS P1//Hor
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_P8_H_422();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
	PASS P2//vert
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_P8_V();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}
}