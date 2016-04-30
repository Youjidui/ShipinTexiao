#include "..\Shaders\ColorSpaceFunc.INC"
float4x4 g_matWorldViewProj;
float4x4 g_matTexture;

float3	 g_mix;
float2	 g_bAlpha;
float2	 g_fAlphaBuffer;

float g_subtrcontr;
float g_ftime;

float3	colorBlack;

texture g_txColor0;
texture g_txAlpha0;
texture g_txColor1;
texture g_txAlpha1;

sampler g_samColor0=
sampler_state
{	
	Texture=<g_txColor0>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Wrap;
	AddressV=Wrap;
};

sampler g_samAlpha0=
sampler_state
{	
	Texture=<g_txAlpha0>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Wrap;
	AddressV=Wrap;
};

sampler g_samColor1=
sampler_state
{	
	Texture=<g_txColor1>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Wrap;
	AddressV=Wrap;
};

sampler g_samAlpha1=
sampler_state
{	
	Texture=<g_txAlpha1>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Wrap;
	AddressV=Wrap;
};

void VertScene(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	
	oPos=mul(float4(vPos,1.f),g_matWorldViewProj);
	oTex=mul(float4(vTex,1.0f,0.0f),g_matTexture);		
}	

float4 PixSceneRGBA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 color0 = tex2D(g_samColor0,Tex);
	float4 color1 = tex2D(g_samColor1,Tex);
	color0.a *= g_fAlphaBuffer.x;
	color1.a *= g_fAlphaBuffer.y;	
	
	if( g_mix.z<0.5f )
		return color0*g_mix.x+color1*g_mix.y;
	else
	{
		// noadditive mix
		color0 *= float4(g_mix.xxx,1.f);
		color1 *= float4(g_mix.yyy,1.f);
				
		//float3 rgbToY = float3(0.222f, 0.707f, 0.071f);
		float Y0 = CS_RGB2Y(color0.rgb);//dot(color0.rgb,rgbToY);
		float Y1 = CS_RGB2Y(color1.rgb);//dot(color1.rgb,rgbToY);
		//return (Y0>=Y1)?color0:color1;	
		return lerp( color0, color1, step(Y0,Y1) );		
	}
}

float4 PixSceneYUVA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 color0 = tex2D(g_samColor0,Tex)-float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
	float4 color1 = tex2D(g_samColor1,Tex)-float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
	
	color0.a *= g_fAlphaBuffer.x;
	color1.a *= g_fAlphaBuffer.y;	
	
	if( !any(color0.a) )
		color0.rgb = colorBlack;
	if( !any(color1.a) )
		color1.rgb = colorBlack;
	
	/*if( g_mix.z<0.5f)
		return color0*g_mix.x+color1*g_mix.y+float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
	else
	{
		// noadditive mix
		color0 = color0*float4(g_mix.xxx,1.f)+float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
		color1 = color1*float4(g_mix.yyy,1.f)+float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
		//return (color0.x>=color1.x)?color0:color1;			
		return lerp( color0, color1, step(color0.x,color1.x) );		

	}*/
	    if( g_mix.z<0.5f)
	    {
			float4 color2;
            color2.rgb = color0*g_mix.x*color0.a+color1*g_mix.y*color1.a;
            color2.a = saturate(color0.a*g_mix.x+color1.a*g_mix.y);
            color2.rgb =  color2 / color2.a;
            return color2 + float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
		}
        else
        {
            // noadditive mix
   			float4 color2;
            color0.rgb = color0*g_mix.x*color0.a;
            color0.a = color0.a*g_mix.x;
            color1.rgb = color1*g_mix.y*color1.a;
            color1.a = color1.a*g_mix.y;
            color2 = (color0.r>color1.r)?color0:color1;
            color2.rgb = color2 / color2.a;
            return color2 + float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
        }
}

float4 PixSceneYUYV_NA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 color0 = tex2D(g_samColor0,Tex)-float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
	float4 color1 = tex2D(g_samColor1,Tex)-float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
	
	if( g_mix.z<0.5f)
		return color0*g_mix.x+color1*g_mix.y+float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
	else
	{
		// noadditive mix
		color0 = color0*float4(g_mix.xxx,1.f)+float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
		color1 = color1*float4(g_mix.yyy,1.f)+float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
		//return (color0.x>=color1.x)?color0:color1;			
		return lerp( color0, color1, step(color0.x,color1.x) );		
	}
}

void PixSceneYUYV_AP(float2 Tex:TEXCOORD0,
						out float4 oColor:COLOR0,
						out float4 oAlpha:COLOR1)
{

	float4 color0 = tex2D(g_samColor0,Tex)-float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
	float4 color1 = tex2D(g_samColor1,Tex)-float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);

	float4	alpha0 = (g_bAlpha.x>0.5f)?tex2D(g_samAlpha0,Tex):1.f;
	float4	alpha1 = (g_bAlpha.y>0.5f)?tex2D(g_samAlpha1,Tex):1.f;	
	alpha0 *= g_fAlphaBuffer.x;
	alpha1 *= g_fAlphaBuffer.y;	
	
	if( g_mix.z<0.5f)
	{
		oColor = color0*g_mix.x+color1*g_mix.y+float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
		oAlpha = alpha0*g_mix.x+alpha1*g_mix.y;
	}
	else
	{
		// noadditive mix
		color0 = color0*float4(g_mix.xxx,1.f)+float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
		color1 = color1*float4(g_mix.yyy,1.f)+float4(0.f,CS_UV_OFFSET,0.f,CS_UV_OFFSET);
		float selFactor = step(color0.x,color1.x);
// 		oColor = (color0.x>=color1.x)?color0:color1;		
// 		oAlpha = (color0.x>=color1.x)?alpha0:alpha1;	
		oColor = lerp(color0,color1,selFactor);
		oAlpha = lerp(alpha0,alpha1,selFactor);
	}

}

