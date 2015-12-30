float4x4 g_matOriginal;
float4x4 g_matBackGround;
float4x4 g_matTexture;
float4x4 g_matShadow;
float4x4 g_matBorder;
float4x4 g_matViewProj;
//float4x4 g_matBorderOuter;
float    g_ShadowSoftness;
float    g_ShadowTransparency;
float    g_BGDensity;
float4   g_SepiaColor;
float4   g_BorderColor;
float4   g_ShadowColor;
float    g_fInstanceCount;
int		 g_nFilter;

float4   g_vDir;
float4   g_vMisc;

#define g_fDencity	g_vMisc.x
#define g_fDistance	g_vMisc.y
#define g_fDust     g_vMisc.z
#define g_fRainbow  g_vMisc.w
#define g_fCount    g_vDir.z

#define _FX
#include "..\Shaders\ColorSpaceFunc.INC"

int      g_iBGType;
texture  g_tex;
texture  g_texNoise;

float3 grayXfer = {0.2988, 0.5868, 0.1144};
float4 mode = {0.f,0.5f,0.5f,0.f};
float4 black_yuva = {0.f,0.5f,0.5f,0.f};
sampler g_samColor=
sampler_state
{	
	Texture=(g_tex);
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=<g_nFilter>;
	AddressU =Clamp;
	AddressV =Clamp;
};

sampler g_samNoise=
sampler_state
{	
	Texture=<g_texNoise>;
	MinFilter=Point;
	MagFilter=Point;
	MipFilter=Point;
	AddressU=Mirror;
	AddressV=Wrap;
};

void VS(float3 vPos:POSITION,				
				float2 vTex:TEXCOORD0,				
				out float4 oPos:POSITION,	
				out float3 oTex:TEXCOORD0,				
				out float2 srcTex:TEXCOORD1,
				uniform bool bBackground,
				uniform bool bShadow,
				uniform bool bBorder,			
				uniform bool bOriginal
			    )
{	 	
  
  float4x4 matTrans ;
  oTex.z = 1.0f;
   if(bBorder)
   {
       if(vPos.z < 1.0f)
       {             
           matTrans = g_matBorder;                    
        }   
        else
        {
           matTrans = g_matOriginal;
           oTex.z = 0.0f;
         }
    }   
	 if(bShadow)
	    matTrans = g_matShadow;
	 if(bBackground)
	    matTrans = g_matBackGround;
	 if(bOriginal)
	    matTrans = g_matOriginal;
      
  	 oPos = mul(float4(vPos.xy,0.0f,1.f),matTrans);
	 oTex.xy = mul(float4(vTex,1.0f,0.0f),g_matTexture);	 
     srcTex.x = vTex.x - 0.5f;
     srcTex.y = 0.5f - vTex.y;
      
}	
 
////////////    original scene;
//////////////////////////////////////////////////
float4 PS_ORG(float3 Tex:TEXCOORD0,uniform bool bBorder):COLOR0
{	
	float4 oColor=(float4)0;
	
	if(bBorder)		
	{	
		//if(Tex.z > 0.0f)
			oColor = g_BorderColor;
		//else			
		//	oColor = tex2D(g_samColor,Tex); 	
	}			
	else 
	   oColor = tex2D(g_samColor,Tex); 	
	   
	 //if(oColor.a < 1.0f / 255.0f)
		//discard;
	
	return oColor;
}


////////////////////////////////////////////////////////////////
/////////////   shadow scene
float4 PS_SHADOW(float2 orgTex:TEXCOORD0, float2 iTex:TEXCOORD1,uniform int nPattern):COLOR0
{    
	float4 color = g_ShadowColor;
	if(nPattern < 2)
		color.a = tex2D(g_samColor,orgTex).a;
    color.a *= g_ShadowTransparency;
    
    if(nPattern != 1)
    {
		float outer = 0.5f;
		float inner = outer - g_ShadowSoftness*0.5;
  
		iTex = abs(iTex);
		float2 softedge_xy = 1.0 - smoothstep(inner,outer, iTex);
		float softedge = sqrt( softedge_xy.x * softedge_xy.y ); 
     
		color.a *= softedge;                                   
    }  

   return color; 
 }

