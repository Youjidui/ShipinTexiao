float4x4 g_matWVP;
float4x4 g_matTex;
float4x4 g_matTexRote;
float4x4 g_matTexRoteR;

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

texture g_txShape;

#ifdef __NV8__
sampler g_samShape = 
sampler_state
{	
	Texture = <g_txShape>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = Wrap;
	AddressV = Wrap;
};
#else
sampler g_samShape = 
sampler_state
{	
	Texture = <g_txShape>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
	AddressU = Wrap;
	AddressV = Wrap;
};
#endif

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
};

float4		g_vRings;
#define		g_fTranslate		g_vRings.x
#define		g_fRandomTranslate	g_vRings.y
#define		g_fAlpha			g_vRings.z

float4		g_vMisc;
#define		g_fTextYOffset		g_vMisc.x
#define		g_fAspect			g_vMisc.y
#define		g_vTexCenter		g_vMisc.zw

float4		g_vBlock;
#define		g_fWidth			g_vBlock.x
#define		g_fRandomWidth		g_vBlock.y
#define		g_fSpiral			g_vBlock.z
#define		g_fRandomPixel		g_vBlock.w

#define PI	3.1415926f

void VS(float4 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0
				)
{	
	oPos = mul(vPos,g_matWVP);	
	oTex = vTex;	
}	

float GetShapeR(float fAngle)
{
	fAngle *= -1.0f;	
	#ifdef __NV8__
		float  fLen  = length(tex2D(g_samShape,float2((fAngle + radians(360.0f)) /  radians(360.0f) + 0.5f / 1800.0f ,g_fTextYOffset)).xy );
#else
		float x = fAngle /  radians(360.0f) * 1800.0f;
		float x1 = floor(x) / 1800.0f;
		float x2 = x1 + 1.0f / 1800.0f;
		float xf = frac(x);
		float2 l1 = tex2D(g_samShape,float2(x1 + 0.5f / 1800.0f ,g_fTextYOffset)).xy;
		float2 l2 = tex2D(g_samShape,float2(x2 + 0.5f / 1800.0f ,g_fTextYOffset)).xy;
		float2 l = lerp(l1,l2,xf);
		float  fLen  = length(l);
#endif	

	return fLen;
}
float4 PS(float2 Tex:TEXCOORD0,uniform bool bAlpha,uniform bool bBrokenRings):COLOR0
{
	float4 oColor=(float4)0;
	
	float rnd;
    rnd = tex2D( g_samNoise, Tex ).x;
    
    rnd = ( rnd - 0.5f ) * 0.50f * g_fRandomPixel; 

	
	float2 fTex = Tex - 0.5f;	
	fTex.x *= g_fAspect;
	float2 tex_fx = fTex  - g_vTexCenter;	
    
    tex_fx = mul(float3(tex_fx,1.0f),g_matTexRote);	    
    
    float rr = length(tex_fx);  
    float fAngle = atan2( tex_fx.y, tex_fx.x );
	float fLen = GetShapeR(fAngle);
	
	float r = rr / fLen;	
	r += rnd;
	r /= g_fWidth;
		
	float nID = floor(r);
	float4 random = tex2D(g_samNoise,float2(nID / 50.0f,nID / 20.0f));
	
	float nID2 = nID;
	
	nID = ( r < nID2        + random.x * g_fRandomWidth ) ? nID - 1.50f : nID;
	nID = ( r > nID2 + 1.0f + random.y * g_fRandomWidth ) ? nID + 1.50f : nID;
	
	if(bBrokenRings)
	{
		fAngle = fAngle + PI;
		fAngle /= g_fWidth;
		nID += floor(fAngle);
	}
	
	fAngle = nID * g_fSpiral;
	float s,c;
	sincos(fAngle,s,c);	
	
	float2 f = (g_fTranslate + (random.z + 1.0f) / 2.0f * g_fRandomTranslate) * float2(s,c);    
        
    f = mul(float3(f,1.0f),g_matTexRoteR);
    
    float2 tex = fTex.xy + f;		
	tex.x /= g_fAspect;
	
	tex = tex + 0.5f;
	
	if(tex.x < 0.0f || tex.x >= 1.0f || tex.y < 0.0f || tex.y >= 1.0f)
		discard;
	
	tex = mul(float3(tex,1.0f),g_matTex);
	oColor = tex2D(g_samColor,tex.xy);
	
	if(bAlpha)
	{
		oColor.a *= g_fAlpha;
		oColor.rgb *= oColor.a;
	}
		
	return oColor;
}

technique Rings
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS(false,false);
		CullMode = None;
		ZEnable = False;
	}
}

technique RingsTrans
{	
	PASS Trans //p0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS(true,false);
		CullMode = None;
		ZEnable = False;
		AlphablendEnable = True;
		SrcBlend = One;
		DestBlend = InvSrcAlpha;
	}	
}
technique BrokenRings
{
	PASS P0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS(false,true);
		CullMode = None;
		ZEnable = False;
	}
}

technique BrokenRingsTrans
{	
	PASS Trans //p0
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile  ps_3_0 PS(true,true);
		CullMode = None;
		ZEnable = False;
		AlphablendEnable = True;
		SrcBlend = One;
		DestBlend = InvSrcAlpha;
	}	
}