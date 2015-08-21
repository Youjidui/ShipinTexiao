float4x4 matWVP             : register(c0);
float4x4 matTex_stage0      : register(c4);
float4x4 matTex_stage1      : register(c8);

struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float2 UV_stage0  : TEXCOORD0;
    float2 UV_stage1  : TEXCOORD1;
};

VS_OUTPUT main( float3 varPosition     : POSITION,
                float2 varTextureCoord : TEXCOORD )
{
    VS_OUTPUT Output;
    Output.UV_stage0 = mul( matTex_stage0, float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.UV_stage1 = mul( matTex_stage1, float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.Position = mul( matWVP, float4(varPosition,1.0) );
    return Output;
}