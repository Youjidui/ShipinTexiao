float2 textureSize:register(c0);
struct VS_OUTPUT
{
   float4 pos        : POSITION0;
   float2 texCoord0  : TEXCOORD0;
   float2 texCoord1  : TEXCOORD1;   
};

VS_OUTPUT main( float4 inPos: POSITION,float2 texCoord0: TEXCOORD0 )
{
   VS_OUTPUT o = (VS_OUTPUT) 0;
   inPos.xy    = sign( inPos.xy);
   o.pos       = float4( inPos.xy, 0.0f, 1.0f);
   // get into range [0,1]
   o.texCoord1 = o.texCoord0 = texCoord0;
   if(o.texCoord0.x == 1.0 && o.texCoord0.y==0.0)
   {
     o.texCoord1 = float2( textureSize.x, 0.0 );
   }
   else if(o.texCoord0.x == 1.0 && o.texCoord0.y== 1.0)
   {
     o.texCoord1 = float2( textureSize.x,textureSize.y );
   }
   else if(o.texCoord0.x == 0.0 && o.texCoord0.y== 1.0)
   {
     o.texCoord1 = float2( 0.0, textureSize.y );
   }   
   return o;
}