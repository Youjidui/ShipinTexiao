float4x4 matWVP               : register( c0  );
float4   srctexPos_Scale[8]   : register( c4  );//x,y:scale, z,w:pos
float4   dsttexPos_Scale[8]   : register( c12 );

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

VS_OUTPUT main( float3 varPosition     : POSITION,
                float2 varTexCoord : TEXCOORD )
{
    VS_OUTPUT Output;        
    Output.UV0       = varTexCoord * srctexPos_Scale[0].xy /dsttexPos_Scale[0].xy -dsttexPos_Scale[0].zw + srctexPos_Scale[0].zw;
    Output.UV1       = varTexCoord * srctexPos_Scale[1].xy /dsttexPos_Scale[1].xy-dsttexPos_Scale[1].zw + srctexPos_Scale[1].zw;
    Output.UV2       = varTexCoord * srctexPos_Scale[2].xy /dsttexPos_Scale[2].xy-dsttexPos_Scale[2].zw + srctexPos_Scale[2].zw;
    Output.UV3       = varTexCoord * srctexPos_Scale[3].xy /dsttexPos_Scale[3].xy-dsttexPos_Scale[3].zw + srctexPos_Scale[3].zw;
    Output.UV4       = varTexCoord * srctexPos_Scale[4].xy /dsttexPos_Scale[4].xy-dsttexPos_Scale[4].zw + srctexPos_Scale[4].zw;
    Output.UV5       = varTexCoord * srctexPos_Scale[5].xy /dsttexPos_Scale[5].xy-dsttexPos_Scale[5].zw + srctexPos_Scale[5].zw;
    Output.UV6       = varTexCoord * srctexPos_Scale[6].xy /dsttexPos_Scale[6].xy-dsttexPos_Scale[6].zw + srctexPos_Scale[6].zw;
    Output.UV7       = varTexCoord * srctexPos_Scale[7].xy /dsttexPos_Scale[7].xy-dsttexPos_Scale[7].zw + srctexPos_Scale[7].zw;        
    Output.Position  = mul( matWVP, float4(varPosition,1.0) );
    return Output;
}