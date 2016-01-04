#include "..\Shaders\ColorSpaceFunc.INC"

float4x4 g_matWorldViewProj;
float4x4 g_matTexture;

texture g_txColor;
texture g_txAlpha;
texture	g_txRandom;
texture g_txRandomPixel;
texture g_txATan;

sampler g_samColor=
sampler_state
{	
	Texture=<g_txColor>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};

sampler g_samAlpha=
sampler_state
{	
	Texture=<g_txAlpha>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};

sampler g_samRandom=
sampler_state
{
	Texture=<g_txRandom>;
	MinFilter=Point;
	MagFilter=Point;
	AddressU=Wrap;
	AddressV=Wrap;
};

sampler g_samAtan=
sampler_state
{
	Texture=<g_txATan>;
	MinFilter=Linear;
	MagFilter=Linear;
};

sampler g_samRandomPixel = 
sampler_state
{
	Texture=<g_txRandomPixel>;
	MinFilter=Point;
	MagFilter=Point;
	AddressU=Wrap;
	AddressV=Wrap;
};

float4	g_param;
#define	g_blockAngle	g_param.x
#define	g_movement		g_param.y
#define	g_angleBlock	g_param.z
#define g_rotateAngle	g_param.w

float4  g_param2;
#define g_center		g_param2.xy
#define	g_aspect		g_param2.z
#define	g_blockCount	g_param2.w

float3	g_randomDegree;
float4	g_texRc;		// left,top,width,height


void VertScene(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oPosCoord:TEXCOORD0,
				out	float2 oTexCoord:TEXCOORD1)
{	
	oPos		= mul(float4(vPos,1.f),g_matWorldViewProj);
	oPosCoord	= vPos.xy;			// [-0.5,0.5] 
	//oTexCoord	= mul(float4(vTex,1.0f,0.0f),g_matTexture);		
	oTexCoord = vTex;
}	

static const float D3DPI = atan(1.f);

float4 PixSceneRGBA(float2 posCoord:TEXCOORD0, float2 texCoord:TEXCOORD1,uniform bool bYUVA):COLOR0
{
	float2	posRelative = (posCoord-g_center)*float2(g_aspect,1.f);
	float	angle	= atan2(-posRelative.y,posRelative.x)+D3DPI+g_rotateAngle;		// 贴图坐标和位置坐标y方向相反
	//float2	posRelative = normalize(posCoord*float2(g_aspect,1.f) - g_center)/2.f+0.5f;
	//float angle		= tex2D(g_samAtan, float2(-posRelative.y,posRelative.x))+3.1415926f+g_rotateAngle;
	float  pixelRandomY = tex2D(g_samRandomPixel,float2(texCoord.y,0.5f)).r;
	angle += tex2D(g_samRandomPixel,float2((texCoord.x+pixelRandomY)/2.f,1.5f)).r * g_randomDegree.z * D3DPI;
	float seqNum	= floor(angle/g_blockAngle);
	
	float2	randomMoveWidth = tex1D(g_samRandom, seqNum/g_blockCount).bg;	
	randomMoveWidth.x = randomMoveWidth.x-1.f;					// [0,1]->[-0.5f,0.5f]缩小的倍数
	
	seqNum = floor((angle+g_blockAngle*randomMoveWidth.x*g_randomDegree.x)/g_blockAngle);	
	randomMoveWidth = tex1D(g_samRandom, seqNum/g_blockCount).bg;
		
	randomMoveWidth.y = randomMoveWidth.y*0.2f-0.1f;			// [0,1]->[-0.1,0.1]
	
	float	blockAngle = seqNum*g_angleBlock-D3DPI-g_rotateAngle;
		
	float2 blockAngleCosSin;
	sincos(blockAngle,blockAngleCosSin.y,blockAngleCosSin.x);
	float2 texCoordTrue = texCoord + blockAngleCosSin*g_movement + randomMoveWidth.y*g_randomDegree.y;

	float4 retColor = bYUVA? cYUVAMode : 0.f;
	if(texCoordTrue.x>=0.f && texCoordTrue.x<=1.f && texCoordTrue.y>=0.f && texCoordTrue.y<=1.f)
	{
		//float2 texCoordChanged = g_texRc.zw * texCoordTrue + g_texRc.xy;
		float2 texCoordChanged	= mul(float4(texCoordTrue,1.0f,0.0f),g_matTexture);	
		retColor = tex2D(g_samColor,texCoordChanged);
	}	
	
	return retColor;
}


