float4x4 matWVP             : register(c0);
float4x4 matTex             : register(c4);
struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float2 UV         : TEXCOORD0;
};

VS_OUTPUT main( float3 varPosition     : POSITION,
                float2 varTextureCoord : TEXCOORD )
{
    VS_OUTPUT Output;
    Output.UV       = mul( matTex, float4(varTextureCoord, 1.0, 0.0) ).xy;    
    Output.Position = mul( matWVP, float4(varPosition,1.0) );
    return Output;
}
