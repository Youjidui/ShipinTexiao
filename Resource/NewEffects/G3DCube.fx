#include "..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
//float4x4 g_matWorld;
float4x4 g_matTex;
float4x4 g_matRot;

float4	 g_vMisc;
#define  g_fDiffuse		g_vMisc.y
#define  g_fAmbient		g_vMisc.z
#define	g_fShift		g_vMisc.w

float4   g_vLightDir;
#define	 g_nTransDir    g_vLightDir.w

float4 g_vShape;
#define g_vDivide	g_vShape.xy
#define g_vInterval g_vShape.zw

float4 g_vSlideColor;

float4	g_vAlpha;
#define	g_fUpAlpha			g_vAlpha.x
#define	g_fDownAlpha		g_vAlpha.y
#define	g_fZFUNC			g_vAlpha.z

float4 g_vBackColor;

texture g_txColor;
texture g_txColor1;

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
sampler g_samColor1 =
sampler_state
{	
	Texture=<g_txColor1>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Clamp;
	AddressV=Clamp;
};
struct VS_OUTPUT {
   float4 Pos: POSITION;
   float3 Normal:TEXCOORD1;   
   float3 texCoord: TEXCOORD0;   
};

VS_OUTPUT VS(float4 Pos:POSITION,				
				float vTex:TEXCOORD0,				
				float fInstance:TEXCOORD1,
				uniform bool bClipEffect)
{	
	 VS_OUTPUT Out;
	
	int nFace = round(vTex.x);	
	float3 pos = Pos.xyz;	
	if(bClipEffect)
		pos /= float3(g_vDivide.x  ,g_vDivide.y ,g_vDivide.y);	
	else
	{
		if(g_nTransDir < 2)
			pos /= float3(g_vDivide.x  ,g_vDivide.y ,g_vDivide.y);	
		else
			pos /= float3(g_vDivide.x  ,g_vDivide.y ,g_vDivide.x);	
	}
	
	
	float fIndex = fInstance;	
	fIndex += 0.1f;	
	
	int x = fmod(fIndex  , g_vDivide.x) ;
	int y = floor(fIndex / g_vDivide.x );
	
	float3 vCenter = float3(-0.5f  + 1.0f / g_vDivide.x  * (x + 0.5f ),0.5f - 1.0f / g_vDivide.y  * (y + 0.5f),0.0f);	
	
	float3 vNormal = 0;
	
	float2 Tex = 0;
	if(nFace == 0) //FrontFace
	{
		Tex = pos.xy + vCenter.xy;
		Tex = float2(0.5f,0.50f) +float2(Tex.x,-Tex.y);
		
		vNormal = float3(0.0f,0.0f,-1.0f);
	}
	else if(nFace == 1) //Back Face
	{
		Tex = float2(pos.x,-pos.y) + vCenter.xy;		
		
		Tex = float2(0.5f,0.50f) +float2(Tex.x,-Tex.y);
		
		vNormal = float3(0.0f,0.0f,1.0f);
	}
	else if(nFace == 2 ) //top Face
	{
		Tex = pos.xy + float2(0.0f,Pos.z < 0.0f?-1.0f / g_vDivide.y : 0.0f) + vCenter.xy ;		
		Tex = float2(0.5f,0.50f) +float2(Tex.x,-Tex.y);
		
		vNormal = float3(0.0f,1.0f,0.0f);
	}
	else if(nFace == 3) //Bottom Face
	{
		Tex = pos.xy + float2(0.0f,Pos.z < 0.0f?1.0f / g_vDivide.y:0.0f);	
		Tex = float2(Tex.x,Tex.y) + vCenter.xy;		
		
		Tex = float2(0.5f,0.50f) +float2(Tex.x,-Tex.y);
		
		vNormal = float3(0.0f,-1.0f,0.0f);
	}
	else if(nFace == 4) //Left Face
	{
		if(!bClipEffect)
		{
			Tex = float2(pos.z,pos.y) + float2(-vCenter.x,vCenter.y);	
			Tex = float2(-Tex.x,Tex.y);		
			
			Tex = float2(0.5f,0.50f) +float2(Tex.x,-Tex.y);			
		}	
		vNormal = float3(-1.0f,0.0f,0.0f);
	}
	else // Right Face
	{
		if(!bClipEffect)
		{
			Tex = float2(pos.z,pos.y) + float2(vCenter.x,vCenter.y);	
			Tex = float2(Tex.x,Tex.y);		
			
			Tex = float2(0.5f,0.50f) +float2(Tex.x,-Tex.y);
		}
		
		vNormal = float3(1.0f,0.0f,0.0f);
	}	
	pos = mul(float4(pos,1.0f),g_matRot);	
	vCenter.x *= g_vMisc.x;
	pos += vCenter * float3(g_vInterval,1.0f);  
	
	if(bClipEffect)
		pos.z += 0.5f / g_vDivide.y;	
	else
	{
		if(g_nTransDir < 2)
			pos.z += 0.5f / g_vDivide.y;	
		else
			pos.z += 0.5f / g_vDivide.x;	
	}  	
			
   Out.Pos = mul(float4(pos,1.0f),g_matWorldViewProj);    
   Out.Pos.y += g_fShift * Out.Pos.w;   
		
   Out.Normal = mul(vNormal,g_matRot);   
   float fD = dot(float3(0,0,-1),normalize(Out.Normal));
   Out.Pos.z -= lerp(0,1e-3,fD) * Out.Pos.w;

   Out.texCoord.xy = mul(float3(Tex,1.0f),g_matTex);
   Out.texCoord.z = nFace ;      

   return Out;
}	

