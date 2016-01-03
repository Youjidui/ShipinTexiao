#include "..\Shaders\ColorSpaceFunc.inc"

matrix g_matMVP;

matrix g_matRot;

matrix g_matTex;

float4  g_aspect;

float4  g_trans;

float4  g_rot;

float4  g_texBound;
float4	g_vAlpha;
texture g_txBG;

texture g_txColor;


sampler g_samColor =
sampler_state
{	
	Texture=<g_txColor>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=Clamp;
	AddressV=Clamp;
};

sampler g_samBG =
sampler_state
{	
	Texture=<g_txBG>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Clamp;
	AddressV=Clamp;
};


void VS(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,				
				out float2 oTex:TEXCOORD0,
				out float2 rotTex:TEXCOORD1,
				out float2 posTex:TEXCOORD2
				)
{	
  
   	oPos	= mul(float4(vPos,1.f), g_matMVP);
	oTex = mul(float3(vTex,1.f),g_matTex);

	rotTex = mul(float4(vPos.xy,0.f,1.f) ,g_matRot);
	
	posTex = vPos.xy;
	posTex.x /= -g_aspect.x;	    
}	


float4 PS(float2 texCoord:TEXCOORD0,
               float2  rotCoord:TEXCOORD1,
               float2  posTex:TEXCOORD2):COLOR0
{   
   
   
   float4 color = 0;
   
 
  
  
    float2 f = ( rotCoord.x > 0.0f ) ? -g_trans.xx : g_trans.xx;
    float2 g = ( rotCoord.y < 0.0f ) ? -g_trans.yy : g_trans.yy;

    float2 tex =  g_rot.xzxz * f.xxyy + g_rot.ywyw * g.xxyy;
    
    tex.x /= -g_aspect.x;
 
    tex  += posTex; 
      
    float  key_x =  g_trans.x <= abs(rotCoord.x) ? 0.f:1.0f ;
    float  key_y =  g_trans.y <= abs( rotCoord.y )? 0.0f: 1.f ;
         
    float k = 1.0f - max( key_x, key_y );

    tex =  g_aspect.zw - tex * g_aspect.xy ;

	color = tex2D(g_samBG,texCoord) ;
	color.a *= g_vAlpha.y;
	
    if(k > 0.5f && abs(tex.x - 0.5f) < 0.5f && abs(tex.y - 0.5f) < 0.5f)
	{
		float4 colorFg = tex2D(g_samColor,mul(float3(tex,1.f),g_matTex));
		colorFg.a *= g_vAlpha.x;
			
		color.rgb *= color.a;
		color += cYUVAMode * (1.0f - color.a);
		colorFg.rgb *= colorFg.a;
		color = colorFg + color * ( 1.0f - colorFg.a);
		    
		if(color.a > 0.0f)
			color.rgb = lerp(cYUVAMode,color,1.0f / color.a);
				
	}           
        
   return color;
 
}

technique BarnSlide
{
	PASS P0
	{
		VertexShader	= compile vs_3_0 VS();
		PixelShader	= compile ps_3_0 PS();
		cullmode = none;
//		fillmode = wireframe;	
	}

}
