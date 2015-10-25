float4x4 g_matWorldViewProj;
float4x4 g_matMask;

float4		g_vMisc;
#define		g_fBorderWidth		g_vMisc.x
#define		g_fSoftWidth		g_vMisc.y
#define		g_fOffset			g_vMisc.z
#define     g_fCenterOffset     g_vMisc.w

float4      g_vMisc1;
#define		g_fZFunc			g_vMisc1.x

texture2D g_txMask0;
texture2D g_txMask1;

sampler g_samMask0=
sampler_state
{	
	Texture=<g_txMask0>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};
sampler g_samMask1=
sampler_state
{	
	Texture=<g_txMask1>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

void VS(float4 vPos:POSITION,													
				out float4 oPos:POSITION,
				out float4 oColor:COLOR0,
				uniform int nType
				)
{
	oColor = 0;		
	
	float2 pos = vPos.xy;
	
	if(nType == 0) // basic 
	{
		pos.x *= g_fOffset;
	}
	else
	{
		pos.x = -g_fOffset + g_fCenterOffset;
	}
	
	if(nType == 1) // border 
	{
		pos.x += g_fBorderWidth * (vPos.z > 0?1.0f : -1.0f);
	}
	else if(nType == 2) //  soft_no_border
	{
		oColor.a = vPos.z;
		
		pos.x += g_fSoftWidth * (vPos.z > 0?-1.0f : 1.0f);
	}
	else if(nType == 3) // soft_border_right
	{
		oColor.a = 1.0f - vPos.z;
		
		float fDistance = g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.x += max(0,fDistance);
		if(fDistance < 0)
			oColor.a = 1.0f + fDistance / (2.0f * g_fSoftWidth);
	}
	else if(nType == 4) // soft_border_left
	{
		float fDistance =  -g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.x += min(0,fDistance);
		oColor.a = vPos.z;
		if(fDistance > 0)
			oColor.a = 1.0f  - fDistance  / (2.0f * g_fSoftWidth);
	}
	else if(nType == 5) // soft_Greate_border_right
	{	
		float fDistance =  -g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.x += max(0,fDistance);
		oColor.a = vPos.z;
		if(fDistance < 0)
			oColor.a = - fDistance  / (2.0f * g_fSoftWidth);
	}
	else if(nType == 6) // soft_Greate_border_left
	{
		oColor.a = 1.0f - vPos.z;
		
		float fDistance = g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.x += min(0,fDistance);
		if(fDistance > 0)
			oColor.a =  fDistance / (2.0f * g_fSoftWidth);	
		
	}
	oPos = mul(float4(pos,0.0f,1.0f),g_matWorldViewProj);	
	
	oPos /= oPos.w;
	oPos.z = 0.6f;
}	

float4 PS(float2 Tex:TEXCOORD0,
		  float4 Color:COLOR0,
		  uniform int nType):COLOR0
{
	float4 retColor = Color;
	
	if(nType == 1) // Border
	  retColor = float4(1.0f,0.0f,0.0f,1.0f);
	else if(nType == 2) // soft_no_Border	
	  retColor = float4(0.0f,1.0f,0.0f,Color.a + 0.004f);  
	else if(nType == 3) // soft_Border_right
	  retColor = float4(0.0f,1.0f,0.0f,Color.a + 0.004f);  
	else if(nType == 4) // soft_Border	_left
	  retColor = float4(0.0f,0.0f,1.0f,Color.a + 0.004f);  
	else if(nType == 5) // soft_Greare_Border_right
	  retColor = float4( Color.a + 0.004f,1.0f,0.0f,0.0f);  
	else if(nType == 6) // soft_Greate_Border_Left	
	  retColor = float4( Color.a + 0.004f,0.0f,1.0f,0.0f);  
	  
	return retColor;
}

technique Mask
{
	PASS Shape //0
	{
		VertexShader=compile vs_1_1 VS(0);
		PixelShader=compile  ps_2_0 PS(0);
		CullMode=None;	
			
		ZEnable = False;		
		//FillMode = WireFrame;
	}
	PASS Border //1 
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(1);
		
		ZEnable = True;
		ZFunc = <g_fZFunc>;
		ZWriteEnable = False;
		
		CullMode=None;		
		//FillMode = WireFrame;
	}
	PASS Soft_No_Border //2
	{
		VertexShader=compile vs_1_1 VS(2);
		PixelShader=compile  ps_2_0 PS(2);
		CullMode=None;		
		ShadeMode = Gouraud;
		
		ZEnable = True;
		ZFunc = <g_fZFunc>;
		ZWriteEnable = False;
		//FillMode = WireFrame;
	}
	PASS Soft_Border_Right //3
	{
		VertexShader=compile vs_1_1 VS(3);
		PixelShader=compile  ps_2_0 PS(3);
		CullMode=None;		
		ShadeMode = Gouraud;
		
		ZEnable = True;
		ZFunc = <g_fZFunc>;
		ZWriteEnable = False;	
		//FillMode = WireFrame;
	}
	PASS Soft_Border_Left //4
	{
		VertexShader=compile vs_1_1 VS(4);
		PixelShader=compile  ps_2_0 PS(4);
		CullMode=None;		
		ShadeMode = Gouraud;	
		
		ZEnable = True;
		ZFunc = <g_fZFunc>;
		ZWriteEnable = False;
		//FillMode = WireFrame;
	}
	
	PASS Soft_Greate_Border_Right //5
	{
		VertexShader=compile vs_1_1 VS(5);
		PixelShader=compile  ps_2_0 PS(5);
		CullMode=None;		
		ShadeMode = Gouraud;	
		ColorWriteEnable = 1;	
		
		ZEnable = True;
		ZFunc = <g_fZFunc>;
		ZWriteEnable = False;
		//FillMode = WireFrame;
	}
	
	PASS Soft_Greate_Border_Left //6
	{
		VertexShader=compile vs_1_1 VS(6);
		PixelShader=compile  ps_2_0 PS(6);
		CullMode=None;
		ShadeMode = Gouraud;			
		ColorWriteEnable = 1;
		
		ZEnable = True;
		ZFunc = <g_fZFunc>;
		ZWriteEnable = False;
		//FillMode = WireFrame;
	}
	
}