////////////////////////////////////////////////////////////
/////////   background scene                      

float4 PS_BACKGROUND(float2 iTex: TEXCOORD0, uniform bool bYUV):COLOR0
{
   
    float4 oColor;
  
    float3 Mono = g_SepiaColor.rgb;

    float4 scnColor = tex2D(g_samColor,iTex) ;
    if(bYUV)
    {
      scnColor -= mode;
      scnColor.rgb = ColorSpace_YUVToRGB(scnColor.rgb);
    }		
    
    if(g_iBGType == 1)
       oColor = scnColor;
    if(g_iBGType == 2)
    {
          Mono = 0;          
    }              
    if(g_iBGType == 2 || g_iBGType == 3)
    {
     float gray = dot(grayXfer, scnColor.rgb);
     float SepiaMono = dot(grayXfer,Mono);
     float3 result = 0;
       if( gray == 0 )
          result = 0;
       else if( gray == 1 )
          result = 1;
       else
     //   result = lerp(gray.xxx, Mono, 0.5 );      
       {  
           //result = gray + (Mono - gray)*0.5f;
           result = 1.f - (1.f - Mono) * (1.f - gray.xxx);
       //   if(gray <= SepiaMono)
         //      result = Mono * gray/SepiaMono;
         //  else result = 1.f - (1.f - Mono) * (1.f - gray)/SepiaMono  ;
          
        } 
       oColor = float4(result, scnColor.a ); 

     }
     if(bYUV)
     {
        oColor.rgb = ColorSpace_RGBToYUV(oColor.rgb) ;
        oColor += mode;
     }
    
     oColor.a *=  g_BGDensity;
     
     return oColor ; 
}
//////////////////////////////////////////////////
//// decay
void VS_DECAY(float3 vPos:POSITION,
              float2 vTex:TEXCOORD0,
              float  vInstance:TEXCOORD1,
              out float4 oPos:POSITION,
              out float3 oTex:TEXCOORD0              
              )
              
{   
     float2 pos = g_vDir.xy * ( (g_fInstanceCount - vInstance) * g_fDistance) ; 
     oPos = mul(float4(vPos.xy +pos,0.f,1.0f),g_matViewProj);
     oTex = mul(float3(vTex,1.f), g_matTexture);
     oTex.z = vInstance;      
}             

void PS_DECAY(float3 iTex: TEXCOORD0,
                   out float4 oColor:COLOR0,                   
                   uniform bool bYUV)
{	
    oColor =tex2D(g_samColor,iTex.xy);     
    if(bYUV)
          oColor -= mode;    
 
	if(oColor.a > 1.0f / 255.0f)
		oColor.rgb /= oColor.a;
	oColor.a *= smoothstep(0,g_fInstanceCount,iTex.z) + 1.0f / 255.0f;	
	
	if(g_fDust > 0.0f && round(g_fInstanceCount -  iTex.z) > 0.0f)
	{
		float4 fNoise =tex2D(g_samNoise,iTex.xy * (5 + iTex.z));
		if(fNoise.r - 0.4f  < 1.0 - oColor.a)		
			oColor = cYUVAMode;
	}
	
  if(bYUV)
       oColor += mode;  	 
 	  	
}
technique ORG
{
	
	PASS P0  /// draw original
    {
          Vertexshader = compile vs_2_0 VS(false,false,false,true);
	      PixelShader = compile ps_2_0 PS_ORG(false);
	      AlphaBlendEnable = false;
	      
	      ZEnable = True;
	      ZFunc = Always;
	      ZWriteEnable = True;
	      //ScissorTestEnable = True;
    } 
    PASS P1  /// Pos
    {
          Vertexshader = compile vs_2_0 VS(false,false,false,true);
	      PixelShader = compile ps_2_0 PS_ORG(false);	 	     
	     
	       AlphaBlendEnable = True;
	       SrcBlend = SrcAlpha;
	       DestBlend = InvSrcAlpha;
	      
	      ZEnable = True;
	      ZFunc = Always;
	      ZWriteEnable = True;
	      //ScissorTestEnable = True;     
    } 
}
technique Border
{
     PASS P0   // draw border
	 {  
	      Vertexshader = compile vs_2_0 VS(false,false,false,true);
	      PixelShader = compile ps_2_0 PS_ORG(true);
	      AlphaBlendEnable = false;
	      cullmode = none;
	      
	       ZEnable = True;
	      ZFunc = Less;
	      ZWriteEnable = False;
	   //   FillMode = WIREFRAME;
	      
	 }   
}

