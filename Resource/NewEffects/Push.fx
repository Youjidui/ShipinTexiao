float4x4 g_matWorldViewProj;
float4x4 g_matTexture;

texture g_txColor;
float   g_fAlpha;
float   fSoftness;
float4  vColor;
float	g_fAspect;

sampler g_samColor=
sampler_state
{	
	Texture=<g_txColor>;
	MinFilter=Linear;
	MagFilter=Linear;
	MipFilter=Linear;
	AddressU=CLAMP;
	AddressV=Clamp;
};
	
void VS(float3 vPos:POSITION,
			float2 vTex:TEXCOORD0,
			out float4 oPos:POSITION,
			out float2 oTex0:TEXCOORD0,
			out float2 oTex1:TEXCOORD1)
{
	oPos=mul(float4(vPos,1.0f),g_matWorldViewProj);
	oTex0=mul(float4(vTex,1.0f,0.0f),g_matTexture).xy;
	oTex1=vTex;
}

float4 PS0(float2 Tex0:TEXCOORD0,float2 Tex1:TEXCOORD1):COLOR0
{
	// 通过角度判读其颜色变化
	float2 newpos = Tex1 - float2(0.5, 0.5);
	float angle = atan2(newpos.y, newpos.x);
	float PIDiv4 = 3.1415926f/4.0f;	
	float fPI3Div = 3.0f*PIDiv4;
	float RefValue = 0.0f;
	float alpha = 1.0f;
	if( ( angle >= PIDiv4 && angle<= fPI3Div )
	    ||( angle <= -PIDiv4&& angle>= -fPI3Div))
	{
		RefValue = newpos.y * 2.0f;
	}
	else
	{
		RefValue = newpos.x * 2.0f;
	}  
	if(RefValue < 0.0f) 
		RefValue = -RefValue; 
	RefValue = 1.0f - RefValue;
	
	float4 color = tex2D(g_samColor, Tex0);	
	color.gb -= 0.5f;
	color.rgb *= color.a;
	color.gb += 0.5f;
	float4 result = color;
	// 内软边
	if(fSoftness > 0.0f)
	{
		alpha = (1 - smoothstep(0, fSoftness, RefValue) ) * g_fAlpha;
		result =vColor*alpha + color*(1-alpha);
	}	
	result.gb -= 0.5f;
	result.rgb /= result.a;
	result.gb += 0.5f;
	return result;
}

float GenerateSoft(float2 Tex, float fSoftX, float fSoftY)
{
	float fComp = 0.f;
	if(Tex.x < fSoftX)
 		fComp = 1.f - Tex.x / fSoftX;
 	if(Tex.x > (1.f - fSoftX))
 		fComp = (Tex.x - 1.f + fSoftX) / fSoftX;
 	if(Tex.y < fSoftY)
 	{
 		float fTemp = 1.f - Tex.y / fSoftY;
 		if(Tex.x > fSoftX && Tex.x < (1.f - fSoftX))
 			fComp = fTemp;
 		if(Tex.x < fSoftX)
 			fComp = max(fComp , fTemp);
 		if(Tex.x > (1.f - fSoftX))
 			fComp = max(fComp , fTemp);
 	}
 	if(Tex.y > (1.f - fSoftY))
 	{
 		float fTemp = (Tex.y - 1.f + fSoftY) / fSoftY;
 		if(Tex.x > fSoftX && Tex.x < (1.f - fSoftX))
 			fComp = fTemp;
 		if(Tex.x < fSoftX)
 			fComp = max(fComp , fTemp);
 		if(Tex.x > (1.f - fSoftX))
 			fComp = max(fComp , fTemp);
 	}
	
	return fComp;
}

float4 PS(float2 Tex0:TEXCOORD0,float2 Tex1:TEXCOORD1):COLOR0
{
	// 通过角度判读其颜色变化
	float2 newpos = Tex1 + float2(0.5, 0.5);
	float fSoftY = fSoftness/2.f;
	float fSoftX = fSoftY  * g_fAspect;
	float4 SrcColor = tex2D(g_samColor, Tex0);
	float4 PureColor = float4(0.f,0.f,0.f,0.f);
	if(Tex1.x < fSoftX || Tex1.x > (1.f - fSoftX))
		PureColor = vColor;
	if(Tex1.y < fSoftY || Tex1.y > (1.f - fSoftY))
		PureColor = vColor;
	
	float fAlpha = GenerateSoft( Tex1, fSoftX, fSoftY);
	float4 resultColor = (1.f - fAlpha) * SrcColor + fAlpha * PureColor;
	
	return resultColor;
}

technique Picture
{
	PASS P0
	{
		VertexShader=compile vs_2_0 VS();
		PixelShader=compile ps_2_0 PS();
		CullMode=None;
	}
	
}
