float4x4 g_matWVP;
float4x4 g_matTex;
float4x4 g_matCoord;
float4 g_vABC;
float g_fLeftBorder;
float g_fWidth;

int4 g_nImageInfo;

texture g_txColor;

sampler g_samColor =
sampler_state
{	
	Texture=<g_txColor>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};

texture g_txNoise;
sampler g_samNoise=
sampler_state
{	
	Texture=<g_txNoise>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Mirror;
	AddressV=Wrap;
	AddressW=Wrap;
};

void VS(float4 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0,
				out float2 oCoord:TEXCOORD1)
{	
	oPos = mul(vPos,g_matWVP);	
	oTex = mul(float4(vTex,1.0f, 0.f),g_matTex);
	oCoord = mul(float4(vTex,1.0f, 0.f),g_matCoord);
}	

float4 PS_Diffuse(uniform bool bAlpha, float2 inTex:TEXCOORD0, float2 inCoord:TEXCOORD1):COLOR0
{
	float4 oColor = tex2D(g_samColor, inTex);
	//计算当前点到基准点的距离，距离公式为|A*x_0 + B*y_0 + C|/sqrt(A^2 + B^2)，其中A,B,C从外部传入
	float fDist = abs(dot(inCoord.xy, g_vABC.xy) + g_vABC.z ) / length(g_vABC.xy);
	float fOutAlpha = saturate((fDist - g_fLeftBorder) / g_fWidth);
	if(fOutAlpha == 0.f)
		discard;
	float3 vSamNoise = float3( (inTex.x + inCoord.y)*5789.f, (inTex.y + inCoord.x)*337.3f, (inTex.y + inCoord.y)*42.9f);
	float fNoise = tex3D(g_samNoise, vSamNoise).r;
	if( fNoise > fOutAlpha)
	{
		discard;
	}

	if(bAlpha)
	{
		oColor.rgb *= oColor.a;
	}
	return oColor;
}

float4 PS_Pixelate(uniform bool bAlpha, float2 inTex:TEXCOORD0, float2 inCoord:TEXCOORD1):COLOR0
{
	float4 oColor = tex2D(g_samColor, inTex);
	float2 vPixelCoord;
	vPixelCoord.x = floor(inCoord.x * g_nImageInfo.x / g_nImageInfo.z) * g_nImageInfo.z / g_nImageInfo.x;
	vPixelCoord.y = floor(inCoord.y * g_nImageInfo.x / g_nImageInfo.w) * g_nImageInfo.w / g_nImageInfo.x;
	//计算当前点到基准点的距离，距离公式为|A*x_0 + B*y_0 + C|/sqrt(A^2 + B^2)，其中A,B,C从外部传入
	float fDist = abs(dot(vPixelCoord.xy, g_vABC.xy) + g_vABC.z ) / length(g_vABC.xy);
	float fOutAlpha = saturate((fDist - g_fLeftBorder) / g_fWidth);
	if(fOutAlpha == 0.f)
		discard;

	float3 vSamNoise = float3(vPixelCoord.xy * 835.18f, vPixelCoord.x * 34.5f + vPixelCoord.y * 128.f);
	float fNoise = tex3D(g_samNoise, vSamNoise).r;
	if( fNoise > fOutAlpha)
	{
		discard;
	}

	if(bAlpha)
	{
		oColor.rgb *= oColor.a;
	}
	return oColor;
}

technique DiffuseWipe
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_Diffuse(false);
		CullMode = None;
		ZEnable = False;
	}
}

technique DiffuseWipeTrans
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_Diffuse(true);
		CullMode = None;
		ZEnable = False;
		AlphablendEnable = True;
		SrcBlend = One;
		DestBlend = InvSrcAlpha;
	}
}

technique PixelateWipe
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_Pixelate(false);
		CullMode = None;
		ZEnable = False;
	}
}

technique PixelateWipeTrans
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS_Pixelate(true);
		CullMode = None;
		ZEnable = False;
		AlphablendEnable = True;
		SrcBlend = One;
		DestBlend = InvSrcAlpha;
	}
}