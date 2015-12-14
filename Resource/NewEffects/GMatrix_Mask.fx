float4x4 g_matWorldViewProj;

float4	g_vBorder;
float4	g_vSoft;

float4 g_vMisc;
#define	g_nTileNum		round(g_vMisc.x)
#define g_fOffset		g_vMisc.y

texture g_txSequence;
sampler g_samSequence=
sampler_state
{	
	Texture=<g_txSequence>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};

void VS(float4 vPos:POSITION,													
				out float4 oPos:POSITION,
				out float2 oTex:TEXCOORD0				
				)
{	
	oPos = mul(float4(vPos.xy,0.0f,1.0f),g_matWorldViewProj);	
	
	oTex = (oPos / oPos.w).xy;
	oTex = (oTex + 1.0f) / 2.0f;
	oTex.y = 1.0f - oTex.y;	
}	

float4 PS(float2 Tex:TEXCOORD0,		  		  
		  uniform int nType):COLOR0
{
	float4 retColor = float4(0.0f,0.0f,0.0f,1.0f);
	
	int x = floor(Tex.x * g_nTileNum);
	int y = floor(Tex.y * g_nTileNum);
	float2 Tex1 = float2((x + 0.5f) / 20.0f,(y + 0.5f) / 20.0f);
	float nSequence = round(tex2D(g_samSequence,Tex1).r) + 1.0f - frac(g_fOffset);
	
	if(nType == 0) //normal
	{		
		retColor.a = lerp(0,1,nSequence - floor(g_fOffset));
		retColor.g = sign(retColor.a);
	}	
	else if(nType == 1) //Only border
	{		
		if(nSequence <= g_vBorder.z)
		{
			retColor.a = lerp(0,1,nSequence - g_vBorder.x);
			if(retColor.a > 1.0f / 255.0f)
			{
				retColor.rg = 1.0f;
			}
			else
				retColor = 0.0f;		
		}
		else
		{
			retColor.a = lerp(1.0f,0.0f,nSequence - g_vBorder.y);
			if(retColor.a > 1.0f / 255.0f)
			{				
				retColor.rb = 1.0f;				
			}		
			else
				retColor.a = 1.0f;
		}			
	}	
	else if(nType == 2) //Only soft
	{				
		retColor.a = lerp(0,1,(nSequence  - g_vSoft.x) / (g_vSoft.y - g_vSoft.x));					
		retColor.g = 1.0;			
	}	
	else if(nType == 3) //Border > soft
	{
		if(nSequence <= g_vBorder.z)
		{
			retColor.a = lerp(0,1,(nSequence - (g_vBorder.x - g_vSoft.z)) / (2.0f * g_vSoft.z + 1.0f));
			if(retColor.a > 1.0f / 255.0f)
			{
				retColor.rg = 1.0f;
			}				
			else
				retColor = 0.0f;	
		}
		else
		{
			retColor.a = lerp(1.0f,0.0f,(nSequence - (g_vBorder.y - g_vSoft.z)) / (2.0f * g_vSoft.z + 1.0f));
			if(retColor.a > 1.0f / 255.0f)
			{				
				retColor.rb = 1.0f;				
			}		
			else
				retColor.a = 1.0f;
		}		
	}	
	else if(nType == 4) //Border < soft
	{
		if(nSequence <= g_vBorder.z)
		{
			retColor.r = lerp(1.0f,0.0f,(nSequence - (g_vBorder.y - g_vSoft.z)) / (2.0f * g_vSoft.z + 1.0f));
			if(retColor.r >= 1.0f)			
				discard;
		}
		else
		{					
			retColor.r = lerp(0,1,(nSequence - (g_vBorder.x - g_vSoft.z)) / (2.0f * g_vSoft.z + 1.0f));
			if(retColor.r >= 1.0f)			
				discard;	
		}		
	}	
	return retColor;
}

technique Mask
{
	PASS Shape //0
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS(0);
		CullMode=None;
		ZEnable = False;		
		//FillMode = WireFrame;
	}
	PASS Only_Border //1 
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS(1);
		CullMode=None;
		ZEnable = False;
		//FillMode = WireFrame;
	}
	PASS Only_Soft //2
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS(2);
		CullMode=None;
		ZEnable = False;		
		//FillMode = WireFrame;
	}
	PASS Soft_Bordert //3
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS(3);
		CullMode=None;
		ZEnable = False;		
		//FillMode = WireFrame;
	}	
	PASS Soft_Bordert //4
	{
		VertexShader=compile vs_1_1 VS();
		PixelShader=compile  ps_2_0 PS(4);
		CullMode=None;
		ZEnable = False;		
		ColorWriteEnable = 1;
		//FillMode = WireFrame;
	}	
}
