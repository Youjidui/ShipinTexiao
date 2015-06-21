float4x4 matWVP             : register(c0);
float4x4 matTex[8]          : register(c10);
float4x4 matMesh[8]         : register(c50); // (x,y) - target offset, (z,w) - target scale

struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float2 UV0        : TEXCOORD0;
    float2 UV1        : TEXCOORD1;
    float2 UV2        : TEXCOORD2;
    float2 UV3        : TEXCOORD3;
    float2 UV4        : TEXCOORD4;
    float2 UV5        : TEXCOORD5;
    float2 UV6        : TEXCOORD6;
    float2 UV7        : TEXCOORD7;
};

struct VS_INPUT
{
	float3 varPosition     : POSITION;
    float2 varTextureCoord0 : TEXCOORD0;
    float2 varTextureCoord1 : TEXCOORD1;
    float2 varTextureCoord2 : TEXCOORD2;
    float2 varTextureCoord3 : TEXCOORD3;
    float2 varTextureCoord4 : TEXCOORD4;
    float2 varTextureCoord5 : TEXCOORD5;
    float2 varTextureCoord6 : TEXCOORD6;
    float2 varTextureCoord7 : TEXCOORD7;
};

// 8¸öUV×ø±ê
VS_OUTPUT main( VS_INPUT input )
{
    VS_OUTPUT Output;

	Output.UV0 = mul( matTex[0], float4(input.varTextureCoord0, 1.0, 0.0) ).xy;
	Output.UV0 = mul( matMesh[0], float4(Output.UV0, 1.0f, 0.0f) ).xy;
	Output.UV1 = mul( matTex[1], float4(input.varTextureCoord1, 1.0, 0.0) ).xy;
	Output.UV1 = mul( matMesh[1], float4(Output.UV1, 1.0f, 0.0f)).xy;
	Output.UV2 = mul( matTex[2], float4(input.varTextureCoord2, 1.0, 0.0) ).xy;
	Output.UV2 = mul( matMesh[2], float4(Output.UV2, 1.0f, 0.0f)).xy;
	Output.UV3 = mul( matTex[3], float4(input.varTextureCoord3, 1.0, 0.0) ).xy;
	Output.UV3 = mul( matMesh[3], float4(Output.UV3, 1.0f, 0.0f)).xy;
	Output.UV4 = mul( matTex[4], float4(input.varTextureCoord4, 1.0, 0.0) ).xy;
	Output.UV4 = mul( matMesh[4], float4(Output.UV4, 1.0f, 0.0f)).xy;
	Output.UV5 = mul( matTex[5], float4(input.varTextureCoord5, 1.0, 0.0) ).xy;
	Output.UV5 = mul( matMesh[5], float4(Output.UV5, 1.0f, 0.0f)).xy;
	Output.UV6 = mul( matTex[6], float4(input.varTextureCoord6, 1.0, 0.0) ).xy;
	Output.UV6 = mul( matMesh[6], float4(Output.UV6, 1.0f, 0.0f)).xy;
	Output.UV7 = mul( matTex[7], float4(input.varTextureCoord7, 1.0, 0.0) ).xy;
	Output.UV7 = mul( matMesh[7], float4(Output.UV7, 1.0f, 0.0f)).xy;

    // target scale & translation
    Output.Position = mul( matWVP, float4( input.varPosition, 1.0 ) );   // transform to viewport space
    return Output;
}