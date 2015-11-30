#include"..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matPrevRotate;
float4x4 g_matRotate;
float4x4 g_matWorld;
float4x4 g_matWVP;
float4x4 g_matTexFront;
float4x4 g_matTexRear;

float3 g_vLightDir = float3(-1.0f,0.0f,-1.0f);
float3 g_vEyeDir = float3(1.0f,0.0f,-1.0f);

float4 g_vMatterColor;
float4 g_vHightLight;
#define g_fFrontPower  g_vHightLight.x
#define g_fFrontCo	   g_vHightLight.y
#define g_fRearPower  g_vHightLight.z
#define g_fRearCo	   g_vHightLight.w
float	g_fDiffuseCo;

float4  g_vAlpha;
#define g_fFrontAlpha	g_vAlpha.x
#define g_fRearAlpha	g_vAlpha.y

float4 g_vMisc;
#define  g_fProcess			g_vMisc.x
#define  g_fWrapRadius		g_vMisc.y
#define  g_fMatterRatio		g_vMisc.z
#define  g_fUseGround		g_vMisc.w

texture g_txFrontPicture;
texture g_txRearPicture;

sampler g_samFrontPicture=
sampler_state
{	
	Texture=<g_txFrontPicture>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Clamp;
	AddressV=Clamp;
};

sampler g_samRearPicture=
sampler_state
{	
	Texture=<g_txRearPicture>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Clamp;
	AddressV=Clamp;
};
#define PI 3.1415926f
#define PI3 (PI * 3.0f)
#define PI_2 (PI / 2.0f)
#define PI_15 (PI * 1.5f)

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float3 texFront:  TEXCOORD0;  
   float3 texRear:  TEXCOORD1;  
   float3 normal	:TEXCOORD2;  
   float3 lightdir	:TEXCOORD3;  
   float3 eyedir	:TEXCOORD4;     
};

VS_OUTPUT VS(float4 Pos:POSITION,
				uniform bool bPageRoll)
{	
	 VS_OUTPUT Out = (VS_OUTPUT)0;
	 
	float3 pos = 0;
	pos.xy = Pos.xy;
// 	pos.x *= g_fAspect;
// 	pos.xy = clamp(mul(float4(pos.xy * sqrt(1 + g_fAspect * g_fAspect),0.0f,1.0f),g_matPrevRotate),float2(-0.5f * g_fAspect,-0.5f),float2(0.5f * g_fAspect,0.5f));
// 	
// 	float2 vTex = pos / float2(g_fAspect, 1.0f)+ 0.5f;
	
	float2 vTex = pos + 0.5f;
	
	pos.xy = mul(float4(pos,1.0f),g_matRotate).xy;
	
	Out.texFront.z = 0.0f;	
	
	pos.z = g_fWrapRadius;	
	
	float fArcLen = g_fProcess - pos.x;
	float fZ = 0.99f;
	float3 normal = float3(-fArcLen,0.0f,g_fWrapRadius);
	if(fArcLen > 0 )
	{		
		float fTheta = fArcLen / g_fWrapRadius;
		if(bPageRoll) //พําณ
		{			
			fTheta = clamp(fTheta,0,PI3);				
			fZ = lerp(0.0f,0.47f,1.0f - fTheta / PI_15);	
			if(fZ < 0.0f)
				fZ = 0.95f + fZ;			
		}
		else //ีÛาณ
		{				
			fTheta = clamp(fTheta,0,PI);
			fZ = 0.85 - fArcLen / 2.0f;		
		}
		
		fTheta += PI_2;
		float s,c;
		sincos(fTheta,s,c);
		
		normal = float3(c * g_fWrapRadius,0.0f,s * g_fWrapRadius);
		
		Out.texFront.z = smoothstep(radians(90.0f),radians(135.0f),fTheta);
		
		if(bPageRoll)  //พําณ
			pos.x = c * g_fWrapRadius + g_fProcess ;
		else //ีÛาณ		
		{
			pos.x = c * g_fWrapRadius + g_fProcess + max(0,fArcLen - g_fWrapRadius * PI);
			
			float fOverLen = max(0,fArcLen - g_fWrapRadius * PI);
			pos.x = c * g_fWrapRadius + g_fProcess + fOverLen;
			
			if(fTheta > radians(200.0f))
				Out.texFront.z =1.0f -  smoothstep(radians(225.0f),radians(270.0f),fTheta);
			if(fOverLen > 0)
			{
				 normal = float3( fOverLen ,0.0f,-g_fWrapRadius);
			}
		}
		
		pos.z = s * g_fWrapRadius ; 
	}
	
		
	Out.normal = mul(normal,g_matWorld);
	Out.lightdir = mul(g_vLightDir,g_matWorld);	
	Out.eyedir = mul(g_vEyeDir,g_matWorld);
		
   Out.Pos = mul(float4(pos,1.0f),g_matWVP);
   vTex.y = 1.0f - vTex.y;
   Out.texFront.xy = mul( float3(vTex,1.0f),g_matTexFront);
   Out.texRear.xy = mul( float3(vTex,1.0f),g_matTexRear);
   Out.Pos.xy /= Out.Pos.w;
   Out.Pos.w = 1.0f;
   Out.Pos.z = fZ;
   return Out;
}

