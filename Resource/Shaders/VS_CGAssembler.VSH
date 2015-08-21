float4x4 matWVP             : register(c0);
float4x4 matTex[8]          : register(c10);
float4   matMesh[8]         : register(c50); // (x,y) - target offset, (z,w) - target scale

struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float2 UV         : TEXCOORD0;
    int    index      : TEXCOORD1;
};

VS_OUTPUT main( float3 varPosition     : POSITION,
                float2 varTextureCoord : TEXCOORD )
{
    VS_OUTPUT Output;
    int index = (int)varPosition.z;

    // tex scale & translation
    Output.UV = mul( matTex[index], float4(varTextureCoord, 1.0, 0.0) ).xy;

    // target scale & translation
    float4 tempPos = float4( varPosition.xy, 0.0, 1.0 );
    tempPos.x = (tempPos.x+0.5) * matMesh[index].z + matMesh[index].x-0.5;
    tempPos.y = (tempPos.y-0.5) * matMesh[index].w - matMesh[index].y+0.5;

    Output.Position = mul( matWVP, tempPos );   // transform to viewport space
    Output.index = index;
    return Output;
}


