float4x4 g_matWorldViewProj;

float   g_fBorderWidth;
float   g_fSoftWidth;

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
		pos.x += g_fBorderWidth * (vPos.z > 0?1.0f : -1.0f);
	}
	else if(nType == 2) //  soft_no_border
	{
		oColor.a = vPos.z;
		
		pos.x += g_fSoftWidth * (vPos.z > 0?1.0f : -1.0f);
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
	
}	

float4 PS(float2 Tex:TEXCOORD0,
		  float4 Color:COLOR0,
		  uniform int nType):COLOR0
{
	float4 retColor = 0;	//original code
	
	if(nType == 1) // Border
	  retColor = float4(1.0f,0.0f,0.0f,1.0f);
	else if(nType == 2) // soft_no_Border	
	  retColor = float4(0.0f,0.0f,1.0f,Color.a + 0.004f);  
	else if(nType == 3) // soft_Border_right
	  retColor = float4(0.0f,0.0f,1.0f,Color.a + 0.004f);  
	else if(nType == 4) // soft_Border	_left
	  retColor = float4(0.0f,1.0f,0.0f,Color.a + 0.004f);  
	else if(nType == 5) // soft_Greare_Border_right
	  retColor = float4( Color.a + 0.004f,0.0f,1.0f,0.0f);  
	else if(nType == 6) // soft_Greate_Border_Left	
	  retColor = float4( Color.a + 0.004f,1.0f,0.0f,0.0f);  
	  
	return retColor;
}

technique Mask
{
	PASS Shape //0
	{
		VertexShader=compile vs_1_1 VS(0);
		PixelShader=compile  ps_2_0 PS(0);
		CullMode=None;
		ScissorTestEnable = True;
		//FillMode = WireFrame;
	}
	PASS Border //1 
	{
		VertexShader=compile vs_1_1 VS(1);
		PixelShader=compile  ps_2_0 PS(1);
		CullMode=None;
		ScissorTestEnable = True;
		//FillMode = WireFrame;
	}
	PASS Soft_No_Border //2
	{
		VertexShader=compile vs_1_1 VS(2);
		PixelShader=compile  ps_2_0 PS(2);
		CullMode=None;
		ScissorTestEnable = True;
		ShadeMode = Gouraud;
		//FillMode = WireFrame;
	}
	PASS Soft_Border_Right //3
	{
		VertexShader=compile vs_1_1 VS(3);
		PixelShader=compile  ps_2_0 PS(3);
		CullMode=None;
		ScissorTestEnable = True;
		ShadeMode = Gouraud;	
		//FillMode = WireFrame;
	}
	PASS Soft_Border_Left //4
	{
		VertexShader=compile vs_1_1 VS(4);
		PixelShader=compile  ps_2_0 PS(4);
		CullMode=None;
		ScissorTestEnable = True;
		ShadeMode = Gouraud;	
		//FillMode = WireFrame;
	}
	
	PASS Soft_Greate_Border_Right //5
	{
		VertexShader=compile vs_1_1 VS(5);
		PixelShader=compile  ps_2_0 PS(5);
		CullMode=None;
		ScissorTestEnable = True;
		ShadeMode = Gouraud;	
		ColorWriteEnable = 1;	
		//FillMode = WireFrame;
	}
	
	PASS Soft_Greate_Border_Left //6
	{
		VertexShader=compile vs_1_1 VS(6);
		PixelShader=compile  ps_2_0 PS(6);
		CullMode=None;
		ScissorTestEnable = True;
		ShadeMode = Gouraud;			
		ColorWriteEnable = 1;
		//FillMode = WireFrame;
	}
	
}
