float4x4 matWVP             : register(c0);
float4x4 matTex             : register(c4);//这个矩阵应该是Indentity矩阵

struct VS_OUTPUT
{
    float4 oPosition   : POSITION;      // vertex position
    float2 oUV0        : TEXCOORD0;
    float2 oUV1        : TEXCOORD1;
    float2 oUV2        : TEXCOORD2;
    float2 oUV3        : TEXCOORD3;
    float2 oUV4        : TEXCOORD4;
    float2 oUV5        : TEXCOORD5;
    float2 oUV6        : TEXCOORD6;
    float2 oUV7        : TEXCOORD7;
    
};

VS_OUTPUT main( float3 varPos          : POSITION,
                float2 varTex0         : TEXCOORD0 )
                
{
    VS_OUTPUT Output;
    Output.oPosition =  mul( matWVP, float4(varPos, 1.0) );
    Output.oUV0      =  mul( matTex, float4(varTex0, 1.0, 0.0) ).xy;
    Output.oUV1 = Output.oUV0;
    Output.oUV2 = Output.oUV0;
    Output.oUV3 = Output.oUV0;
    Output.oUV4 = Output.oUV0;
    Output.oUV5 = Output.oUV0;
    Output.oUV6 = Output.oUV0;
    Output.oUV7 = Output.oUV0; 
    return Output;
}
