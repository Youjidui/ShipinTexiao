matrix	u_matWVP;
float4	u_color;

void	VSmain(float3 iPos:POSITION, out float4 oPos:POSITION)
{
	oPos	= mul( float4(iPos,1.f), u_matWVP );
}

float4	PSmain(float4 iColor:COLOR):COLOR
{
	return u_color;
}

technique main
{
	pass P0
	{
		VertexShader = compile vs_3_0	VSmain();
		PixelShader = compile ps_3_0	PSmain();	
		
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
}