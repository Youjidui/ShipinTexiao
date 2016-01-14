#include "..\Shaders\ColorSpaceFunc.inc"

#define BROKENGLASS	0
#define EXPLODE			1

static const float dropDisturb = 2.f;	// distrub 4 lines.
static const float D3DPI20 = atan(1.f)*20.f;
static const float	g_force = 4.f;	// s = a*t*t; if t=0.5,s=1 -> a = 4.f;

float4x4 g_matWorldViewProj;
float4x4 g_matWorldNormal;
float4x4 g_matTexture;
float	 g_offsetField;

int2	g_numXY;
float	g_time;	
float3	g_lightDir;
float2  g_lightAmbDiff;
bool	g_bReverseDir;
float2	g_centerXY;

texture g_txColor;
texture g_txAlpha;
texture g_txRandom;
texture g_txRandomARGB;

sampler g_samColor=
sampler_state
{	
	Texture=<g_txColor>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Wrap;
	AddressV=Wrap;
};

sampler g_samAlpha=
sampler_state
{	
	Texture=<g_txAlpha>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Wrap;
	AddressV=Wrap;
};

sampler g_samRandom = 
sampler_state
{
	Texture = <g_txRandom>;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler g_samRandomARGB = 
sampler_state
{
	Texture = <g_txRandomARGB>;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4x4 matrixRotation(float3 rotAngle)
{
	float3 sinXYZ;
	float3 cosXYZ;
	sincos(rotAngle,sinXYZ,cosXYZ);
	float4x4 matRotX =
	{
		1, 0, 0, 0,
		0, cosXYZ.x, -sinXYZ.x, 0,
		0, sinXYZ.x, cosXYZ.x, 0,
		0, 0, 0, 1
	};
	float4x4 matRotY = 
	{
		cosXYZ.y, 0, sinXYZ.y, 0,
		0, 1, 0, 0,
		-sinXYZ.y, 0, cosXYZ.y, 0,
		0, 0, 0, 1	
	};
	float4x4 matRotZ = 
	{
		cosXYZ.z, -sinXYZ.z, 0, 0,
		sinXYZ.z, cosXYZ.z, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1	
	};
	return mul( mul(matRotY, matRotZ), matRotX );	// Yaw Pitch Row
};

float4x4 computeTransformDrop(float2 triSeqNo, float2 quadWH, float4 random)
{
	if( g_bReverseDir )
		triSeqNo.y = g_numXY.y - 1 - triSeqNo.y;

	float startFallingTime = float(triSeqNo.y) / g_numXY.y;
	startFallingTime += random.w * quadWH.y * min(dropDisturb,g_numXY.y);		// random falling time
	float timeElapsed = max(0.f,g_time-startFallingTime);
	
	float distanceFalling = g_force * timeElapsed * timeElapsed;
	float4x4 matTransform = matrixRotation(random.xyz*D3DPI20*timeElapsed);
	matTransform[3].y -= distanceFalling;
	return matTransform;	
}

float4x4 computeTransformExplode(float2 triSeqNo, float2 quadWH, float4 random)
{
	float2 center = g_numXY/2.f - g_centerXY*g_numXY;
	float2 curDirection = triSeqNo*float2(0.5f,1.f)-center;
	float maxDist = length(max(abs(g_numXY-center),abs(center)));
	float curDist = length(curDirection);

	if( g_bReverseDir )
		curDist = maxDist - curDist;

	float startFallingTime = curDist / maxDist;
	startFallingTime += random.w * 1.f/maxDist * dropDisturb;		// random falling time
	float timeElapsed = max(0.f,g_time-startFallingTime);
	
	float distanceFalling = g_force * timeElapsed * timeElapsed;
	float4x4 matTransform = matrixRotation(random.xyz*D3DPI20*timeElapsed);
	if( !any(curDirection) ) curDirection = random.xy;
	matTransform[3].xy += distanceFalling*normalize(curDirection);
	return matTransform;		
}

void VertScene(float3 vPos:POSITION,	
				float3 vNormal:NORMAL,			
// 				float2 vTex:TEXCOORD0,	
				int3   vSeqNum:TEXCOORD0,
				int    instanceNum:TEXCOORD1,			
				out float4 oPos:POSITION,
				out float3 oNormal:NORMAL,				
				out float2 oTex:TEXCOORD0,
				uniform const int type)
{	
	int triangleRowCount = g_numXY.x * 2;
	int triagnelAllCount = triangleRowCount * g_numXY.y;
	int triangleSeq = instanceNum * 2 + vSeqNum.z;
	
	int2 triangleXY;
	triangleXY.y = floor(triangleSeq / triangleRowCount);			// row number
	triangleXY.x = triangleSeq - triangleRowCount * triangleXY.y;	// seq in row
	int2 quadXY = floor(triangleXY * float2(0.5f,1.f));
	float2 quadWH = float2(1.f,1.f) / g_numXY;
	float2 fromCenter = quadXY * quadWH + quadWH/2.f - float2(0.5f,0.5f);
	
	float2 positionOffset = {0.f,0.f};
// 	if( quadXY.y == 0 && vSeqNum.y == 0 );
// 	else if( quadXY.y == g_numXY.y-1 && vSeqNum.y == 1 );
// 	else if( quadXY.x == 0 && vSeqNum.x == 0 );
// 	else if( quadXY.x == g_numXY.x-1 && vSeqNum.x == 1 );
	int2 checkBound = quadXY + vSeqNum;
	int2 checkBound2 = checkBound - g_numXY;
	if( all(checkBound*checkBound2) )
	{	int2 splitSeqXY = quadXY + vSeqNum.xy;
		float splitSeq = splitSeqXY.y * g_numXY.x + splitSeqXY.x;
		
		positionOffset.x = tex2Dlod(g_samRandom,float4(splitSeq/1024.f,1.5f,0.f,0.f)).r;
		positionOffset.y = tex2Dlod(g_samRandom,float4(splitSeq/1024.f,2.5f,0.f,0.f)).r;
		positionOffset *= quadWH/2.f;
	}

	float4 randomTrans = tex1Dlod(g_samRandomARGB,float4(triangleSeq/1024.f,0.5f,0.f,0.f));
	randomTrans = randomTrans*2.f - 1.f;

	// position and texCoord
	float3 position = vPos * float3(quadWH,1.f);
 	position.xy = position.xy + fromCenter + positionOffset;
	oTex = mul(float4(position.xy,1.0f,0.0f),g_matTexture);	
	oTex.y += g_offsetField;	
	
	// falling time start, falling distance
	// transform
	
	float4x4 matTransform;
	if( type == BROKENGLASS )
		matTransform = computeTransformDrop(triangleXY,quadWH,randomTrans);
	else
		matTransform = computeTransformExplode(triangleXY,quadWH,randomTrans);

	oNormal = mul(vNormal,matTransform);

	position.xy -= fromCenter;
	float4 posOut = mul(float4(position,1.f),matTransform);
	posOut.xy += fromCenter;
	//float4 posOut = float4(position,1.f);
	
	oPos=mul(posOut,g_matWorldViewProj);
	oNormal = mul(oNormal,g_matWorldNormal);
}	


float4 PixSceneYUYV_NA(float2 Tex:TEXCOORD0, float3 normalWorld:NORMAL):COLOR0
{
	float4 fColor=(float4)0;
	
		
	fColor=tex2D(g_samColor,Tex);
		
	return fColor;
}

void  PixSceneYUYV_AP(float2 Tex:TEXCOORD0,out float4 oColor:COLOR0,out float4 oAlpha:COLOR1)
{		
	oColor=tex2D(g_samColor,Tex);
	oAlpha=tex2D(g_samAlpha,Tex);	
}

float4 PixSceneYUVA(float2 Tex:TEXCOORD0, float3 normalWorld:NORMAL):COLOR0
{
	float4 fColor = tex2D(g_samColor,Tex) - float4(0.f, CS_UV_OFFSET, CS_UV_OFFSET, 0.f);
	
	float diffuse = dot(normalWorld,-g_lightDir) * g_lightAmbDiff.y;	// no need to clamp
		
	return fColor * (diffuse + g_lightAmbDiff.x) + float4(0.f, CS_UV_OFFSET, CS_UV_OFFSET, 0.f);
}


technique RenderYUYV_NA
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene(BROKENGLASS);
		PixelShader=compile  ps_3_0 PixSceneYUYV_NA();
		CullMode=None;
		AlphaBlendEnable = False;
	}
}

technique RenderYUYV_AP
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene(BROKENGLASS);
		PixelShader=compile  ps_3_0 PixSceneYUYV_AP();
		CullMode=None;
		AlphaBlendEnable = False;
	}
}

technique RenderYUVA
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene(BROKENGLASS);
		PixelShader=compile  ps_3_0 PixSceneYUVA();
		CullMode=None;
		AlphaBlendEnable = False;
		//FillMode = WIREFRAME;
	}
}

technique RenderYUVA_EXPLODE
{
	PASS P0
	{
		VertexShader=compile vs_3_0 VertScene(EXPLODE);
		PixelShader=compile  ps_3_0 PixSceneYUVA();
		CullMode=None;
		AlphaBlendEnable = False;
		//FillMode = WIREFRAME;
	}
}