float4 PS(VS_OUTPUT In,float fFace:VFACE,uniform int nFmt):COLOR0
{
	float4 fColor=(float4)0;
	
	float3 vReflectDir = normalize(reflect(normalize(In.lightdir),normalize(In.normal)));
	float3 vViewDir = normalize(In.eyedir);
	float se = 0.0f;
		
	float fDiffuse = dot(normalize(In.lightdir),normalize(In.normal));
	if(In.texFront.z < 1.0f)
		fDiffuse = max(fDiffuse,lerp(1.0f,fDiffuse,In.texFront.z));
	fDiffuse = max(0,lerp(1.0f,fDiffuse,g_fDiffuseCo));
	
	if(fFace < 0.5)	//back
	{
		if(g_fUseGround)
		{
			fColor = tex2D(g_samFrontPicture,In.texFront);
			fColor.a *= g_fFrontAlpha;
		}
		else  if(g_fMatterRatio < 1.0f)
		{
		  fColor = tex2D(g_samRearPicture,In.texRear);
		  fColor.a *= g_fRearAlpha;
		}
		fColor =  lerp(fColor,g_vMatterColor,g_fMatterRatio);
		
		if(g_fRearPower * g_fRearCo > 0.0f)
			se = pow(max(0,dot(-vReflectDir,vViewDir)),g_fRearPower) * g_fRearCo;
	}
	else //front
	{	
		fColor=tex2D(g_samFrontPicture,In.texFront);	
		fColor.a *= g_fFrontAlpha;
			
		if(g_fFrontPower * g_fFrontCo > 0.0f)
			se = pow(max(0,dot(-vReflectDir,vViewDir)),g_fFrontPower) * g_fFrontCo;	
	}
	se *= In.texFront.z;
	se = max(0,se);
	
	if(nFmt == 0) //yuva	
	{
		fColor.rgb -= CS_YUVA_OFFSET;
		
		fColor.rgb = ColorSpace_YUVToRGB(fColor.rgb);
		fColor.rgb *= fDiffuse;
		fColor.rgb += se;
		fColor.rgb = ColorSpace_RGBToYUV(fColor.rgb);
		
		fColor.rgb += CS_YUVA_OFFSET;
	}
	else // rgba
	{
		fColor.rgb *= fDiffuse;
		fColor += se;
	}	
	return fColor;
}


technique PageRoll
{
	PASS yuva //P0
	{
		VertexShader=compile vs_3_0 VS(1);
		PixelShader=compile  ps_3_0 PS(0);
		CullMode=None;
		ZEnable = True;
		ZFunc = Less;
		ZWriteEnable = True;
		//FillMode = WireFrame;	
		StencilEnable = True;
		StencilFunc = Always;
		StencilPass = Incr;	
	}
	PASS rgba //P1
	{
		VertexShader=compile vs_3_0 VS(1);
		PixelShader=compile  ps_3_0 PS(1);
		CullMode=None;
		ZEnable = True;
		ZFunc = Less;
		ZWriteEnable = True;
		//FillMode = WireFrame;	
		StencilEnable = True;
		StencilFunc = Always;
		StencilPass = Incr;	
	}
}
technique PageTurn
{
	PASS yuva //P0
	{
		VertexShader=compile vs_3_0 VS(0);
		PixelShader=compile  ps_3_0 PS(0);
		CullMode=None;
		ZEnable = True;
		ZFunc = LessEqual;
		ZWriteEnable = True;
		//FillMode = WireFrame;
		StencilEnable = True;
		StencilFunc = Always;
		StencilPass = Incr;	
	}
	PASS rgba //P1
	{
		VertexShader=compile vs_3_0 VS(0);
		PixelShader=compile  ps_3_0 PS(1);
		CullMode=None;
		ZEnable = True;
		ZFunc = LessEqual;
		ZWriteEnable = True;
		//FillMode = WireFrame;
		StencilEnable = True;
		StencilFunc = Always;
		StencilPass = Incr;	
	}
}