float4 PS(VS_OUTPUT In,uniform bool bLight,uniform bool bYUVA):COLOR0
{	
	float4 oColor = tex2D(g_samColor,In.texCoord.xy);	
	oColor.a *= g_fUpAlpha;
	
	if(round(In.texCoord.z) > 3.5f)
		oColor = g_vSlideColor;
		
	if(bLight)
	{
		float fCo = saturate(dot(normalize(In.Normal),g_vLightDir.xyz)) * g_fDiffuse + g_fAmbient;
		if(bYUVA)		
			oColor.rgb = lerp(float3(0.0f,0.5f,0.5f),oColor.rgb,fCo);
		else
			oColor.rgb *= fCo;
	}	
	return oColor;
}
float4 PS_TRANS(VS_OUTPUT In,uniform bool bLight,uniform bool bYUVA,uniform int nDir):COLOR0
{	
	int nFace = round(In.texCoord.z);
	
	float4 oColor = 0;
	if(nFace < 2)
	{
		oColor = tex2D(g_samColor,In.texCoord.xy);	
		oColor.a *= g_fUpAlpha;
	}
	else
	{
		if(
			(nDir < 2 && nFace < 4)
		||	(nDir > 1 && nFace > 3)		
		)
		{
			oColor = tex2D(g_samColor1,In.texCoord.xy);
			oColor.a *= g_fDownAlpha;
		}
		else
			oColor = g_vSlideColor;
	}	
	if(bLight)
	{
		float fCo = saturate(dot(normalize(In.Normal),g_vLightDir.xyz)) * g_fDiffuse + g_fAmbient;
		if(bYUVA)		
			oColor.rgb = lerp(float3(0.0f,0.5f,0.5f),oColor.rgb,fCo);
		else
			oColor.rgb *= fCo;
	}	
	return oColor;
}
////////////Compose/////////////
void  VS_COMPOSE(float4 Pos:POSITION,				
				 float2 vTex:TEXCOORD0,				
				 out float4 oPos:POSITION,
				 out float2 oTex:TEXCOORD)
{
	oPos = mul(Pos,g_matWorldViewProj);
	oTex = mul(float3(vTex,1.0f),g_matTex);
}
float4 PS_COMPOSE(float2 vTex:TEXCOORD,uniform bool bBlend):COLOR0
{
	float4 vColor0 = tex2D(g_samColor,vTex);	
	float4 vColor1 = tex2D(g_samColor1,vTex);	
	
	
	float4 oColor = cYUVAMode * (1.0f - vColor1.a) + float4( vColor1.rgb * vColor1.a,vColor1.a);
	oColor = oColor * (1.0f - vColor0.a) + float4(vColor0.rgb * vColor0.a,vColor0.a);
	
	if(oColor.a > 0.0f)
		oColor.rgb = lerp(cYUVAMode,oColor,1.0f / oColor.a);
	if(bBlend)
	{
		float4 vColor = cYUVAMode * (1.0f - g_vBackColor.a) + float4(g_vBackColor.rgb * g_vBackColor.a,g_vBackColor.a);
		oColor = vColor * (1.0f - oColor.a) + float4(oColor.rgb * oColor.a,oColor.a);
		
		if(oColor.a > 0.0f)
			oColor.rgb = lerp(cYUVAMode,oColor,1.0f / oColor.a);
	}
	return oColor;
}
technique Compose
{
	PASS Clip//P0
	{
		VertexShader=compile vs_1_1 VS_COMPOSE();
		PixelShader=compile  ps_2_0 PS_COMPOSE(false);
		CullMode=None;
		ZEnable = False;		
		AlphaBlendEnable = False;		
	}
	PASS Trans//P0
	{
		VertexShader=compile vs_1_1 VS_COMPOSE();
		PixelShader=compile  ps_2_0 PS_COMPOSE(true);
		CullMode=None;
		ZEnable = False;		
		AlphaBlendEnable = False;			
	}
}
///////////////////////
technique Picture
{
	PASS RGBA_No_light//P0
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(0,0);
		CullMode=None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;		
	}	
	PASS YUVA_NO_Light//P0
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(0,1);
		CullMode=None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;		
	}	
	PASS RGBA_light//P0
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(1,0);
		CullMode=None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;		
	}	
	PASS YUVA_Light//P0
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(1,1);
		CullMode=None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;		
	}	
}

