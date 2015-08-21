T_Color BlendM( float4 bColor,float4 mAlpha,float4 tTolor,float4 tAlpha )
{
  T_Color ret;
  ret.tAlpha     =  mAlpha;
  ret.tColor     =  bColor + ( tTolor-bColor )*tAlpha/mAlpha;     
  return ret;
}

T_Color BlendC( float4 bColor,float4 bAlpha,float4 tTolor,float4 tAlpha )
{
  T_Color ret;
  //ret.tAlpha     =  any(tAlpha)?max(bAlpha,tAlpha):bAlpha; 
  //float4 ttColor =  bSourceBlended?tTolor:tTolor*tAlpha;
  //bColor         = any(bAlpha)?bColor*bAlpha:0.0;
  //ret.tColor     =  any(tAlpha)?ttColor+bColor*(1.0-tAlpha):bColor;
  
  ret.tAlpha     =  bAlpha+tAlpha-bAlpha*tAlpha;
  float4 ttColor =  bSourceBlended?(any(tAlpha)?tTolor:0.0):tTolor*tAlpha;
  ret.tColor     =  any(ret.tAlpha)?bColor + ( ttColor-bColor*tAlpha)/ret.tAlpha:0.0;     
  return ret;
}


T_Color Blend( float4 bColor,float4 bAlpha,float4 tTolor,float4 tAlpha )
{
  T_Color ret;
  ret.tAlpha     =  bAlpha+tAlpha-bAlpha*tAlpha;
  ret.tColor     =  any(ret.tAlpha)?bColor + ( tTolor-bColor )*tAlpha/ret.tAlpha:0.0;     
  return ret;
}

