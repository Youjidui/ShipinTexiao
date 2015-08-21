struct T_Color 
{
   float4 tColor;//Color
   float4 tAlpha;//Alpha
};

/*T_Color Color_Blend( float4 bColor,float4 bAlpha,float4 tColor,float4 tAlpha  )
{
  T_Color ret;
  ret.tAlpha     =  bAlpha+tAlpha-bAlpha*tAlpha;
  tColor         =  sBlended?tColor:tColor*tAlpha;
  ret.tColor     =  any(ret.tAlpha)?bColor + ( tColor-bColor*tAlpha)/ret.tAlpha:0.0;
  return ret;
}*/

T_Color Color_Blend( float4 bColor,float4 bAlpha,float4 tColor,float4 tAlpha,bool pBlendMode )
{
  T_Color ret;
  tColor         = pBlendMode?tColor:tColor*sign(tAlpha);
  ret.tAlpha     =   bAlpha+tAlpha-bAlpha*tAlpha;   
  ret.tColor     =    bColor*(1.0-tAlpha)+ tColor; 
  ret.tColor     =   any(tAlpha)?ret.tColor:bColor+ tColor;  
  return ret;
}

T_Color Color_BlendA( float4 bColor,float4 bAlpha,
                      float4 tColor,float4 tAlpha,
                      bool pBlendMode,float4 tColorOrg )
{
  T_Color ret;
  tColor         =   pBlendMode?tColor:tColor*sign(tAlpha);
  ret.tAlpha     =   bAlpha+tAlpha-bAlpha*tAlpha;   
  float4 blendA  =   bColor*(1.0-tAlpha)+tColor;
  float4 blendP  =   tColor+bColor;   
  ret.tColor     =   any(tAlpha)?blendA:blendP;  
  return ret;
}