float4x4 g_matWorldViewProj;
float4x4 g_matWorldViewProjFix;
float4x4 g_matWorldViewProjCenter;
float4x4 g_matWorld;

float4x4 g_matMask;

texture g_txMask;

float   g_fBorderWidth;
float   g_fSoftWidth;


float	g_bInvert;
float   g_fOffset;


sampler g_samMask=
sampler_state
{	
	Texture=<g_txMask>;
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
	
	float fSign = (vPos.z > 0?1.0f : -1.0f);
		
	float2 pos = vPos.xy;
	if(nType == 1) // border 
	{
		pos.x += g_fBorderWidth * fSign;
	}
	else if(nType == 2) //  soft_no_border
	{
		if(g_bInvert)
			oColor.a = vPos.z;
		else
			oColor.a = 1.0 - vPos.z;
		
		pos.x += g_fSoftWidth * fSign;
	}
	else if(nType == 3) // soft_border_right
	{	
		oColor.a = 1.0 - vPos.z;
		
		float fDistance = g_fBorderWidth + g_fSoftWidth * fSign;
		pos.x += max(0,fDistance);
		if(fDistance < 0)
			oColor.a = 1.0f + fDistance / (2.0f * g_fSoftWidth);
	}
	else if(nType == 4) // soft_border_left
	{
		float fDistance =  -g_fBorderWidth + g_fSoftWidth * fSign;
		pos.x += min(0,fDistance);
	
		oColor.a = vPos.z;
		if(fDistance > 0)
			oColor.a = 1.0f  - fDistance  / (2.0f * g_fSoftWidth);
	}
	else if(nType == 5) // soft_Greate_border_right
	{	
		float fDistance =  -g_fBorderWidth + g_fSoftWidth * fSign;
		pos.x += max(0,fDistance);
	
		oColor.a = vPos.z;
		if(fDistance < 0)
			oColor.a = - fDistance  / (2.0f * g_fSoftWidth);
	}
	else if(nType == 6) // soft_Greate_border_left
	{
		oColor.a = 1.0 - vPos.z;
		
		float fDistance = g_fBorderWidth + g_fSoftWidth * fSign;
		pos.x += min(0,fDistance);
		if(fDistance > 0)
			oColor.a =  fDistance / (2.0f * g_fSoftWidth);	
		
	}
	if(nType == 0) // basic
	{	
		if(vPos.z > 1.5f)
			oPos = mul(float4(pos,0.0f,1.0f),g_matWorldViewProj);	
		else if(vPos.z > 0.5f)
			oPos = mul(float4(pos,0.0f,1.0f),g_matWorldViewProjCenter);	
		else			
			oPos = mul(float4(pos,0.0f,1.0f),g_matWorldViewProjFix);	
	}
	else
		oPos = mul(float4(pos,0.0f,1.0f),g_matWorldViewProj);	
}

// r Border ------------b Blend With Current -------------g Blend With Back
void PS( float4 Color:COLOR0,
		  out float4 oColor:COLOR0,		  
		  uniform int nType)
{		
	if(nType == 0)
	{
		//oColor = 1.0f;
		oColor = float4(0.f, 0.f, 0.f, 1.f);
		return;
	}
	//oColor = 1;
	oColor = 0;
	float fAlpha = max(0.008f,Color.a + 0.008);
	if(g_bInvert)
	{
		if(nType == 1) // Border
		oColor = float4(1.0f,0.0f,0.0f,1.0f);
		else if(nType == 2) // soft_no_Border	
		oColor = float4(0.0f,1.0f,0.0f,fAlpha);  
		else if(nType == 3) // soft_Border_right
		oColor = float4(0.0f,0.0f,1.0f,fAlpha);  
		else if(nType == 4) // soft_Border	_left
		oColor = float4(0.0f,1.0f,0.0f,fAlpha);  
		else if(nType == 5) // soft_Greare_Border_right
		oColor = float4( fAlpha,0.0f,1.0f,0.0f);  
		else if(nType == 6) // soft_Greate_Border_Left	
		oColor = float4( fAlpha,1.0f,0.0f,0.0f);  
	}
	else
	{
		if(nType == 1) // Border
		oColor = float4(1.0f,0.0f,0.0f,1.0f);
		else if(nType == 2) // soft_no_Border	
		oColor = float4(0.0f,1.0f,0.0f,fAlpha);  
		else if(nType == 3) // soft_Border_right
		oColor = float4(0.0f,1.0f,0.0f,fAlpha);  
		else if(nType == 4) // soft_Border	_left
		oColor = float4(0.0f,0.0f,1.0f,fAlpha);  
		else if(nType == 5) // soft_Greare_Border_right
		oColor = float4( fAlpha,0.0f,1.0f,0.0f);  
		else if(nType == 6) // soft_Greate_Border_Left	
		oColor = float4( fAlpha,1.0f,0.0f,0.0f);  
	 } 		
}

