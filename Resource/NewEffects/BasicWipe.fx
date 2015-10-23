#include "..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTexture;
float4x4 g_matMask;

float4	 g_vMisc;
#define  g_fBorderWidth  g_vMisc.y
#define  g_fSoftWidth    g_vMisc.z
#define  g_bInvert       (g_vMisc.w > 0.5f)

float4   g_vBorderColor;

float4	g_vDivideMisc;

float4 g_vMultiple;
#define g_vNumBase g_vMultiple.xy
#define g_vNumCalc g_vMultiple.zw

float4 g_vMultipleMisc;
#define g_fFlip			g_vMultipleMisc.x
#define g_fOverlap		g_vMultipleMisc.y
float4   g_vTexCoordFlip;

float4 g_vTransMisc;
#define g_fAlpha0		g_vTransMisc.x
#define g_fAlpha1		g_vTransMisc.y
#define g_fHasAlpha0	g_vTransMisc.z
#define g_fHasAlpha1	g_vTransMisc.w

bool	g_bAlphaBlend;

texture g_txColor;
texture g_txColor1;
texture g_txAlpha;
texture g_txAlpha1;
texture g_txMask;
texture g_txDivide;

sampler g_samColor = 
sampler_state
{	
	Texture = <g_txColor>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samColor1 = 
sampler_state
{	
	Texture = <g_txColor1>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samAlpha = 
sampler_state
{	
	Texture = <g_txAlpha>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samAlpha1 = 
sampler_state
{	
	Texture = <g_txAlpha1>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samMask = 
sampler_state
{	
	Texture = <g_txMask>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler g_samDivide = 
sampler_state
{	
	Texture = <g_txDivide>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VS_INPUT
{
	float3 vPos:POSITION;
	float2 vTexCoord:TEXCOORD0;
};
struct VS_INSTANCE_INPUT
{
	float3 vPos:POSITION;
	float2 vTexCoord:TEXCOORD0;
	float  fIndex:TEXCOORD1;
};
struct PS_MASK_INPUT
{
	float4 vPos:POSITION;	
	float3 vTex:TEXCOORD0;
};
struct PS_INPUT
{
	float4 vPos:POSITION;
	float2 vTex:TEXCOORD0;
	float2 vMask:TEXCOORD1;
};

struct PS_DIVIDE_INPUT
{
	float4 vPos:POSITION;
	float2 vTex:TEXCOORD0;
	float2 vTexMirror:TEXCOORD1;
	float2 vMask:TEXCOORD2;	
};


////////////////////Msk////////////////////////////
//Divide
PS_DIVIDE_INPUT VS_DIVIDE(VS_INPUT input,uniform  int nType)
{	
	PS_DIVIDE_INPUT output  =  (PS_DIVIDE_INPUT) 0;
	
	float2 pos = input.vPos.xy;
	float2 vTex = input.vTexCoord;
	if(nType == 1) // hor Mirror
	{
		vTex.x = g_vDivideMisc.y - vTex.x;		
	}
	else if(nType == 2) // Vert Mirror
	{
		vTex.y = g_vDivideMisc.z - vTex.y;		
		vTex.y -= g_vDivideMisc.x;
	}
	else if(nType == 3) // hor_vert Mirror
	{
		vTex.x = g_vDivideMisc.y - vTex.x;
		vTex.y = g_vDivideMisc.z - vTex.y;		
		vTex.y -= g_vDivideMisc.x;
	}
	output.vPos = mul(float4(pos,0.0f,1.f),g_matWorldViewProj);
	output.vTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexture);					
	output.vTexMirror = mul(float4(vTex,1.0f,0.0f),g_matTexture);					
	output.vMask = mul(float4(input.vTexCoord,1.0f,0.0f),g_matMask);					
	return output;
}
float4 PS_DIVIDE(PS_DIVIDE_INPUT input,				
				uniform bool bHor
			):COLOR0
{	
	float Divide = tex1D(g_samDivide,bHor? input.vMask.y : input.vMask.x ).r;
	
	float4 oColor = 0;
	
	if(any(Divide))			
		oColor = tex2D(g_samColor,input.vTexMirror);				
	else
		oColor = tex2D(g_samColor,input.vTex);	   
	
	if(any(oColor.a) && !any(oColor.rgb))
		discard;	
	return oColor;
}
//Multiple
PS_MASK_INPUT VS_Multiple(VS_INSTANCE_INPUT input,uniform  int nType)
{
	PS_MASK_INPUT output  =  (PS_MASK_INPUT) 0;
	
	float2 vTex = input.vTexCoord;
	float2 pos = input.vPos.xy;
	float fIndex = input.fIndex;
	fIndex = g_vNumCalc.x * g_vNumCalc.y - 1.0f - fIndex;
	fIndex += 0.1f;
	
	if(!any(g_fOverlap))
	{
		pos = float2(g_vNumBase.x  ,g_vNumBase.y) * pos;			
	}
	
	int x = fmod(fIndex  , g_vNumCalc.x) ;
	int y = floor(fIndex / g_vNumCalc.x );	
	
	float2 vCenter =0;
	if(nType == 0) //Normal
		vCenter = float2(-0.5f + g_vNumBase.x  * (x + 0.50f),0.5f - g_vNumBase.y  * (y + 0.50f));
	else if(nType == 1) //Hor Shift
		vCenter = float2(-0.5f + g_vNumBase.x  * (x + (fmod(y + 0.1f ,2)>0.5f? 0.50f:0.0f)),0.5f - g_vNumBase.y  * (y + 0.5f));
	else if(nType == 2) //Vert Shift
		vCenter = float2(-0.5f + g_vNumBase.x  * (x + 0.5f),0.5f - g_vNumBase.y  * (y + (fmod(x + 0.1f ,2)>0.5f? 0.5f:0.0f)));	
	
	if(any(g_fFlip))
	{
		if(fmod(x + 0.1f ,2) > 0.5f)
		{
			pos.x = - pos.x;
			vTex.x -= g_vTexCoordFlip.x;
		}
		if(fmod(y + 0.1f ,2) > 0.5f)
		{
			pos.y = - pos.y;
			//pos.y += g_vMultipleMisc.z;
			vTex.y -= g_vTexCoordFlip.y;
			
		}	
		//pos.y += g_vMultipleMisc.w * sign(pos.y);		
	}
	
	pos += vCenter ;	

   	output.vPos = mul(float4(pos,0.0f,1.f),g_matWorldViewProj);
	output.vTex.xy = mul(float4(vTex,1.0f,0.0f),g_matTexture);					
	output.vTex.z = 0.95f /*-  (y  + 1.0 - vTex.y)  * 0.04f*/;
						
	return output;
}
void PS_Multiple_Overlap(PS_MASK_INPUT In,out float4 oColor:COLOR0,out float depth:DEPTH)
{
	oColor = tex2D(g_samColor,In.vTex);	
	
 	if(oColor.a > 0.0f && !any(oColor.rgb))
 		discard; 			
			
	depth = 0.9f;

	if(oColor.a < 1.0f / 255.0f)
	{
		depth = 0.1f;		
	}
	else if(oColor.g > 0.0f)
		depth = 0.2f + oColor.a * 0.1f;
	else if(oColor.r > 0.0f)
		depth = 0.4f + oColor.r * 0.1f;
	else if(oColor.b > 0.0f)
		depth = 0.7f - oColor.a * 0.1f;
	
}
void PS_Multiple_Genereal(PS_MASK_INPUT In,out float4 oColor:COLOR0)
{
	oColor = tex2D(g_samColor,In.vTex);	
	
	if(oColor.a > 0.0f && !any(oColor.rgb))
		discard;	
}
///////////////////////////Picture//////////////////////////////////
//Vs
PS_INPUT VS_COLOR(VS_INPUT input)
{	
	PS_INPUT output  =  (PS_INPUT) 0;
	output.vPos = mul(float4(input.vPos.xy,0.0f,1.f),g_matWorldViewProj);
	output.vTex = mul(float4(input.vTexCoord,1.0f,0.0f),g_matTexture);		
	output.vMask = (output.vPos.xy / output.vPos.w + 1.0f) / 2.0f;
	output.vMask.y = 1.0f - output.vMask.y;
	 output.vMask = mul(float4(output.vMask,1.0f,0.0f),g_matMask);	
			
	//output.vMask = mul(float4(input.vTexCoord,1.0f,0.0f),g_matMask);		
	return output;
}
//Ps
void Compose_S(float4 vColor,float4 vMask,bool bBorder,bool bInvert,out float4 oColor)
{
	if(bInvert )
	{
		vMask = vMask.rbga;
		
		if( (any(vMask.rgb) == 0 || g_fBorderWidth <= 0.0f))
			vMask.a = 1.0f - vMask.a;		
	}
	
	if(vMask.a <= 0.0f)
		discard;			
	
	oColor = float4(vColor.rgb,vMask.a * vColor.a);	
	
	if(bBorder)
	{		
		if( vMask.b > 0.0f )
		{
			oColor = lerp(vColor,g_vBorderColor,vMask.a);
			if(vMask.r > 0.0f)
				oColor.a *=  vMask.r;
		}
		else if(vMask.g > 0.0f)
		{			
			if(vMask.r > 0.0f)
			{
				oColor= lerp(vColor, g_vBorderColor,vMask.r);
				oColor.a *= vMask.a;
			}
			else 
				oColor = float4(g_vBorderColor.rgb,vMask.a);				
		}
		else if(vMask.r > 0.0f)
		{
			oColor = g_vBorderColor;
		} 			
	}
}

void PS_YUYV_AP(PS_INPUT input,
				out float4 oColor:COLOR0,
				out float4 oAlpha:COLOR1)
{
	float4 vMask = tex2D(g_samMask,input.vMask);
	float4 vAlpha = tex2D(g_samAlpha,input.vTex);
	float4 vColor = tex2D(g_samColor,input.vTex);
	
	float4 vYUVA0,vYUVA1;
	Compose_S(float4(vColor.bga,vAlpha.b),vMask,g_fBorderWidth > 0.0f,g_bInvert,vYUVA0); 
	Compose_S(float4(vColor.rga,vAlpha.r),vMask,g_fBorderWidth > 0.0f,g_bInvert,vYUVA1); 		
	
	oColor = float4(vYUVA1.r,vYUVA0.g,vYUVA0.r,vYUVA0.b);
	oAlpha = float4(vYUVA1.rr,vYUVA0.rr);
}
void PS_YUYV_NA(PS_INPUT input,
				out float4 oColor:COLOR0,
				out float4 oAlpha:COLOR1)
{
	float4 vMask = tex2D(g_samMask,input.vMask);	
	float4 vColor = tex2D(g_samColor,input.vTex);
	
	float4 vYUVA0,vYUVA1;
	Compose_S(float4(vColor.bga,1.0f),vMask,g_fBorderWidth > 0.0f,g_bInvert,vYUVA0); 
	Compose_S(float4(vColor.rga,1.0f),vMask,g_fBorderWidth > 0.0f,g_bInvert,vYUVA1); 		
	
	oColor = float4(vYUVA1.r,vYUVA0.g,vYUVA0.r,vYUVA0.b);
	oAlpha = float4(vYUVA1.rr,vYUVA0.rr);		
}

void PS_YUVA(PS_INPUT input,
			 uniform bool bBorder,
			 uniform bool bInvert,
			 out float4 oColor:COLOR0)
{
	float4 vMask = tex2D(g_samMask,input.vMask);	
	float4 vColor = tex2D(g_samColor,input.vTex);
	
	Compose_S(vColor,vMask,bBorder,bInvert,oColor);  						
}
//////////////Trans//////////////////////////////////
void Compose(float4 vMask,float4 vColorL,float4 vColorR,bool bHasBorder,out float4 oColor)
{
	if(bHasBorder)
	{
		float fAlpha = 1.0f;
		float4 vBorderColor = g_vBorderColor;		
		
		if(vMask.b > 0.0f) //L
		{
			oColor = lerp(vColorL,vBorderColor,vMask.a);
			if(any(vMask.r))
				oColor = lerp(vColorR,oColor,vMask.r);			
		} 
		else if(vMask.g >0.0f ) //R
		{			
			if(vMask.r > 0.0f)
				oColor = lerp(vColorL,vBorderColor,vMask.r);
			else
			{
				oColor = vBorderColor;
				//oColor.a = vColorL.a;
			}
			
			oColor = lerp(vColorR,oColor,vMask.a);			
		}
		else if(vMask.r > 0.0f )
			oColor = vBorderColor;	
		else
			oColor = lerp(vColorR,vColorL,vMask.a);
	}
	else
		oColor = lerp(vColorR,vColorL,vMask.a);
}
void PS_YUVA_TRANS(PS_INPUT input,
			 uniform bool bHasBorder,
			 out float4 oColor:COLOR0)
{
	float4 vColorL = 0.0f,vColorR = 0.0f;
	
	float4 vMask = tex2D(g_samMask,input.vMask);	
	if(any(vMask))
	{
		if(any(vMask.rgb))
		{
			vColorL = tex2D(g_samColor,input.vTex);
			vColorR = tex2D(g_samColor1,input.vTex);
			
			vColorL.a *= g_fAlpha0;
			vColorR.a *= g_fAlpha1;
	
			Compose(vMask,vColorL,vColorR,bHasBorder,oColor);						
		}
		else
		{
			vColorL = tex2D(g_samColor,input.vTex);
			vColorL.a *= g_fAlpha0;	
			oColor = vColorL;	
		}
	}
	else
	{ 	   
	   vColorR = tex2D(g_samColor1,input.vTex);
	   vColorR.a *= g_fAlpha1;
	   oColor = vColorR;
	}  		
}
void PS_YUYV_TRANS(PS_INPUT input,			 
			 out float4 oColor:COLOR0,
			 out float4 oAlpha:COLOR1)
{
	float4 vMask = tex2D(g_samMask,input.vMask);	
	float4 vColorL = tex2D(g_samColor,input.vTex);
	float4 vColorR = tex2D(g_samColor1,input.vTex);		
	float4 vAlphaL = g_fHasAlpha0 > 0 ? tex2D(g_samAlpha,input.vTex) : 1.0f;
	float4 vAlphaR = g_fHasAlpha1 > 0 ? tex2D(g_samAlpha1,input.vTex) : 1.0f;	
	
	vAlphaL *= g_fAlpha0;
	vAlphaR.a *= g_fAlpha1;
	
	float4 vColor0,vColor1;
	Compose(vMask,float4(vColorL.bga,vAlphaL.b),float4(vColorR.bga,vAlphaR.b),g_fBorderWidth > 0.0f,vColor0);
	Compose(vMask,float4(vColorL.rga,vAlphaL.r),float4(vColorR.rga,vAlphaR.r),g_fBorderWidth > 0.0f,vColor1);
	oColor = float4(vColor1.r,vColor0.g,vColor0.r,vColor0.b);
	oAlpha = float4(vColor1.aa,vColor0.aa);
}
technique Multiple
{
//No Overlap
	PASS Normal//P0
	{
		VertexShader = compile vs_3_0 VS_Multiple(0);
		PixelShader = compile  ps_3_0 PS_Multiple_Genereal();
		CullMode = None;			
		ZEnable  =  False;		
		AlphaBlendEnable = False; 		
	}
	PASS Hor_Shift//P1
	{
		VertexShader = compile vs_3_0 VS_Multiple(1);
		PixelShader = compile  ps_3_0 PS_Multiple_Genereal();
		CullMode = None;			
		ZEnable  =  False;		
		AlphaBlendEnable = False; 	
	}
	PASS Vert_Shift//P2
	{
		VertexShader = compile vs_3_0 VS_Multiple(2);
		PixelShader = compile  ps_3_0 PS_Multiple_Genereal();
		CullMode = None;			
		ZEnable  =  False;		
		AlphaBlendEnable = False; 	
	}
	// Overlap
	PASS Normal//P3
	{
		VertexShader = compile vs_3_0 VS_Multiple(0);
		PixelShader = compile  ps_3_0 PS_Multiple_Overlap();
		CullMode = None;			
		ZEnable  =  True;
		ZFunc    = LessEqual;
		ZWriteEnable = True;
		AlphaBlendEnable = False;			
	}
	PASS Hor_Shift//P4
	{
		VertexShader = compile vs_3_0 VS_Multiple(1);
		PixelShader = compile  ps_3_0 PS_Multiple_Overlap();
		CullMode = None;			
		ZEnable  =  True;
		ZFunc    = Less;
		ZWriteEnable = True;
	}
	PASS Vert_Shift//P5
	{
		VertexShader = compile vs_3_0 VS_Multiple(2);
		PixelShader = compile  ps_3_0 PS_Multiple_Overlap();
		CullMode = None;			
		ZEnable  =  True;
		ZWriteEnable = True;
		ZFunc    = Less;
	}
	
}	
technique Divide
{

	PASS Hor_Mirror//P0
	{
		VertexShader = compile vs_3_0 VS_DIVIDE(1);
		PixelShader = compile  ps_3_0 PS_DIVIDE(1);
		CullMode = None;			
		ZEnable  =  False;
//		AlphaBlendEnable = True;
//		SrcBlend = One;
//		DestBlend = One;
//		BlendOp = Add;
		//FillMode  =  WireFrame;
	}
	PASS Hor_Mirror_Flip//P1
	{
		VertexShader = compile vs_3_0 VS_DIVIDE(3);
		PixelShader = compile  ps_3_0 PS_DIVIDE(1);
		CullMode = None;			
		ZEnable  =  False;
//		AlphaBlendEnable = True;
//		SrcBlend = One;
//		DestBlend = One;
//		BlendOp = Add;
		//FillMode  =  WireFrame;
	}

	PASS Vert_Mirror//P3
	{
		VertexShader = compile vs_3_0 VS_DIVIDE(2);
		PixelShader = compile  ps_3_0 PS_DIVIDE(0);
		CullMode = None;			
		ZEnable  =  False;
//		AlphaBlendEnable = True;
//		SrcBlend = One;
//		DestBlend = One;
//		BlendOp = Add;
		//FillMode  =  WireFrame;
	}
	PASS Vert_Mirror_Flip//P4
	{
		VertexShader = compile vs_3_0 VS_DIVIDE(3);
		PixelShader = compile  ps_3_0 PS_DIVIDE(0);
		CullMode = None;			
		ZEnable  =  False;
//		AlphaBlendEnable = True;
//		SrcBlend = One;
//		DestBlend = One;
//		BlendOp = Add;
		//FillMode  =  WireFrame;
	}
}
PixelShader Picture_YUVA_NO_BORDER[2]=
{
	compile  ps_2_0 PS_YUVA(0,0),
	compile  ps_2_0 PS_YUVA(0,1),
};
PixelShader Picture_YUVA_BORDER[2]=
{
	compile  ps_2_0 PS_YUVA(1,0),
	compile  ps_2_0 PS_YUVA(1,1),
};
technique Picture
{
	PASS YUYV_AP //P0
	{
		VertexShader = compile vs_3_0 VS_COLOR();
		PixelShader = compile  ps_3_0 PS_YUYV_AP();
		CullMode = None;			
		ZEnable  =  False;
	}
 

	PASS YUYV_NA //P1
	{
		VertexShader = compile vs_3_0 VS_COLOR();
		PixelShader = compile  ps_3_0 PS_YUYV_NA();
		CullMode = None;			
		ZEnable  =  False;
	}

	PASS YUVA_RGBA_NO_BORDER //P2
	{
		VertexShader = compile vs_2_0 VS_COLOR();
		PixelShader = (Picture_YUVA_NO_BORDER[int(g_bInvert)]);
		CullMode = None;			
		ZEnable  =  False;
		
		AlphaBlendEnable = <g_bAlphaBlend>;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}

	PASS YUVA_RGBA_BORDER //P3
	{
		VertexShader = compile vs_2_0 VS_COLOR();
		PixelShader = (Picture_YUVA_BORDER[int(g_bInvert)]);
		CullMode = None;			
		ZEnable  =  False;
		
		AlphaBlendEnable = <g_bAlphaBlend>;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}
technique Trans
{
	PASS YUYV_AP //P0
	{
		VertexShader = compile vs_3_0 VS_COLOR();
		PixelShader = compile  ps_3_0 PS_YUYV_TRANS();
		CullMode = None;			
		ZEnable  =  False;
	}
 

	PASS YUYV_NA //P1
	{
		VertexShader = compile vs_3_0 VS_COLOR();
		PixelShader = compile  ps_3_0 PS_YUYV_TRANS();
		CullMode = None;			
		ZEnable  =  False;
	}

	PASS YUVA_RGBA_NO_BORDER //P2
	{
		VertexShader = compile vs_2_0 VS_COLOR();
		PixelShader = compile  ps_2_0 PS_YUVA_TRANS(false);
		CullMode = None;			
		ZEnable  =  False;
		
		AlphaBlendEnable = <g_bAlphaBlend>;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}

	PASS YUVA_RGBA_BORDER //P3
	{
		VertexShader = compile vs_2_0 VS_COLOR();
		PixelShader = compile  ps_2_0 PS_YUVA_TRANS(true);
		CullMode = None;			
		ZEnable  =  False;
		
		AlphaBlendEnable = <g_bAlphaBlend>;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}