void VS_DIRECT_OUT(float4 vPos:POSITION,													
				out float4 oPos:POSITION					
				)
{
	oPos = mul(float4(vPos.xy,0.0f,1.0f),g_matWorldViewProj);	
	oPos /= oPos.w;
	oPos.z = 0.5f;
}
float4 PS_DIRECT_OUT(float2 Tex:TEXCOORD0):COLOR0
{
	return 1;
}

technique ZMask
{
	PASS Shape //0
	{
		VertexShader = compile vs_1_1 VS_DIRECT_OUT();
		PixelShader = compile  ps_2_0 PS_DIRECT_OUT();
		CullMode = None;		
		ZEnable = True;
		ZFunc = Always;
		ZWriteEnable = True;
		ClipPlaneEnable = ClipPlane0;
		ColorWriteEnable = 0;
	}	
}
//Compose
void VS_COMPOSE(float4 vPos:POSITION,													
		float2 vTex:TEXCOORD0,		
		out float4 oPos:POSITION,
		out float2 oTex:TEXCOORD0
		)
{	
	oPos = mul(float4(vPos.xy,0.0f,1.0f),g_matWorldViewProj);	
	oTex.xy = mul(float3(vTex,1.0f),g_matMask);	
}
float GetDepth(float4 vColor)
{
	float depth = 0.9f;	
	
	if(vColor.g > 0.0f)
		depth = 0.2f + vColor.a * 0.1f;
	else if(vColor.r > 0.0f)
	{
		depth = 0.4f + vColor.r * 0.1f;
		if(vColor.b > 0.0f)
			depth = 0.7f - vColor.a * 0.1f;
	}
	else if(vColor.b > 0.0f)
		depth = 0.7f - vColor.a * 0.1f;
	else if(vColor.a < 1.0f)
		depth = vColor.a * 0.1f;	
	
	return depth;
}
void PS_COMPOSE(float2 Tex:TEXCOORD0,out float4 oColor:COLOR0)
{
	float4 vColor0 = tex2D(g_samMask0,Tex);
	float4 vColor1 = tex2D(g_samMask1,Tex);		
 		
	float fDepth0 = GetDepth(vColor0);	
	float fDepth1 = GetDepth(vColor1);		
	
	if(fDepth0 <= fDepth1)
		oColor = vColor0;
	else
		oColor = vColor1;	
}

technique Compose
{	
	PASS P0
	{
		VertexShader=compile vs_3_0 VS_COMPOSE();
		PixelShader=compile  ps_3_0 PS_COMPOSE();
		CullMode=None;		
		//FillMode = WireFrame;
		ZEnable  =  False;
		ZFunc    = Less;
		ZWriteEnable = False;
		
		StencilEnable = False;
		StencilFunc = Less;
		StencilRef  = 0;	
		StencilPass = Keep;	
	}	
}