void VS_COMPOSE(float4 vPos:POSITION,													
		float2 vTex:TEXCOORD0,
		out float4 oPos:POSITION,
		out float2 oTex:TEXCOORD0		
		)
{
	oPos = mul(float4(vPos.xy,0.0f,1.0f),g_matWorldViewProj);
	oTex = mul(float3(vTex,1.0f),g_matMask);
}
void PS_COMPOSE(float2 Tex:TEXCOORD0,out float4 oColor:COLOR0,out float depth:DEPTH,uniform bool bInvert)
{
	oColor = tex2D(g_samMask,Tex);
	
	depth = 0.9f;
	if(any(oColor.a)&&!any(oColor.rgb))		
		discard;
		
	if(bInvert)
	{
		if(any(oColor.g))
			depth = 0.2f + oColor.a / 10.0f;
		else if(any(oColor.r))
		{
			depth = 0.4f + oColor.r / 10.0f;
			if(any(oColor.b))
				depth = 0.7f - oColor.a / 10.0f;
		}
		else if(any(oColor.b))
			depth = 0.7f - oColor.a / 10.0f;
	}
	else
	{
		if(any(oColor.b))
			depth = 0.2f + oColor.a / 10.0f;
		else if(any(oColor.r))
		{
			depth = 0.4f + oColor.r / 10.0f;
			if(any(oColor.g))
				depth = 0.7f - oColor.a / 10.0f;
		}
		else if(any(oColor.g))
			depth = 0.7f - oColor.a / 10.0f;
	}
}

technique Mask
{
	PASS Shape //0
	{
		VertexShader=compile vs_1_1 VS(0);
		PixelShader=compile  ps_2_0 PS(0);
		CullMode=None;	
		ZEnable = False;
		ZWriteEnable = False;			
		//FillMode = WireFrame;
	}
	PASS Border //1 
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(1);
		CullMode=None;	
		ZEnable = False;
		ZWriteEnable = False;
		StencilEnable = True;
		StencilFunc = Always;				
		StencilPass = Incr;			
		//FillMode = WireFrame;
	}
	PASS Soft_No_Border //2
	{
		VertexShader=compile vs_1_1 VS(2);
		PixelShader=compile  ps_2_0 PS(2);
		CullMode=None;	
		
		ZEnable = False;
		ZWriteEnable = False;
			
		ShadeMode = Gouraud;
		StencilEnable = True;
		StencilFunc = Always;				
		StencilPass = Incr;		
		//FillMode = WireFrame;
	}
	PASS Soft_Border_Right //3
	{
		VertexShader=compile vs_1_1 VS(3);
		PixelShader=compile  ps_2_0 PS(3);
		CullMode=None;		
		ShadeMode = Gouraud;	
		StencilEnable = True;
		StencilFunc = Always;				
		StencilPass = Incr;		
		//FillMode = WireFrame;
	}
	PASS Soft_Border_Left //4
	{
		VertexShader=compile vs_1_1 VS(4);
		PixelShader=compile  ps_2_0 PS(4);
		CullMode=None;		
		ShadeMode = Gouraud;	
		StencilEnable = True;
		StencilFunc = Always;				
		StencilPass = Incr;		
		//FillMode = WireFrame;
	}
	
	PASS Soft_Greate_Border_Right //5
	{
		VertexShader=compile vs_1_1 VS(5);
		PixelShader=compile  ps_2_0 PS(5);
		CullMode=None;		
		ShadeMode = Gouraud;	
		ColorWriteEnable = 1;
		StencilEnable = True;
		StencilFunc = Always;				
		StencilPass = Incr;			
		//FillMode = WireFrame;
	}
	
	PASS Soft_Greate_Border_Left //6
	{
		VertexShader=compile vs_1_1 VS(6);
		PixelShader=compile  ps_2_0 PS(6);
		CullMode=None;
		ShadeMode = Gouraud;			
		ColorWriteEnable = 1;
		StencilEnable = True;
		StencilFunc = Always;				
		StencilPass = Incr;		
		//FillMode = WireFrame;
	}
	
}

technique Compose
{
	PASS P0
	{
		VertexShader=compile vs_1_1 VS_COMPOSE();
		PixelShader=compile  ps_2_0 PS_COMPOSE(0);
		CullMode=None;		
		//FillMode = WireFrame;
		ZEnable  =  True;
		ZFunc    = Less;
		ZWriteEnable = True;
		StencilEnable = True;
		StencilFunc = Less;
		StencilPass = Keep;	
		StencilRef  = 0;	
	}
	PASS P1
	{
		VertexShader=compile vs_1_1 VS_COMPOSE();
		PixelShader=compile  ps_2_0 PS_COMPOSE(1);
		CullMode=None;		
		//FillMode = WireFrame;
		ZEnable  =  True;
		ZFunc    = Less;
		ZWriteEnable = True;
		StencilEnable = True;
		StencilFunc = Less;
		StencilPass = Keep;	
		StencilRef  = 0;
	}
}