float4 PixSceneSubtraction(float2 Tex:TEXCOORD0):COLOR0
{
	float4 color0 = tex2D(g_samColor0,Tex);
	float4 color1 = tex2D(g_samColor1,Tex);
	
	color0.a *= g_fAlphaBuffer.x;
	color1.a *= g_fAlphaBuffer.y;	
	
	float4 tempr2 = color0 + color1 - color0 * color1 / g_subtrcontr;
	float p1 = max(0.f, 1.f - g_mix.z * 2.f);
	float p2 = min(g_mix.z * 2.f, 2.f - g_mix.z * 2.f);
	float p3 = max(0, g_mix.z * 2.f - 1.f);
	float4 tempr3 = color0 * p1 + tempr2 * p2 + color1 * p3;
	
	float4 tempr4 = saturate(tempr3);
	tempr4.a = color0.a * (1 - g_mix.z) + color1.a * g_mix.z;
    return tempr4;
		
}

float4 PixSceneSubtractionYUVA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 color0 = tex2D(g_samColor0,Tex) - CS_YUVA_OFFSET;
	float4 color1 = tex2D(g_samColor1,Tex) - CS_YUVA_OFFSET;
	
	color0.a *= g_fAlphaBuffer.x;
	color1.a *= g_fAlphaBuffer.y;	
	
	color0 = CS_YUVA2RGBA(color0);
	color1 = CS_YUVA2RGBA(color1);
	
// 	color0.rgb *= color0.a;
// 	color1.rgb *= color1.a;		
	
	float4 tempr2 = color0 + color1 - color0 * color1 / g_subtrcontr;
	float p1 = max(0.f, 1.f - g_mix.z * 2.f);
	float p2 = min(g_mix.z * 2.f, 2.f - g_mix.z * 2.f);
	float p3 = max(0, g_mix.z * 2.f - 1.f);
	float4 tempr3 = color0 * p1 + tempr2 * p2 + color1 * p3;
	
	float4 tempr4 = saturate(tempr3);
	tempr4.a = color0.a * (1 - g_mix.z) + color1.a * g_mix.z;
	tempr4 = CS_RGBA2YUVA(tempr4);
    return tempr4+ CS_YUVA_OFFSET;
		
}

float4 PixSceneSandStorm(float2 Tex:TEXCOORD0):COLOR0
{
	float4 color0 = tex2D(g_samColor0,Tex);
	float4 color1 = tex2D(g_samColor1,Tex);
	
	color0.a *= g_fAlphaBuffer.x;
	color1.a *= g_fAlphaBuffer.y;	
	
	float g_randnum = GPURand(Tex, g_ftime + 0.6);
	
	float4 p1 = max(0,min(1,g_randnum + g_mix.z * 2 - 1));
	
	float4 descolor = min(1,max(0,(color1 * p1 + color0 * (1 - p1)))/* * 255 / 256 */);
	return descolor;
}

float4 PixSceneSandStormYUVA(float2 Tex:TEXCOORD0):COLOR0
{
	float4 color0 = tex2D(g_samColor0,Tex)-float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
	float4 color1 = tex2D(g_samColor1,Tex)-float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
	
	color0.a *= g_fAlphaBuffer.x;
	color1.a *= g_fAlphaBuffer.y;	
	
	color0 = CS_YUVA2RGBA(color0);
	color1 = CS_YUVA2RGBA(color1);
	
// 	color0.rgb *= color0.a;
// 	color1.rgb *= color1.a;

	float g_randnum = GPURand(Tex, g_ftime + 0.6);
	
	float4 p1 = max(0,min(1,g_randnum + g_mix.z * 2 - 1));
	
	float4 descolor = min(1,max(0,(color1 * p1 + color0 * (1 - p1)))/* * 255 / 256 */);
	descolor = CS_RGBA2YUVA(descolor);
	return descolor + float4(0.f,CS_UV_OFFSET,CS_UV_OFFSET,0.f);
}

////////////////////////////////////////////////////////////////////////////

technique RenderRGBA
{
	PASS P0
	{
		VertexShader = compile vs_3_0	VertScene();
		PixelShader = compile ps_3_0	PixSceneRGBA();	
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

technique RenderYUYV_NA
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0	PixSceneYUYV_NA();
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

technique RenderYUYV_AP
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0	PixSceneYUYV_AP();
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

technique RenderYUVA
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0	PixSceneYUVA();
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

technique RenderSubtractionRGBA
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0	PixSceneSubtraction();
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

technique RenderSubtractionYUVA
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0	PixSceneSubtractionYUVA();
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

technique RenderSandStormRGBA
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0	PixSceneSandStorm();
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

technique RenderSandStormYUVA
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VertScene();
		PixelShader = compile ps_3_0	PixSceneSandStormYUVA();
		CullMode=None;
		AlphaBlendEnable = False;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}
