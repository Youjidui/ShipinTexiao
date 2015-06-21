struct P_OUT
{
   float4 oColor : COLOR0;//Color
   float4 oAlpha : COLOR1;//Alpha
};
void BlendMode_ADDFragment(float4 sColor0:r_Color0,
                           float4 sAlpha0:r_Alpha0,
                           float4 sColor1:r_Color1,
                           float4 sAlpha1:r_Alpha1,
                           out P_OUT retColor  )
{
   retColor.oColor  =  sColor0 + sColor1;
   retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;   
}
pixelfragment BMode_ADDFragment = compile_fragment ps_2_0 BlendMode_ADDFragment();




void BlendMode_MulFragment(float4 sColor0:r_Color0,
                           float4 sAlpha0:r_Alpha0,
                           float4 sColor1:r_Color1,
                           float4 sAlpha1:r_Alpha1,
                           out P_OUT retColor  )
{
   retColor.oColor  =  sColor0 * sColor1 ;   
   retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;   
}
pixelfragment BMode_MulFragment0 = compile_fragment ps_2_0 BlendMode_MulFragment();





/*void BlendMode_AlphaBlendFragment (float4 sColor0:r_Color0,
                           float4 sAlpha0:r_Alpha0,
                           float4 sColor1:r_Color1,
                           float4 sAlpha1:r_Alpha1,
                           out P_OUT retColor  )
{   
   retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;   
   retColor.oColor  = any(retColor.oAlpha)?sColor0 + ( sColor1- sColor0)*r_Alpha1/retColor.oAlpha:0.0; 
}
pixelfragment BMode_AlphaBlend = compile_fragment ps_2_0 BlendMode_AlphaBlendFragment();

void BlendMode_ColorBurnFragment ( float4 sColor0:r_Color0,
                                   float4 sAlpha0:r_Alpha0,
                                   float4 sColor1:r_Color1,
                                   float4 sAlpha1:r_Alpha1,
                                   out P_OUT retColor  )
{   
    retColor.oColor = (float4(1.0,0.5,1.0,0.5) - sColor1 * sAlpha1 )/sColor0 * sAlpha0;
    retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;      
}
pixelfragment BMode_ColorBurn = compile_fragment ps_2_0 BlendMode_ColorBurnFragment();


void BlendMode_ColorDodgeFragment ( float4 sColor0:r_Color0,
                                    float4 sAlpha0:r_Alpha0,
                                    float4 sColor1:r_Color1,
                                    float4 sAlpha1:r_Alpha1,
                                    out P_OUT retColor  )
{   
    retColor.oColor = sColor1*sAlpha1/(float4(1.0,0.5,1.0,0.5)- sColor0*sAlpha0);;
    retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;
}

pixelfragment BMode_ColorDodge = compile_fragment ps_2_0 BlendMode_ColorDodgeFragment();

void BlendMode_DarkenFragment ( float4 sColor0:r_Color0,
                                float4 sAlpha0:r_Alpha0,
                                float4 sColor1:r_Color1,
                                float4 sAlpha1:r_Alpha1,
                                out P_OUT retColor  )
{   
    retColor.oColor  = min(sColor0*r_Alpha0,sColor1*sAlpha1);
    retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;
}

pixelfragment BMode_Darken = compile_fragment ps_2_0 BlendMode_DarkenFragment();

void BlendMode_DifferenceFragment ( float4 sColor0:r_Color0,
                                float4 sAlpha0:r_Alpha0,
                                float4 sColor1:r_Color1,
                                float4 sAlpha1:r_Alpha1,
                                out P_OUT retColor  )
{   
    retColor.oColor = sColor1*sAlpha1 - sColor0*sAlpha0;             
    retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;
}
pixelfragment BMode_Difference = compile_fragment ps_2_0 BlendMode_DifferenceFragment();

void BlendMode_DivFragment ( float4 sColor0:r_Color0,
                                float4 sAlpha0:r_Alpha0,
                                float4 sColor1:r_Color1,
                                float4 sAlpha1:r_Alpha1,
                                out P_OUT retColor  )
{     
    retColor.oColor = sColor1*sAlpha1/sColor0*sAlpha0;             
    retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;
}
pixelfragment BMode_Div = compile_fragment ps_2_0 BlendMode_DivFragment();


void BlendMode_HardLightFragment ( float4 sColor0:r_Color0,
                                   float4 sAlpha0:r_Alpha0,
                                   float4 sColor1:r_Color1,
                                   float4 sAlpha1:r_Alpha1,
                                   out P_OUT retColor  )
{     
    
   if(sColor1.x > 128.0f)
   {
      float temp = 2.0*(1.0 - sColor0.x) * (1.0 -  sColor1.x );
      retColor.oColor = min(1.0 - temp, 1.0f);
   }
   else
   {
      float temp = src1.x * (src2.x*2.0f);
      retColor.oColor = min(temp, 1.0f);
   }
   
   if(sColor1.x > 128.0f)
   {
      float temp = 2.0*(1.0 - sColor0.x) * (1.0 -  sColor1.x );
      retColor.oColor = min(1.0 - temp, 1.0f);
   }
   else
   {
      float temp = src1.x * (src2.x*2.0f);
      retColor.oColor = min(temp, 1.0f);
   }
   
   
   
    
    retColor.oColor = sColor1*sAlpha1/sColor0*sAlpha0;             
    retColor.oAlpha  = sAlpha0 + sAlpha1- sAlpha0 * sAlpha1;
}
pixelfragment BMode_HardLight = compile_fragment ps_2_0 BlendMode_HardLightFragment();*/