void PixSceneYUYV_NA(float2 posCoord:TEXCOORD0, float2 texCoord:TEXCOORD1,
						 out float4 oColor:COLOR0, out float4 oAlpha:COLOR1)
{
	float2	posRelative = posCoord*float2(g_aspect,1.f)-g_center;
	float	angle	= atan2(-posRelative.y,posRelative.x)+3.1415926f+g_rotateAngle;		// 贴图坐标和位置坐标y方向相反
	float seqNum	= floor(angle/g_blockAngle);
	
	float2	randomMoveWidth = tex1D(g_samRandom, seqNum/g_blockCount).bg;	
	randomMoveWidth.x = randomMoveWidth.x-1.f;					// [0,1]->[-0.5f,0.5f]缩小的倍数
	
	seqNum = floor((angle+g_blockAngle*randomMoveWidth.x*g_randomDegree.x)/g_blockAngle);	
	randomMoveWidth = tex1D(g_samRandom, seqNum/g_blockCount).bg;
		
	randomMoveWidth.y = randomMoveWidth.y*0.2f-0.1f;			// [0,1]->[-0.1,0.1]
	
	float	blockAngle = seqNum*g_angleBlock-3.1415926f-g_rotateAngle;
		
	float2 blockAngleCosSin;
	sincos(blockAngle,blockAngleCosSin.y,blockAngleCosSin.x);
	float2 texCoordTrue = texCoord + blockAngleCosSin*g_movement + randomMoveWidth.y*g_randomDegree.y;

	oColor = float4(0.f,0.5f,0.f,0.5f);
	oAlpha = 0.f;
	if(texCoordTrue.x>=0.f && texCoordTrue.x<1.f && texCoordTrue.y>=0.f && texCoordTrue.y<1.f)
	{
		float2 texCoordChanged = g_texRc.zw * texCoordTrue + g_texRc.xy;
		oColor = tex2D(g_samColor,texCoordChanged);
		oAlpha = 1.f;
	}	
}


void PixSceneYUYV_AP(float2 posCoord:TEXCOORD0, float2 texCoord:TEXCOORD1,
						 out float4 oColor:COLOR0, out float4 oAlpha:COLOR1)
{
	float2	posRelative = posCoord*float2(g_aspect,1.f)-g_center;
	float	angle	= atan2(-posRelative.y,posRelative.x)+3.1415926f+g_rotateAngle;		// 贴图坐标和位置坐标y方向相反
	float seqNum	= floor(angle/g_blockAngle);
	
	float2	randomMoveWidth = tex1D(g_samRandom, seqNum/g_blockCount).bg;	
	randomMoveWidth.x = randomMoveWidth.x-1.f;					// [0,1]->[-0.5f,0.5f]缩小的倍数
	
	seqNum = floor((angle+g_blockAngle*randomMoveWidth.x*g_randomDegree.x)/g_blockAngle);	
	randomMoveWidth = tex1D(g_samRandom, seqNum/g_blockCount).bg;
		
	randomMoveWidth.y = randomMoveWidth.y*0.2f-0.1f;			// [0,1]->[-0.1,0.1]
	
	float	blockAngle = seqNum*g_angleBlock-3.1415926f-g_rotateAngle;
		
	float2 blockAngleCosSin;
	sincos(blockAngle,blockAngleCosSin.y,blockAngleCosSin.x);
	float2 texCoordTrue = texCoord + blockAngleCosSin*g_movement + randomMoveWidth.y*g_randomDegree.y;

	oColor = float4(0.f,0.5f,0.f,0.5f);
	oAlpha = 0.f;
	if(texCoordTrue.x>=0.f && texCoordTrue.x<1.f && texCoordTrue.y>=0.f && texCoordTrue.y<1.f)
	{
		float2 texCoordChanged = g_texRc.zw * texCoordTrue + g_texRc.xy;
		oColor = tex2D(g_samColor,texCoordChanged);
		oAlpha = tex2D(g_samAlpha,texCoordChanged);
	}	
}




technique RenderRGBA
{
	PASS  RGBA//P0
	{
		VertexShader	= compile vs_3_0 VertScene();
		PixelShader	= compile ps_3_0 PixSceneRGBA(0);	
	}
	PASS YUVA//P1
	{
		VertexShader	= compile vs_3_0 VertScene();
		PixelShader	= compile ps_3_0 PixSceneRGBA(1);	
	}
}

technique RenderYUYV_NA
{
	PASS P0
	{
		VertexShader	= compile vs_3_0 VertScene();
		PixelShader	= compile ps_3_0 PixSceneYUYV_NA();	
	}
}

technique RenderYUYV_AP
{
	PASS P0
	{
		VertexShader	= compile vs_3_0 VertScene();
		PixelShader	= compile ps_3_0 PixSceneYUYV_AP();	
	}
}


