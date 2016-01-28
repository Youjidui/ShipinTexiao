#include "..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTex;

texture g_txColor;
texture g_txColor1;

sampler g_samColor =
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samColor1 =
sampler_state
{	
	Texture = <g_txColor1>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};

float4 rot;
#define g_fAspect	rot.z
#define g_fAlpha	rot.w

float4 g_vWave;
float4 g_vLight;
float4 g_vAlpha;

float4 g_vMisc;
#define	g_vCenter	g_vMisc.xy
#define	g_vAspect	g_vMisc.zw

#define PI	3.1415926f


void VS(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0)
{	
	oPos = mul(float4(vPos,1.f),g_matWorldViewProj);	
	oTex = vTex;	
}	

float4 PS(float2 inTex:TEXCOORD0):COLOR0
{
	float2 fTex = (inTex - 0.5f ) * 2.0f;
	
	fTex.x *= g_fAspect;
	
	float2 tex_fx = fTex * g_vAspect - g_vCenter;	
	
    float r    = length(tex_fx);
	float r_i = 1.0f / r;
    half ratio   = clamp( ( g_vWave.x - r ) * g_vWave.z, 0.0h, 1.0h );
    half angle   = PI * ratio;
    half angle_f = angle * g_vWave.w;

    half d0 = 0.5h  * g_vWave.y * ( g_vWave.w * ( cos( angle ) + 1.0h ) * sin( angle_f ) + sin( angle ) *   cos( angle_f ) );
    half d1 = 0.25h * g_vWave.y * ( g_vWave.w * ( cos( angle ) + 1.0h ) * sin( angle_f ) + sin( angle ) * ( cos( angle_f ) - 1.0h ) );
    half d = ( g_vWave.w * ratio >= 1.0h ) ? d0 : d1;

    half div = d * d + 1.0h;

    half nx = tex_fx.x * r_i * d;
    half ny = tex_fx.y * r_i * d;

    half light = ( 2.0h * ( nx * g_vLight.x + ny * g_vLight.y ) + g_vLight.z ) / div;
    light = max( light, 0.0h );
    light *= light;
    light *= light;
    light *= light;
    light *= g_vLight.w;

    //nx /= div;
    //ny /= div;

    float2 tex = fTex + float2(nx,ny) * rot.xy;
    tex.x /= g_fAspect;
    
    tex = tex / 2.0f + 0.5f;
	
	//if(tex.x < 0.0f || tex.x >= 1.0f || tex.y < 0.0f || tex.y >= 1.0f)
	//	discard;
	tex = mul(float3(tex,1.0f),g_matTex);
	
	float4 vColor0 = tex2D(g_samColor,tex);
	float4 vColor1 = tex2D(g_samColor1,inTex);
	vColor0.a *= g_vAlpha.x;
	vColor1.a *= g_vAlpha.y;
	vColor0.x += light;
		
	float4 oColor = lerp(vColor1,vColor0,lerp(1.0f,0.0f,ratio));	
	
	
	return oColor;
}

technique RippleWipe
{
	PASS RippleWipe//P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS();
		CullMode = None;
		ZEnable = False;
		AlphaBlendEnable = False;		
	}	
}

