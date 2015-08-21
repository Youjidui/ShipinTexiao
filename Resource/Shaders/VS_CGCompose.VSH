float4x4 matWVP             : register(c0);
float4x4 matTex[4]          : register(c10);
float4x4   matMesh[4]         : register(c30); // (x,y) - target offset, (z,w) - target scale
float4   bCGBlended         : register(c50);
struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float2 UV         : TEXCOORD0;
    int    index      : TEXCOORD1;
    bool   bCGBlend   : TEXCOORD2;
};

VS_OUTPUT main( float3 varPosition     : POSITION,
                float2 varTextureCoord : TEXCOORD )
{
    VS_OUTPUT Output;
    int index = (int)varPosition.z;
    // tex scale & translation
   
    // target scale & translation
//    float4 tempPos = float4( varPosition.xy, 0.0, 1.0 );
//    tempPos.x = (tempPos.x+0.5) * matMesh[index].z + matMesh[index].x-0.5;
//    tempPos.y = (tempPos.y-0.5) * matMesh[index].w - matMesh[index].y+0.5;
//
//    Output.Position = mul( matWVP, tempPos );   // transform to viewport space   
	Output.Position = mul(matMesh[index],float4( varPosition.xy, 0.0, 1.0 ));
    Output.UV       = mul( matTex[index], float4(varTextureCoord, 1.0, 0.0) ).xy;
    Output.index    = index;
    Output.bCGBlend = bCGBlended[index];
    return Output;
}
