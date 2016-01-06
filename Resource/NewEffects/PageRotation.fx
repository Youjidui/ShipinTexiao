float4x4 g_matWorldViewProj;
float4x4 g_matTex;

int g_nPattern;
float4	g_vMisc;

texture g_txColor;

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

texture g_txColor1;

sampler g_samColor1=
sampler_state
{	
	Texture=<g_txColor1>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Clamp;
	AddressV=Clamp;
};

void VS(float4 vPos:POSITION,				
		float2 vTex:TEXCOORD0,				
		out float4 oPos:POSITION,				
		out float2 oTex:TEXCOORD0,
		uniform int nPass,
		uniform int nPattern)
{	
	if(nPattern == 0)
	{
		vTex.y /= 2.0f;
		if(nPass == 0)
		{
			vPos.y += 0.5f;
			vPos.y /= 2.0f;		
		}
		else if(nPass == 1)
		{
			vPos.y += 0.5f;
			vPos.y /= 2.0f;
			vTex.y = 1.0f - vTex.y;		
		}
		else if(nPass == 2)
		{
			vPos.y += 0.5f;
			vPos.y /= 2.0f;		
		}
		else if(nPass == 3)
		{
			vPos.y -= 0.5f;
			vPos.y /= 2.0f;
			vTex.y += 0.5f;
		}
	}
	else
	{
		vTex.x /= 2.0f;
		if(nPass == 0)
		{
			vPos.x -= 0.5f;
			vPos.x /= 2.0f;		
		}
		else if(nPass == 1)
		{
			vPos.x -= 0.5f;
			vPos.x /= 2.0f;
			vTex.x = 1.0f - vTex.x;		
		}
		else if(nPass == 2)
		{
			vPos.x -= 0.5f;
			vPos.x /= 2.0f;		
		}
		else if(nPass == 3)
		{
			vPos.x += 0.5f;
			vPos.x /= 2.0f;
			vTex.x += 0.5f;
		}
	}
	
	oPos=mul(vPos,g_matWorldViewProj);
	oTex= mul(float3(vTex,1.0f),g_matTex);	
}
float4 PS(float2 Tex:TEXCOORD0,uniform int nPass):COLOR0
{
	float4 vColor=(float4)0;
	
	if(nPass == 0 || nPass == 3)
	{
		vColor=tex2D(g_samColor,Tex);
		vColor.a *= g_vMisc.x;
	}
	else
	{
		vColor=tex2D(g_samColor1,Tex);
		vColor.a *= g_vMisc.y;
	}

	return vColor;
}
VertexShader aVS[] =
{
	compile vs_1_1  VS(0,0),
	compile vs_1_1  VS(1,0),
	compile vs_1_1  VS(2,0),
	compile vs_1_1  VS(3,0),
	compile vs_1_1  VS(0,1),
	compile vs_1_1  VS(1,1),
	compile vs_1_1  VS(2,1),
	compile vs_1_1  VS(3,1),
};
technique PageRotation
{
	PASS P0
	{
		VertexShader= (aVS[g_nPattern * 4 + 0]);
		PixelShader=compile  ps_2_0 PS(0);
		CullMode= CCW;
		ZEnable = True;
		ZFunc = Always;
		ZWriteEnable = True;		
	}
	PASS P1
	{
		VertexShader= (aVS[g_nPattern * 4 + 1]);
		PixelShader=compile  ps_2_0 PS(1);
		CullMode= CW;	
		
		ZEnable = True;
		ZFunc = Always;
		ZWriteEnable = True;		
	}
	PASS P2
	{
		VertexShader= (aVS[g_nPattern * 4 + 2]);
		PixelShader=compile  ps_2_0 PS(2);
		CullMode= CCW;	
		
		ZEnable = True;
		ZFunc = Less;
		ZWriteEnable = False;		
	}
	PASS P3
	{
		VertexShader= (aVS[g_nPattern * 4 + 3]);
		PixelShader=compile  ps_2_0 PS(3);
		CullMode= CCW;	
		
		ZEnable = True;
		ZFunc = Less;
		ZWriteEnable = False;	
	}
}