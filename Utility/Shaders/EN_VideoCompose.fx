float4x4 g_matWorldViewProj[8];
float4x4 g_matTex[8];

float3 g_vMisc0;
float3 g_vMisc1;
float3 g_vMisc2;
float3 g_vMisc3;
float3 g_vMisc4;
float3 g_vMisc5;
float3 g_vMisc6;
float3 g_vMisc7;

texture g_txYUVA0;
texture g_txYUVA1;
texture g_txYUVA2;
texture g_txYUVA3;
texture g_txYUVA4;
texture g_txYUVA5;
texture g_txYUVA6;
texture g_txYUVA7;

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
sampler g_samYUVA2=
sampler_state
{	
	Texture = <g_txYUVA2>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samYUVA3=
sampler_state
{	
	Texture = <g_txYUVA3>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samYUVA4=
sampler_state
{	
	Texture = <g_txYUVA4>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samYUVA5=
sampler_state
{	
	Texture = <g_txYUVA5>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samYUVA6=
sampler_state
{	
	Texture = <g_txYUVA6>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samYUVA7=
sampler_state
{	
	Texture = <g_txYUVA7>;
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
 void PS1(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
 {	
 	oColor = tex2D(g_samYUVA0,Tex.xy);		

 	PS_ALPHA(g_vMisc0,oColor);
 }
void PS2(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = 0;
	float3 vMisc = 0;	
	if(Tex.z > 0.5f)
	{
		oColor = tex2D(g_samYUVA1,Tex.xy);	
		vMisc = g_vMisc1;	
	}
	else 
	{
		oColor = tex2D(g_samYUVA0,Tex.xy);
		vMisc = g_vMisc0;	
	}
	PS_ALPHA(vMisc,oColor);	
}
void PS3(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = 0;
	float3 vMisc = 0;	
	if(Tex.z > 1.5f)
	{
		oColor = tex2D(g_samYUVA2,Tex.xy);	
		vMisc = g_vMisc2;	
	}
	else if(Tex.z > 0.5f)
	{
		oColor = tex2D(g_samYUVA1,Tex.xy);	
		vMisc = g_vMisc1;	
	}
	else 
	{
		oColor = tex2D(g_samYUVA0,Tex.xy);
		vMisc = g_vMisc0;	
	}
	PS_ALPHA(vMisc,oColor);	
}
void PS4(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = 0;
	float3 vMisc = 0;	
	if(Tex.z > 2.5f)
	{
		oColor = tex2D(g_samYUVA3,Tex.xy);	
		vMisc = g_vMisc3;	
	}
	else if(Tex.z > 1.5f)
	{
		oColor = tex2D(g_samYUVA2,Tex.xy);	
		vMisc = g_vMisc2;	
	}
	else if(Tex.z > 0.5f)
	{
		oColor = tex2D(g_samYUVA1,Tex.xy);	
		vMisc = g_vMisc1;	
	}
	else 
	{
		oColor = tex2D(g_samYUVA0,Tex.xy);
		vMisc = g_vMisc0;	
	}
	PS_ALPHA(vMisc,oColor);	
}
void PS5(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = 0;
	float3 vMisc = 0;	
	if(Tex.z > 3.5f)
	{
		oColor = tex2D(g_samYUVA4,Tex.xy);	
		vMisc = g_vMisc4;	
	}
	else if(Tex.z > 2.5f)
	{
		oColor = tex2D(g_samYUVA3,Tex.xy);	
		vMisc = g_vMisc3;	
	}
	else if(Tex.z > 1.5f)
	{
		oColor = tex2D(g_samYUVA2,Tex.xy);	
		vMisc = g_vMisc2;	
	}
	else if(Tex.z > 0.5f)
	{
		oColor = tex2D(g_samYUVA1,Tex.xy);	
		vMisc = g_vMisc1;	
	}
	else 
	{
		oColor = tex2D(g_samYUVA0,Tex.xy);
		vMisc = g_vMisc0;	
	}
	PS_ALPHA(vMisc,oColor);	
}
void PS6(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = 0;
	float3 vMisc = 0;	
	if(Tex.z > 4.5f)
	{
		oColor = tex2D(g_samYUVA5,Tex.xy);	
		vMisc = g_vMisc5;	
	}
	else if(Tex.z > 3.5f)
	{
		oColor = tex2D(g_samYUVA4,Tex.xy);	
		vMisc = g_vMisc4;	
	}
	else if(Tex.z > 2.5f)
	{
		oColor = tex2D(g_samYUVA3,Tex.xy);	
		vMisc = g_vMisc3;	
	}
	else if(Tex.z > 1.5f)
	{
		oColor = tex2D(g_samYUVA2,Tex.xy);	
		vMisc = g_vMisc2;	
	}
	else if(Tex.z > 0.5f)
	{
		oColor = tex2D(g_samYUVA1,Tex.xy);	
		vMisc = g_vMisc1;	
	}
	else 
	{
		oColor = tex2D(g_samYUVA0,Tex.xy);
		vMisc = g_vMisc0;	
	}
	PS_ALPHA(vMisc,oColor);	
}
void PS7(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = 0;
	float3 vMisc = 0;	
	if(Tex.z > 5.5f)
	{
		oColor = tex2D(g_samYUVA6,Tex.xy);	
		vMisc = g_vMisc6;	
	}
	else if(Tex.z > 4.5f)
	{
		oColor = tex2D(g_samYUVA5,Tex.xy);	
		vMisc = g_vMisc5;	
	}
	else if(Tex.z > 3.5f)
	{
		oColor = tex2D(g_samYUVA4,Tex.xy);	
		vMisc = g_vMisc4;	
	}
	else if(Tex.z > 2.5f)
	{
		oColor = tex2D(g_samYUVA3,Tex.xy);	
		vMisc = g_vMisc3;	
	}
	else if(Tex.z > 1.5f)
	{
		oColor = tex2D(g_samYUVA2,Tex.xy);	
		vMisc = g_vMisc2;	
	}
	else if(Tex.z > 0.5f)
	{
		oColor = tex2D(g_samYUVA1,Tex.xy);	
		vMisc = g_vMisc1;	
	}
	else 
	{
		oColor = tex2D(g_samYUVA0,Tex.xy);
		vMisc = g_vMisc0;	
	}
	PS_ALPHA(vMisc,oColor);	
}

void PS8(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = 0;
	float3 vMisc = 0;	
	if(Tex.z > 6.5f)
	{
		oColor = tex2D(g_samYUVA7,Tex.xy);	
		vMisc = g_vMisc7;	
	}
	else if(Tex.z > 5.5f)
	{
		oColor = tex2D(g_samYUVA6,Tex.xy);	
		vMisc = g_vMisc6;	
	}
	else if(Tex.z > 4.5f)
	{
		oColor = tex2D(g_samYUVA5,Tex.xy);	
		vMisc = g_vMisc5;	
	}
	else if(Tex.z > 3.5f)
	{
		oColor = tex2D(g_samYUVA4,Tex.xy);	
		vMisc = g_vMisc4;	
	}
	else if(Tex.z > 2.5f)
	{
		oColor = tex2D(g_samYUVA3,Tex.xy);	
		vMisc = g_vMisc3;	
	}
	else if(Tex.z > 1.5f)
	{
		oColor = tex2D(g_samYUVA2,Tex.xy);	
		vMisc = g_vMisc2;	
	}
	else if(Tex.z > 0.5f)
	{
		oColor = tex2D(g_samYUVA1,Tex.xy);	
		vMisc = g_vMisc1;	
	}
	else 
	{
		oColor = tex2D(g_samYUVA0,Tex.xy);
		vMisc = g_vMisc0;	
	}
	PS_ALPHA(vMisc,oColor);	
}

float4 Blend(uniform sampler sam,float2 Tex,float3 vMisc)
{
	float4 vColor = tex2D(sam,Tex.xy);
	if( vMisc.y > 0.0f &&  vColor.a > 0.0f)
	{
		vColor -= cYUVAMode;
		vColor.rgb /= vColor.a;
		vColor += cYUVAMode;
	}	
	vColor.a *= vMisc.x;
	
	return vColor;
}

//压层时，需要单独 Alpha Blend ,A  压 B 公式 = Aalpha  + Balpha * (1 - Aalpha)
void FULL_PS2(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{
	oColor = Blend(g_samYUVA1,Tex.xy,g_vMisc1);
	//if(oColor.a < 1.0f)
	{
		float3 vBlack = float3(16.0f / 255.0f,cYUVAMode.yz);
		float4 vColor0 = Blend(g_samYUVA0,Tex.xy,g_vMisc0);	
		vColor0 = lerp(float4(vBlack.rgb,1.0f),float4(vColor0.rgb,1.0f),vColor0.a);		
		
		oColor = lerp(vColor0,float4(oColor.rgb,1.0f),oColor.a);
	}
}
void FULL_PS3(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = Blend(g_samYUVA2,Tex.xy,g_vMisc2);
	//if(oColor.a < 1.0f)
	{
		float4 vColor0 = 0;
		FULL_PS2(Tex,vColor0);		
		
		oColor = lerp(vColor0,float4(oColor.rgb,1.0f),oColor.a);
	}
}
void FULL_PS4(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = Blend(g_samYUVA3,Tex.xy,g_vMisc3);
	//if(oColor.a < 1.0f)
	{
		float4 vColor0 = 0;
		FULL_PS3(Tex,vColor0);	
		
		oColor = lerp(vColor0,float4(oColor.rgb,1.0f),oColor.a);
	}
}
void FULL_PS5(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = Blend(g_samYUVA4,Tex.xy,g_vMisc4);
	//if(oColor.a < 1.0f)
	{
		float4 vColor0 = 0;
		FULL_PS4(Tex,vColor0);	
		
		oColor = lerp(vColor0,float4(oColor.rgb,1.0f),oColor.a);
	}
}
void FULL_PS6(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{
	oColor = Blend(g_samYUVA5,Tex.xy,g_vMisc5);
	//if(oColor.a < 1.0f)
	{
		float4 vColor0 = 0;
		FULL_PS5(Tex,vColor0);
		oColor = lerp(vColor0,float4(oColor.rgb,1.0f),oColor.a);
	}
}
void FULL_PS7(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = Blend(g_samYUVA6,Tex.xy,g_vMisc6);
	//if(oColor.a < 1.0f)
	{
		float4 vColor0 = 0;
		FULL_PS6(Tex,vColor0);
		oColor = lerp(vColor0,float4(oColor.rgb,1.0f),oColor.a);
	}
}
void FULL_PS8(float3 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{	
	oColor = Blend(g_samYUVA7,Tex.xy,g_vMisc7);
	//if(oColor.a < 1.0f)
	{
		float4 vColor0 = 0;
		FULL_PS7(Tex,vColor0);
		oColor = lerp(vColor0,float4(oColor.rgb,1.0f),oColor.a);
	}	
}
technique Render
{
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
	
	PASS Layer_2//P1
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS2();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;
		
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOP = Add;		

	}
 	PASS Layer_3//P2
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS3();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_4//P3
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS4();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_5//P4
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS5();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_6//P5
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS6();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_7//P6
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS7();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_8//P7
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 PS8();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}	
}
//
technique FullRender
{
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
	
	PASS Layer_2//P1
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile  ps_2_0 FULL_PS2();
		
		CullMode = None;
		ZEnable = False;
		ShadeMode = Flat;
	}
	
 	PASS Layer_3//P2
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 FULL_PS3();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	} 	
 	PASS Layer_4//P3
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 FULL_PS4();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_5//P4
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 FULL_PS5();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_6//P5
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 FULL_PS6();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_7//P6
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 FULL_PS7();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	}
 	PASS Layer_8//P7
 	{
 		VertexShader = compile vs_3_0 VS();
 		PixelShader = compile  ps_3_0 FULL_PS8();
 		
 		CullMode = None;
 		ZEnable = False;
 		ShadeMode = Flat;
 		
 		AlphaBlendEnable = True;
 		SrcBlend = SrcAlpha;
 		DestBlend = InvSrcAlpha;
 		BlendOP = Add;		
 
 	} 	
}