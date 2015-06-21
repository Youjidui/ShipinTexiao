float4x4 matWVP             : register(c0);

struct VS_OUTPUT
{
    float4 Position   : POSITION;      // vertex position
    float4 color	    : COLOR;
};

VS_OUTPUT main( float3 varPosition     : POSITION,
                float4 color : COLOR )
{
    VS_OUTPUT Output;
   
    Output.Position = mul( matWVP, float4(varPosition,1.0) );
	Output.color = color;    
    
    return Output;
}