technique Shadow
{   
	PASS  NoBorder//P1
	{
	     VertexShader=compile vs_2_0 VS(false,true,false,false);
		 PixelShader=compile  ps_2_0 PS_SHADOW(0);
	     AlphaBlendEnable = True;	   
	     SrcBlend = SrcAlpha;
	     DestBlend = INVSRCALPHA;	      	
	}	 
	
	PASS  Border_Front//P2
	{
	     VertexShader=compile vs_2_0 VS(false,true,false,false);
		 PixelShader=compile  ps_2_0 PS_SHADOW(1);
	     AlphaBlendEnable = True;	   
	     SrcBlend = SrcAlpha;
	     DestBlend = INVSRCALPHA;
	     
	      ZEnable = True;
	      ZFunc = Always;		
	}	 
	PASS  Border//P3
	{
	     VertexShader=compile vs_2_0 VS(false,true,false,false);
		 PixelShader=compile  ps_2_0 PS_SHADOW(2);
		 
	     AlphaBlendEnable = True;	   
	     SrcBlend = SrcAlpha;
	     DestBlend = INVSRCALPHA;		     
	     
	     ZEnable = True;
	     ZFunc = Less;
	     ZWriteEnable = False;	
	}	 
}

technique BackGround
{
    PASS RGBA//P0
	{
	     VertexShader=compile vs_2_0 VS(true,false,false,false);
		 PixelShader=compile  ps_2_0 PS_BACKGROUND(false);
		 AlphaBlendEnable = False;
	     SrcBlend = SrcAlpha;
	     DestBlend = INVSRCALPHA;
	 	     
	}
	PASS YUVA//P1
	{
	     VertexShader=compile vs_2_0 VS(true,false,false,false);
		 PixelShader=compile  ps_2_0 PS_BACKGROUND(true);
		 AlphaBlendEnable = False;
	     SrcBlend = SrcAlpha;
	     DestBlend = INVSRCALPHA;
	 	     
	}
}

technique Decay
{
     PASS RGBA//P0
     {
         VertexShader=compile vs_2_0 VS_DECAY();		 
		 PixelShader=compile  ps_2_0 PS_DECAY(false);
		 AlphaBlendEnable = True ;
	     SrcBlend = SrcAlpha;
	     DestBlend = INVSRCALPHA;
// 		 ZWriteEnable = true;
 	     ZEnable = false;
//          ZFunc = Less;	  
	     cullmode = none;
     }
     PASS YUVAA//P0
     {
         VertexShader=compile vs_2_0 VS_DECAY();		 
		 PixelShader=compile  ps_2_0 PS_DECAY(true);
		 AlphaBlendEnable = True ;
	     SrcBlend = SrcAlpha;
	     DestBlend = INVSRCALPHA;
// 		 ZWriteEnable = true;
 	     ZEnable = false;
//         ZFunc = Less;	  
	     cullmode = none;
     }
}