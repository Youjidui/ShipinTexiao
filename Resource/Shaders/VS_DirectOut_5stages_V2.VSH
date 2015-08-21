float4x4 matWVP             : register(c0);
float4x4 matTex_stage0      : register(c4);
float4x4 matTex_stage1      : register(c8);
float4x4 matTex_stage2      : register(c12);
float4x4 matTex_stage3      : register(c16);
float4x4 matTex_stage4      : register(c20);

struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float2 UV_stage0  : TEXCOORD0;
    float2 UV_stage1  : TEXCOORD1;
    float2 UV_stage2  : TEXCOORD2;
    float2 UV_stage3  : TEXCOORD3;
    float2 UV_stage4  : TEXCOORD4;
};

VS_OUTPUT main( float3 varPosition     : POSITION,
                float2 varTextureCoord : TEXCOORD )
{
    VS_OUTPUT Output;
    Output.UV_stage0 = mul( matTex_stage0, float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.UV_stage1 = mul( matTex_stage1, float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.UV_stage2 = mul( matTex_stage2, float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.UV_stage3 = mul( matTex_stage3, float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.UV_stage4 = mul( matTex_stage4, float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.Position = mul( matWVP, float4(varPosition,1.0) );
    return Output;
}