PixelShader RGBA_No_light[4] = 
{
	compile  ps_2_0 PS_TRANS(0,0,0),
	compile  ps_2_0 PS_TRANS(0,0,1),
	compile  ps_2_0 PS_TRANS(0,0,2),
	compile  ps_2_0 PS_TRANS(0,0,3),
};
PixelShader YUVA_NO_Light[4] = 
{
	compile  ps_2_0 PS_TRANS(0,1,0),
	compile  ps_2_0 PS_TRANS(0,1,1),
	compile  ps_2_0 PS_TRANS(0,1,2),
	compile  ps_2_0 PS_TRANS(0,1,3),
};
PixelShader RGBA_light[4] = 
{
	compile  ps_2_0 PS_TRANS(1,0,0),
	compile  ps_2_0 PS_TRANS(1,0,1),
	compile  ps_2_0 PS_TRANS(1,0,2),
	compile  ps_2_0 PS_TRANS(1,0,3),
};
PixelShader YUVA_Light[4] = 
{
	compile  ps_2_0 PS_TRANS(1,1,0),
	compile  ps_2_0 PS_TRANS(1,1,1),
	compile  ps_2_0 PS_TRANS(1,1,2),
	compile  ps_2_0 PS_TRANS(1,1,3),
};
technique Trans
{
	PASS RGBA_No_light//P0
	{
		VertexShader = compile vs_1_1 VS(0);
		PixelShader = (RGBA_No_light[g_nTransDir]);
		CullMode = None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;		
	}	
	PASS YUVA_NO_Light//P1
	{
		VertexShader = compile vs_1_1 VS(0);
		PixelShader = (YUVA_NO_Light[g_nTransDir]);
		CullMode = None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;			
	}	
	PASS RGBA_light//P2
	{
		VertexShader = compile vs_1_1 VS(0);
		PixelShader = (RGBA_light[g_nTransDir]);
		CullMode = None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;			
	}	
	PASS YUVA_Light//P3
	{
		VertexShader = compile vs_1_1 VS(0);
		PixelShader = (YUVA_Light[g_nTransDir]);
		CullMode = None;
		ZEnable = True;
		ZFunc = <int(g_fZFUNC)>;
		ZWriteEnable = true;
		AlphaBlendEnable = False;		
	}	
}