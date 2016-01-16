float4x4 g_matWorldViewProj;

float   g_fBorderWidth;
float   g_fSoftWidth;
float4  g_vNormal;
float4  g_vStencil;
#define g_nStencilFunc int(g_vStencil.x)
#define g_nStencilRef  int(g_vStencil.y)


void VS(float4 vPos:POSITION,													
				out float4 oPos:POSITION,
				out float4 oColor:COLOR0,				
				uniform int nType
				)
{
	oColor = 0;	
	
	float2 pos = vPos.xy;
	if(nType == 1) // border 
	{
		pos.y += g_fBorderWidth * (vPos.z > 0?1.0f : -1.0f);
	}
	else if(nType == 2) //  soft_no_border
	{
		oColor.a = 1.0f - vPos.z;
		
		pos.y += g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
	}
	else if(nType == 3) // soft_border_up
	{
		oColor.a = 1.0f - vPos.z;
		
		float fDistance = g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.y += max(0,fDistance);
		if(fDistance < 0)
			oColor.a = 1.0f + fDistance / (2.0f * g_fSoftWidth);
	}
	else if(nType == 4) // soft_border_Down
	{
		float fDistance =  -g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.y += min(0,fDistance);
		oColor.a = vPos.z;
		if(fDistance > 0)
			oColor.a = 1.0f  - fDistance  / (2.0f * g_fSoftWidth);
	}
	else if(nType == 5) // soft_Greate_border_up
	{	
		float fDistance =  -g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.y += max(0,fDistance);
		oColor.a = vPos.z;
		if(fDistance < 0)
			oColor.a = - fDistance  / (2.0f * g_fSoftWidth);
	}
	else if(nType == 6) // soft_Greate_border_Down
	{
		oColor.a = 1.0f - vPos.z;
		
		float fDistance = g_fBorderWidth + g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
		pos.y += min(0,fDistance);
		if(fDistance > 0)
			oColor.a =  fDistance / (2.0f * g_fSoftWidth);	
		
	}
	oPos = mul(float4(pos,0.0f,1.0f),g_matWorldViewProj);		
}	

// r Border ------------b Blend With Current -------------g Blend With Back
float4 PS(float2 Tex:TEXCOORD0,
		  float4 Color:COLOR0,
		  uniform int nType):COLOR0
{
	float4 retColor = float4(0.0f,0.0f,0.0f,1.0f);
	
	if(nType == 1) // Border
	  retColor = float4(1.0f,0.0f,0.0f,1.0f);
	else if(nType == 2) // soft_no_Border	
	  retColor = float4(0.0f,0.0f,1.0f,Color.a + 0.004f);  
	else if(nType == 3) // soft_Border_up
	  retColor = float4(0.0f,1.0f,0.0f,Color.a + 0.004f);  
	else if(nType == 4) // soft_Border	_Down
	  retColor = float4(0.0f,0.0f,1.0f,Color.a + 0.004f);  
	else if(nType == 5) // soft_Greare_Border_up
	  retColor = float4( Color.a + 0.004f,0.0f,1.0f,0.0f);  
	else if(nType == 6) // soft_Greate_Border_Down	
	  retColor = float4( Color.a + 0.004f,1.0f,0.0f,0.0f);  
	  
	return retColor;
}

technique Mask
{
	PASS Shape //P0
	{
		VertexShader=compile vs_1_1 VS(0);
		PixelShader=compile  ps_2_0 PS(0);
		CullMode=None;	
		ZEnable = False;
		ClipPlaneEnable = 0;
		
		StencilEnable = False;		
		Stencilfunc = Less;	
		StencilRef = 1;
		StencilPass = Keep;
		ColorWriteEnable = 0x0f;
		//FillMode = WireFrame;
	}
	PASS Border //P1
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(1);
		CullMode=None;	
		StencilEnable = True;		
		Stencilfunc = <g_nStencilFunc>;	
		StencilRef = <g_nStencilRef>;	
		StencilPass = Keep;
		ColorWriteEnable = 0x0f;
		//FillMode = WireFrame;
	}
	PASS Soft_No_Border //P2
	{
		VertexShader=compile vs_1_1 VS(2);
		PixelShader=compile  ps_2_0 PS(2);
		CullMode=None;
		ShadeMode = Gouraud;		
		StencilEnable = True;		
		Stencilfunc = <g_nStencilFunc>;	
		StencilRef = <g_nStencilRef>;	
		StencilPass = Keep;
		ColorWriteEnable = 0x0f;
		//FillMode = WireFrame;
	}
	PASS Soft_Border_up //P3
	{
		VertexShader=compile vs_1_1 VS(3);
		PixelShader=compile  ps_2_0 PS(3);
		CullMode=None;
		ShadeMode = Gouraud;
		StencilEnable = True;		
		Stencilfunc = <g_nStencilFunc>;	
		StencilRef = <g_nStencilRef>;		
		StencilPass = Keep;
		ColorWriteEnable = 0x0f;	
		//FillMode = WireFrame;
	}
	PASS Soft_Border_Down //P4
	{
		VertexShader=compile vs_1_1 VS(4);
		PixelShader=compile  ps_2_0 PS(4);
		CullMode=None;
		ShadeMode = Gouraud;	
		StencilEnable = True;		
		Stencilfunc = <g_nStencilFunc>;	
		StencilRef = <g_nStencilRef>;	
		StencilPass = Keep;
		ColorWriteEnable = 0x0f;
		//FillMode = WireFrame;
	}
	PASS Soft_Great_Border_up //P5
	{
		VertexShader=compile vs_1_1 VS(5);
		PixelShader=compile  ps_2_0 PS(5);
		CullMode=None;
		ShadeMode = Gouraud;			
		ColorWriteEnable = 1;
		StencilEnable = True;		
		Stencilfunc = <g_nStencilFunc>;	
		StencilRef = <g_nStencilRef>;	
		StencilPass = Keep;		
		//FillMode = WireFrame;
	}
	PASS Soft_Great_Border_Down //P6
	{
		VertexShader=compile vs_1_1 VS(6);
		PixelShader=compile  ps_2_0 PS(6);
		CullMode=None;
		ShadeMode = Gouraud;		
		ColorWriteEnable = 1;
		StencilEnable = True;		
		Stencilfunc = <g_nStencilFunc>;	
		StencilRef = <g_nStencilRef>;	
		StencilPass = Keep;		
		//FillMode = WireFrame;
	}
	PASS Stencil //P7
	{
		VertexShader=compile vs_1_1 VS(0);
		PixelShader=compile  ps_2_0 PS(0);
		CullMode=None;		
		ColorWriteEnable = 0;
		StencilEnable = True;
		StencilPass = Incr;
		Stencilfunc = Always;
		ClipPlaneEnable = ClipPlane0;
		//FillMode = WireFrame;
	}
}
