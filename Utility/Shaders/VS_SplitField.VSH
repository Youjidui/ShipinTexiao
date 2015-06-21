float4x4 matWVP             : register(c0);
float4x4 matTex             : register(c4);
float4   matMesh            : register(c8); // (x,y) - target offset, (z,w) - target scale

struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float2 UV         : TEXCOORD0;    
};

VS_OUTPUT main( float3 varPosition     : POSITION,
                float2 varTextureCoord : TEXCOORD )
{
    VS_OUTPUT Output;
    
    // target scale & translation
    float4 tempPos = float4( varPosition.xy, 0.0, 1.0 );
    
    tempPos.x = (tempPos.x+0.5) * matMesh.z + matMesh.x-0.5;
    tempPos.y = (tempPos.y-0.5) * matMesh.w - matMesh.y+0.5;
    
    Output.Position = mul( matWVP, tempPos );   // transform to viewport space    
    Output.UV       = mul( matTex, float4(varTextureCoord, 1.0, 0.0) ).xy;
    
    return Output;
}