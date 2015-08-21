#include ".\ColorSpaceFunc.INC"

float4x4	g_matWVP;
float4x4    g_matTex;

float4		g_vMisc;
#define	g_fAlpha	g_vMisc.x

//for Prenent ComposeField
texture    tx_Odd;
texture    tx_Even;
float4     vSizeTex;
///////////////////////////
texture		g_txColor;

sampler g_samColor=
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samColor_MipMap=
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samOdd=
sampler_state
{	
	Texture = <tx_Odd>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samEven =
sampler_state
{	
	Texture = <tx_Even>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VS_INPUT
{
	float4 Pos:POSITION;
	float2 Tex:TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos:POSITION;
	float2 Tex:TEXCOORD0;
};


PS_INPUT VS(VS_INPUT In)
{
	PS_INPUT Out = (PS_INPUT)0;
	//Out.Pos	= mul( In.Pos, g_matWVP );
	Out.Pos	= mul( float4(In.Pos.xy,0.0f,1.0f), g_matWVP );
	Out.Tex = mul(float3(In.Tex,1.0f),g_matTex);
	if(In.Pos.z > 1.0f)
		Out.Tex.x -= g_matTex._41;
	else if(In.Pos.z > 0.0f)
		Out.Tex.y -= g_matTex._42;
	return Out;
}

float4	PS_Channel(PS_INPUT In , uniform int nChannel):COLOR
{
	float4 oColor = tex2D(g_samColor,In.Tex);	
	oColor.r = oColor[nChannel];
			
	if(nChannel == 3) //Alpha
		oColor.r *= g_fAlpha;	
	else
		oColor.r *= oColor.a * g_fAlpha;
	
	return oColor;
}

float4	PS_Zebra(PS_INPUT In ,int2 vPos:VPOS):COLOR
{
	float4 oColor = tex2D(g_samColor,In.Tex);
	
	float fLum = CS_RGB2Y(oColor.rgb);
	if(fLum > 235.0f / 255.0f && (/*(vPos.x + vPos.y) % 10 < 1.0f ||*/ abs(vPos.x - vPos.y) % 15 < 1.0f))
		oColor = 0.5f;
	
	return oColor;
}
float4	PS_UnAverage(PS_INPUT In ,int2 vPos:VPOS ):COLOR
{
	float4 oColor = tex2D(g_samColor,In.Tex);	
	if(vPos.x % 2 > 0.5f)
		oColor.yz = tex2D(g_samColor,In.Tex - g_vMisc.xy).yz;
	
	return oColor;
}
float4	PS_DirectOut(PS_INPUT In,uniform sampler samColor):COLOR
{
	float4 oColor = tex2D(samColor,In.Tex);
	
	return oColor;
}
void PS_YUVA_BLEND_WITH_BLACK(PS_INPUT In,	
		uniform bool bCGBlend,				
		uniform bool bDiscardAlpha,
		out float4 oColor:COLOR0		
		)
{
	oColor = tex2D(g_samColor,In.Tex); 		
	
	if(bCGBlend && oColor.a > 0.50f / 255.0f)
	{		
		oColor.rgb = lerp(CS_YUVA_OFFSET,oColor,1.0f / oColor.a);
	}
	oColor.a *= g_fAlpha;
	
	if(bDiscardAlpha)
	{
		float3 vBlack = float3(16.0f / 255.0f,CS_YUVA_OFFSET.yz);	
		oColor.rgb = lerp(vBlack,oColor.rgb,oColor.a);		
		oColor.a = 1.0f;	
	}
}

void PS_ComposeField(PS_INPUT In,uniform bool bCompose,out float4 oColor:COLOR0)
{
	if(bCompose)
	{
		float y  = frac(In.Tex.y*vSizeTex.y-vSizeTex.z+0.1f);
		  
		oColor  = 0;
		if(y>0.5f)
			oColor  = tex2D( g_samEven,float2(In.Tex.x,In.Tex.y-vSizeTex.w) ); 
		else
			oColor  = tex2D( g_samOdd, In.Tex );   
	}
	else
		oColor = (tex2D( g_samOdd, In.Tex ) + tex2D( g_samEven, In.Tex )) / 2.0f;
}
void PS_AverageFrame(PS_INPUT In,out float4 oColor:COLOR0)
{	
	float4 vColor0 = tex2D( g_samOdd, In.Tex );
	float4 vColor1 = tex2D( g_samEven, In.Tex );
	
	if(g_vMisc.x > 0.5f && vColor0.a > 0.50f / 255.0f)
	{		
		vColor0.rgb = lerp(cYUVAMode,vColor0,1.0f / vColor0.a);
	}
	vColor0.a *= g_vMisc.y;
	
	if(g_vMisc.z > 0.5f && vColor1.a > 0.50f / 255.0f)
	{		
		vColor1.rgb = lerp(cYUVAMode,vColor1,1.0f / vColor1.a);
	}
	vColor1.a *= g_vMisc.w;
	
	oColor = (vColor0 + vColor1) / 2.0f;
}
void PS_CLEAR_CGBLEND(PS_INPUT input,out float4 oColor:COLOR0)
{	
	oColor = tex2D(g_samColor,input.Tex);		
		
	if(oColor.a > 0.5f / 255.0f)
	{
		oColor.rgb = lerp(CS_YUVA_OFFSET,oColor,1.0f / oColor.a);
	}	
}
technique ShowChannel
{
	pass  Y //P0
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_Channel(0);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
		
		ColorWriteEnable = Red;
	}
	pass  U //P1
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_Channel(1);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
		
		ColorWriteEnable = Red;
	}
	pass  V //P2
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_Channel(2);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
		
		ColorWriteEnable = Red;
	}
	pass  A //P3
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_Channel(3);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
		
		ColorWriteEnable = Red;
	}
}

