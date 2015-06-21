
#define IMPLEMENT_BLEND_ADDFX( nStage )\
void BlendMode_ADDFragment_Stage_##nStage( float2 texCoord:TEXCOORD0,\                            
                                           inout float4 retColor:COLOR0,\
                                           inout float4 retAlpha:COLOR1 )\
{\
   float4  sColor = tex2D(sColor##nStage,texCoord)-float4(0.0,0.5,0.0,0.5);\
   float4  sAlpha = bAlphaExisted[nStage]?tex2D(sAlpha##nStage,texCoord)*bAlphaValue[nStage]:bAlphaValue[nStage];\
   retColor  = retColor + sColor;\
   retAlpha  = sAlpha + retAlpha- retAlpha * sAlpha;\   
}\
pixelfragment BlendMode_Add_Stage_##nStage = compile_fragment ps_2_0 BlendMode_ADDFragment_Stage_##nStage();