technique ShowZebra
{
	pass  P0
	{
		VertexShader = compile vs_3_0	VS();
		PixelShader = compile ps_3_0	PS_Zebra();	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}
technique UnAverage
{
	pass  P0
	{
		VertexShader = compile vs_3_0	VS();
		PixelShader = compile ps_3_0	PS_UnAverage();	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}
technique DirectOut
{
// NO Scale
	pass No_Scale// P0
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_DirectOut(g_samColor);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
// Scale no blend
	pass  MipMap_No_Blend//P1
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_DirectOut(g_samColor_MipMap);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	pass  MipMap_Blend //P2
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_DirectOut(g_samColor_MipMap);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
	//Clear CGBlend
	pass Clear_CGBlend// P3
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_CLEAR_CGBLEND();	
		
		CullMode = None;	
	}
}
technique BlendWithBlack
{
	pass No_CGBlend_BlendWithBlack// P0
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_YUVA_BLEND_WITH_BLACK(false,true);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
		ColorWriteEnable = Red | Green | Blue;
	}
	pass CGBlend_BlendWithBlack// P1
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_YUVA_BLEND_WITH_BLACK(true,true);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
		ColorWriteEnable = Red | Green | Blue;
	}
	//
	pass No_CGBlend_COPY// P2
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_YUVA_BLEND_WITH_BLACK(false,false);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;		
	}
	pass CGBlend_COPY// P3
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_YUVA_BLEND_WITH_BLACK(true,false);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;		
	}
}

technique PresentComposeField
{
	pass  P0
	{
		VertexShader = compile vs_3_0	VS();
		PixelShader = compile ps_3_0	PS_ComposeField(true);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	pass  P1
	{
		VertexShader = compile vs_2_0	VS();
		PixelShader = compile ps_2_0	PS_ComposeField(false);	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}
technique AverageFrame
{
	pass  P0
	{
		VertexShader = compile vs_3_0	VS();
		PixelShader = compile ps_3_0	PS_AverageFrame();	
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}	
}