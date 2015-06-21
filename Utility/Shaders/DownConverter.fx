float4x4	g_matWVP;
// 使用了两张 1920 * 540 的 16 位纹理做为缓冲，从而将源直接转入到 1920 * 540 的区域中。
// 在做 YUYV 到 YUVA 的变换时，要求全幅面，以避免映射错误。
// 在做完 Anti-Alias 后，将源的操作区域平移到 1920 * 540 缓存的左上角。
// 由于做 Anti-Alias 时，没有幅面的变化，可以直接使用纹理矩阵进行映射。
// 但要注意，在进行不同的 PASS 渲染时，需要设置不同的纹理矩阵。
float4x4    g_matTex;

// 进行 PASS 渲染时的源纹理。
texture		g_txSrc;

// 进行缩放时的参数。
// 第二个元素的后半部分在 TopField 时为 0，在 BottomField 时为给定公式。
float4 fSrcRect;		// [ srcLeft, srcTop + (srcH - desH) / (2 * desH), srcW, srcH ] 源操作区域
// 此偏移在分开进行缩放的时候起作用，前两个用在水平缩放时，后两个用在垂直缩放时。
// 一般情况下，在做 Anti-Alias 后，源都会在左上角顶齐，如果不做 Anti-Alias，那么在做完水平缩放后，
// 中间结果也会在左上角顶齐，因此，在大部分的时候 hSrcLeft = vSrcLeft = hSrcTop = vSrcTop = 0。
float4 fSrcOffset;		// [ hSrcLeft, (hSrcTop + 0.5f) / baseSrcH, (vSrcLeft + 0.5f) / baseSrcW, vSrcTop + (srcH - desH) / (2 * desH) ]
// 后两者用于水平与垂直分开渲染的情况。
float4 fBaseCoef;		// [ 1.f / baseSrcW, 1.f / baseSrcH, desW / baseSrcW, srcH / baseSrcH ]
float4 fBaseOffset;		// [ 2.f / baseSrcW, 2.f / baseSrcH, 0.5f / baseSrcW, 0.5f / baseSrcH ]
// 缩放算法中，一般情况下的坐标映射公式：目标纹理坐标(Ud, Vd)，源纹理坐标(Us, Vs)。
// 水平：	fXs = Ud * srcW + srcLeft
//			Us = (floor(fXs) + 0.5) / baseSrcW
//			Cx = floor(frac(fXs) * 16)
// 垂直：由于存在 TopField 与 BottomField 的区别，需要做特殊处理。
// TopField：		fYsT = Vd * srcH + srcTop
// BottomField：	fYsB = Vd * srcH + (srcH - desH) / (2 * desH) + srcTop
//					fYs = Vd * srcH + srcTop + (bTopField) ? 0 : (srcH - desH) / (2 * desH)
//					Vs = (floor(fYs) + 0.5) / baseSrcH
//					Cy = floor(frac(fYs) * 16)
// 如果在做完 Anti-Alias 后，能将源的操作区域平移到缓存的左上角，则 srcLeft = srcTop = 0。

// 由于 Anti-Alias 的系数具有对称性，可以根据这种特性对 PS 进行一定的优化。
#define _SPECIAL_QUICK_PROCESS_
// 抗混叠系数
#define COEF_AA_3_EDGE			0.25f
#define COEF_AA_3_MID			0.5f
#define COEF_AA_3_EDGE_SQUARE	0.0625f
#define COEF_AA_3_EDGE_HALF		0.125f
static const float3 s_fCoefAA = { COEF_AA_3_EDGE, COEF_AA_3_MID, COEF_AA_3_EDGE };
// 4 采样系数
#define COEF_4_V_COUNT		16
static const float4 s_samCoef4[COEF_4_V_COUNT] = {
	{ 0.000000f, 1.000000f, 0.000000f, 0.000000f, },
	{ -0.025879f, 0.984863f, 0.058594f, -0.017578f, },
	{ -0.054688f, 0.960449f, 0.114258f, -0.020020f, },
	{ -0.074707f, 0.922852f, 0.178711f, -0.026855f, },
	{ -0.086914f, 0.873047f, 0.250000f, -0.036133f, },
	{ -0.092285f, 0.811035f, 0.328125f, -0.046875f, },
	{ -0.092285f, 0.740234f, 0.410156f, -0.058105f, },
	{ -0.087402f, 0.662109f, 0.494629f, -0.069336f, },
	{ -0.079590f, 0.579590f, 0.579590f, -0.079590f, },
	{ -0.069336f, 0.494629f, 0.662109f, -0.087402f, },
	{ -0.058105f, 0.410156f, 0.740234f, -0.092285f, },
	{ -0.046875f, 0.328125f, 0.811035f, -0.092285f, },
	{ -0.036133f, 0.250000f, 0.873047f, -0.086914f, },
	{ -0.026855f, 0.178711f, 0.922852f, -0.074707f, },
	{ -0.020020f, 0.114258f, 0.960449f, -0.054688f, },
	{ -0.017578f, 0.058594f, 0.984863f, -0.025879f, },
};

sampler g_samColor =
sampler_state
{	
	Texture = <g_txSrc>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

void VS_AA(float4 PosIn:POSITION,
			float2 TexIn:TEXCOORD0,
			out float4 PosOut:POSITION,
			out float2 TexSrc:TEXCOORD0)
{
	PosOut = mul(PosIn, g_matWVP);
	TexSrc = mul(float3(TexIn, 1.f), g_matTex);
}

float4 PS_AAH(float2 TexSrc:TEXCOORD0):COLOR
{
#ifdef _SPECIAL_QUICK_PROCESS_
	float4 fPrev = tex2D(g_samColor, TexSrc - float2(fBaseCoef.x, 0.f));
	float4 fCurr = tex2D(g_samColor, TexSrc) * COEF_AA_3_MID;
	float4 fNext = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, 0.f));

	return ((fPrev + fNext) * COEF_AA_3_EDGE + fCurr);
#else
	float4 fPrev = tex2D(g_samColor, TexSrc - float2(fBaseCoef.x, 0.f));
	float4 fCurr = tex2D(g_samColor, TexSrc);
	float4 fNext = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, 0.f));

	return (fPrev * s_fCoefAA.x + fCurr * s_fCoefAA.y + fNext * s_fCoefAA.z);
#endif
}

float4 PS_AAV(float2 TexSrc:TEXCOORD0):COLOR
{
#ifdef _SPECIAL_QUICK_PROCESS_
	float4 fPrev = tex2D(g_samColor, TexSrc - float2(0.f, fBaseCoef.y));
	float4 fCurr = tex2D(g_samColor, TexSrc) * COEF_AA_3_MID;
	float4 fNext = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));

	return ((fPrev + fNext) * COEF_AA_3_EDGE + fCurr);
#else
	float4 fPrev = tex2D(g_samColor, TexSrc - float2(0.f, fBaseCoef.y));
	float4 fCurr = tex2D(g_samColor, TexSrc);
	float4 fNext = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	
	return (fPrev * s_fCoefAA.x + fCurr * s_fCoefAA.y + fNext * s_fCoefAA.z);
#endif
}

float4 PS_AAA(float2 TexSrc:TEXCOORD0):COLOR
{
#ifdef _SPECIAL_QUICK_PROCESS_
	float4 f00 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, -fBaseCoef.y));
	float4 f02 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, -fBaseCoef.y));
	float4 f20 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, fBaseCoef.y));
	float4 f22 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, fBaseCoef.y));
	float4 f11 = tex2D(g_samColor, TexSrc) * COEF_AA_3_EDGE;
	float4 fTmp = (f00 + f02 + f20 + f22) * COEF_AA_3_EDGE_SQUARE + f11;
	float4 f01 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 f10 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, 0.f));
	float4 f12 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, 0.f));
	float4 f21 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	
	return ((f01 + f10 + f12 + f21) * COEF_AA_3_EDGE_HALF + fTmp);
#else
	// 前一行
	float4 fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, -fBaseCoef.y));
	float4 fCol1 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, -fBaseCoef.y));
	float4 fLine0 = (fCol0 * s_fCoefAA.x + fCol1 * s_fCoefAA.y + fCol2 * s_fCoefAA.z);
	// 当前行
	fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, 0.f));
	fCol1 = tex2D(g_samColor, TexSrc);
	fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, 0.f));
	float4 fLine1 = (fCol0 * s_fCoefAA.x + fCol1 * s_fCoefAA.y + fCol2 * s_fCoefAA.z);
	// 下一行
	fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, fBaseCoef.y));
	fCol1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, fBaseCoef.y));
	float4 fLine2 = (fCol0 * s_fCoefAA.x + fCol1 * s_fCoefAA.y + fCol2 * s_fCoefAA.z);
	
	return (fLine0 * s_fCoefAA.x + fLine1 * s_fCoefAA.y + fLine2 * s_fCoefAA.z);
#endif
}

void VS_I4A(float4 PosIn:POSITION,
			float2 TexIn:TEXCOORD0,
			out float4 PosOut:POSITION,
			out float2 TexDes:TEXCOORD0)
{
	PosOut = mul(PosIn, g_matWVP);
	TexDes = TexIn;
}

float4 PS_I4A(float2 TexDes:TEXCOORD0):COLOR
{
	// 求出源纹理中的真实坐标与系数坐标
	float2 fRealSrc = TexDes * fSrcRect.zw + fSrcRect.xy;		// 原图大小的浮点映射坐标
	float2 TexSrc = floor(fRealSrc) * fBaseCoef.xy + fBaseOffset.zw;
	int2 nCoefIndex = floor(frac(fRealSrc) * COEF_4_V_COUNT);
	// 取系数
	float4 fCoefH = s_samCoef4[nCoefIndex.x];
	float4 fCoefV = s_samCoef4[nCoefIndex.y];
	// 水平方向
	// 前一行
	float4 fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, -fBaseCoef.y));
	float4 fCol1 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, -fBaseCoef.y));
	float4 fCol3 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.x, -fBaseCoef.y));
	float4 fLine0 = (fCol0 * fCoefH.x + fCol1 * fCoefH.y + fCol2 * fCoefH.z + fCol3 * fCoefH.w);
	// 当前行
	fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, 0.f));
	fCol1 = tex2D(g_samColor, TexSrc);
	fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, 0.f));
	fCol3 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.x, 0.f));
	float4 fLine1 = (fCol0 * fCoefH.x + fCol1 * fCoefH.y + fCol2 * fCoefH.z + fCol3 * fCoefH.w);
	// 下一行
	fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, fBaseCoef.y));
	fCol1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, fBaseCoef.y));
	fCol3 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.x, fBaseCoef.y));
	float4 fLine2 = (fCol0 * fCoefH.x + fCol1 * fCoefH.y + fCol2 * fCoefH.z + fCol3 * fCoefH.w);
	// 下下一行
	fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, fBaseOffset.y));
	fCol1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseOffset.y));
	fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, fBaseOffset.y));
	fCol3 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.y, fBaseOffset.y));
	float4 fLine3 = (fCol0 * fCoefH.x + fCol1 * fCoefH.y + fCol2 * fCoefH.z + fCol3 * fCoefH.w);
	// 垂直方向
	return (fLine0 * fCoefV.x + fLine1 * fCoefV.y + fLine2 * fCoefV.z + fLine3 * fCoefV.w);
}

float4 PS_I4H(float2 TexDes:TEXCOORD0):COLOR
{
	// 原图大小的浮点映射坐标
	float fRealSrcX = TexDes.x * fSrcRect.z + fSrcOffset.x;
	float2 TexSrc = float2(floor(fRealSrcX), TexDes.y) * float2(fBaseCoef.x, fBaseCoef.w) + float2(fBaseOffset.z, fSrcOffset.y);
	// 取系数
	float4 fCoefH = s_samCoef4[floor(frac(fRealSrcX) * COEF_4_V_COUNT)];
	// 当前行
	float4 fCol0 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x, 0.f));
	float4 fCol1 = tex2D(g_samColor, TexSrc);
	float4 fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, 0.f));
	float4 fCol3 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.x, 0.f));
	return (fCol0 * fCoefH.x + fCol1 * fCoefH.y + fCol2 * fCoefH.z + fCol3 * fCoefH.w);
}

float4 PS_I4V(float2 TexDes:TEXCOORD0):COLOR
{
	// 原图大小的浮点映射坐标
	float fRealSrcY = TexDes.y * fSrcRect.w + fSrcOffset.w;
	float2 TexSrc = float2(TexDes.x, floor(fRealSrcY)) * float2(fBaseCoef.z, fBaseCoef.y) + float2(fSrcOffset.z, fBaseOffset.w);
	// 取系数
	float4 fCoefV = s_samCoef4[floor(frac(fRealSrcY) * COEF_4_V_COUNT)];
	// 当前列
	float4 fLine0 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 fLine1 = tex2D(g_samColor, TexSrc);
	float4 fLine2 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	float4 fLine3 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseOffset.y));
	return (fLine0 * fCoefV.x + fLine1 * fCoefV.y + fLine2 * fCoefV.z + fLine3 * fCoefV.w);
}

technique DownConverter
{
	pass AntiAliasH
	{
		VertexShader = compile vs_3_0	VS_AA();
		PixelShader = compile ps_3_0	PS_AAH();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	pass AntiAliasV
	{
		VertexShader = compile vs_3_0	VS_AA();
		PixelShader = compile ps_3_0	PS_AAV();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	pass AntiAliasA
	{
		VertexShader = compile vs_3_0	VS_AA();
		PixelShader = compile ps_3_0	PS_AAA();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	pass Interpolate4H
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_I4H();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	pass Interpolate4V
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_I4V();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	pass Interpolate4A
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_I4A();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
}

/********************************************/

// 8 采样系数（需要同时采样两个向量，坐标乘 2）
#define COEF_8_V_COUNT		32
static const float4 s_samCoef8[COEF_8_V_COUNT] = {
	{0.000000f, 0.000000f, 0.000000f, 1.000000f}, {0.000000f, 0.000000f, 0.000000f, 0.000000f}, 
	{-0.004883f, 0.017090f, -0.050293f, 0.996094f}, {0.059570f, -0.020020f, 0.006348f, -0.003906f}, 
	{-0.009277f, 0.031250f, -0.091797f, 0.975586f}, {0.126953f, -0.041504f, 0.013184f, -0.004395f}, 
	{-0.012207f, 0.042480f, -0.124512f, 0.941406f}, {0.201172f, -0.063965f, 0.020996f, -0.005371f}, 
	{-0.014160f, 0.050293f, -0.147461f, 0.895508f}, {0.280762f, -0.086426f, 0.028320f, -0.006836f}, 
	{-0.015625f, 0.055664f, -0.161621f, 0.838867f}, {0.363770f, -0.108398f, 0.036133f, -0.008789f}, 
	{-0.015625f, 0.057617f, -0.167969f, 0.772461f}, {0.449219f, -0.128418f, 0.043457f, -0.010742f}, 
	{-0.015137f, 0.057129f, -0.166992f, 0.698730f}, {0.535156f, -0.145996f, 0.049316f, -0.012207f}, 
	{-0.013672f, 0.054199f, -0.159180f, 0.618652f}, {0.618652f, -0.159180f, 0.054199f, -0.013672f}, 
	{-0.012207f, 0.049316f, -0.145996f, 0.535156f}, {0.698730f, -0.166992f, 0.057129f, -0.015137f}, 
	{-0.010742f, 0.043457f, -0.128418f, 0.449219f}, {0.772461f, -0.167969f, 0.057617f, -0.015625f}, 
	{-0.008789f, 0.036133f, -0.108398f, 0.363770f}, {0.838867f, -0.161621f, 0.055664f, -0.015625f}, 
	{-0.006836f, 0.028320f, -0.086426f, 0.280762f}, {0.895508f, -0.147461f, 0.050293f, -0.014160f}, 
	{-0.005371f, 0.020996f, -0.063965f, 0.201172f}, {0.941406f, -0.124512f, 0.042480f, -0.012207f}, 
	{-0.004395f, 0.013184f, -0.041504f, 0.126953f}, {0.975586f, -0.091797f, 0.031250f, -0.009277f}, 
	{-0.003906f, 0.006348f, -0.020020f, 0.059570f}, {0.996094f, -0.050293f, 0.017090f, -0.004883f}
};
#define COEF_8_SPACE		0.03125f

texture	tableCoef8;

sampler samCoef8 = 
sampler_state
{
	Texture = <tableCoef8>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU = Clamp;
	AddressV = Clamp;
};

// (1,2,3,4,3,2,1) / 16

float4 PS_AA8H(float2 TexSrc:TEXCOORD0):COLOR
{
	float4 fPrev3 = tex2D(g_samColor, TexSrc - float2(fBaseCoef.x*3, 0.f));
	float4 fPrev2 = tex2D(g_samColor, TexSrc - float2(fBaseCoef.x*2, 0.f));
	float4 fPrev = tex2D(g_samColor, TexSrc - float2(fBaseCoef.x, 0.f));
	float4 fCurr = tex2D(g_samColor, TexSrc);
	float4 fNext = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x, 0.f));
	float4 fNext2 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x*2, 0.f));
	float4 fNext3 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x*3, 0.f));

	return ( (fPrev3+fNext3) + 2*(fPrev2+fNext2) + 3*(fPrev+fNext) + 4*fCurr ) / 16;
}

float4 PS_AA8V(float2 TexSrc:TEXCOORD0):COLOR
{
	float4 fPrev3 = tex2D(g_samColor, TexSrc - float2(0.f, fBaseCoef.y*3));
	float4 fPrev2 = tex2D(g_samColor, TexSrc - float2(0.f, fBaseCoef.y*2));
	float4 fPrev = tex2D(g_samColor, TexSrc - float2(0.f, fBaseCoef.y));
	float4 fCurr = tex2D(g_samColor, TexSrc);
	float4 fNext = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	float4 fNext2 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*2));
	float4 fNext3 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*3));

	return ( (fPrev3+fNext3) + 2*(fPrev2+fNext2) + 3*(fPrev+fNext) + 4*fCurr ) / 16;
}

float4 PS_I8V( float2 TexDes : TEXCOORD0 ) : COLOR
{
	// 原图大小的浮点映射坐标
	float fRealSrcY = TexDes.y * fSrcRect.w + fSrcOffset.w;
	float2 TexSrc = float2(TexDes.x, floor(fRealSrcY)) * float2(fBaseCoef.z, fBaseCoef.y) + float2(fSrcOffset.z, fBaseOffset.w);

	float4 fLine_3 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y*3));
	float4 fLine_2 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseOffset.y));
	float4 fLine_1 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 fLine0 = tex2D(g_samColor, TexSrc);
	float4 fLine1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	float4 fLine2 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseOffset.y));
	float4 fLine3 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*3));
	float4 fLine4 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*4));

	// 取系数
	float index = (floor(frac(fRealSrcY) * COEF_4_V_COUNT) + 0.5f) / 48;
	
	return (fLine_3 * tex2D(samCoef8, float2(0.5f/8,index)).r + 
			fLine_2 * tex2D(samCoef8, float2(1.5f/8,index)).r + 
			fLine_1 * tex2D(samCoef8, float2(2.5f/8,index)).r + 
			fLine0 * tex2D(samCoef8, float2(3.5f/8,index)).r)
			+
			(fLine1 * tex2D(samCoef8, float2(4.5f/8,index)).r + 
			fLine2 * tex2D(samCoef8, float2(5.5f/8,index)).r + 
			fLine3 * tex2D(samCoef8, float2(6.5f/8,index)).r + 
			fLine4 * tex2D(samCoef8, float2(7.5f/8,index)).r);
}

float4 PS_I8H( float2 TexDes : TEXCOORD0 ) : COLOR
{
	// 原图大小的浮点映射坐标
	float fRealSrcX = TexDes.x * fSrcRect.z + fSrcOffset.x;
	float2 TexSrc = float2(floor(fRealSrcX), TexDes.y) * float2(fBaseCoef.x, fBaseCoef.w) + float2(fBaseOffset.z, fSrcOffset.y);

	// 当前行
	float4 fCol_3 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x*3,0.f));
	float4 fCol_2 = tex2D(g_samColor, TexSrc + float2(-fBaseOffset.x,0.f));
	float4 fCol_1 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x,0.f));
	float4 fCol0 = tex2D(g_samColor, TexSrc);
	float4 fCol1 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x,0.f));
	float4 fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.x,0.f));
	float4 fCol3 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x*3,0.f));
	float4 fCol4 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x*4,0.f));

	float index = (floor(frac(fRealSrcX) * COEF_4_V_COUNT) + 0.5f) / 48;

	return (fCol_3 * tex2D(samCoef8, float2(0.5f/8,index)).r + 
			fCol_2 * tex2D(samCoef8, float2(1.5f/8,index)).r + 
			fCol_1 * tex2D(samCoef8, float2(2.5f/8,index)).r + 
			fCol0 * tex2D(samCoef8, float2(3.5f/8,index)).r)
			+
			(fCol1 * tex2D(samCoef8, float2(4.5f/8,index)).r + 
			fCol2 * tex2D(samCoef8, float2(5.5f/8,index)).r + 
			fCol3 * tex2D(samCoef8, float2(6.5f/8,index)).r + 
			fCol4 * tex2D(samCoef8, float2(7.5f/8,index)).r);
}

/*float4 PS_I8V( float2 TexDes : TEXCOORD0 ) : COLOR
{
	// 原图大小的浮点映射坐标
	float fRealSrcY = TexDes.y * fSrcRect.w + fSrcOffset.w;
	float2 TexSrc = float2(TexDes.x, floor(fRealSrcY)) * float2(fBaseCoef.z, fBaseCoef.y) + float2(fSrcOffset.z, fBaseOffset.w);
	// 取系数
	int index0 = floor(frac(fRealSrcY) * COEF_4_V_COUNT) * 2;
	int index1 = index0 + 1;	

	float4 fLine_3 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y*3));
	float4 fLine_2 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseOffset.y));
	float4 fLine_1 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 fLine0 = tex2D(g_samColor, TexSrc);
	float4 fLine1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	float4 fLine2 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseOffset.y));
	float4 fLine3 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*3));
	float4 fLine4 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*4));

	return (fLine_3 * s_samCoef8[index0].x + 
			fLine_2 * s_samCoef8[index0].y + 
			fLine_1 * s_samCoef8[index0].z + 
			fLine0 * s_samCoef8[index0].w)
			+
			(fLine1 * s_samCoef8[index1].x + 
			fLine2 * s_samCoef8[index1].y + 
			fLine3 * s_samCoef8[index1].z + 
			fLine4 * s_samCoef8[index1].w);
}

float4 PS_I8H( float2 TexDes : TEXCOORD0 ) : COLOR
{
	// 原图大小的浮点映射坐标
	float fRealSrcX = TexDes.x * fSrcRect.z + fSrcOffset.x;
	float2 TexSrc = float2(floor(fRealSrcX), TexDes.y) * float2(fBaseCoef.x, fBaseCoef.w) + float2(fBaseOffset.z, fSrcOffset.y);
	// 取系数
	int index0 = floor(frac(fRealSrcX) * COEF_4_V_COUNT) * 2;
	int index1 = index0 + 1;	

	// 当前行
	float4 fCol_3 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.x*3));
	float4 fCol_2 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseOffset.x));
	float4 fCol_1 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.x));
	float4 fCol0 = tex2D(g_samColor, TexSrc);
	float4 fCol1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.x));
	float4 fCol2 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseOffset.x));
	float4 fCol3 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.x*3));
	float4 fCol4 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.x*4));

	return (fCol_3 * s_samCoef8[index0].x + 
			fCol_2 * s_samCoef8[index0].y + 
			fCol_1 * s_samCoef8[index0].z + 
			fCol0 * s_samCoef8[index0].w)
			+
			(fCol1 * s_samCoef8[index1].x + 
			fCol2 * s_samCoef8[index1].y + 
			fCol3 * s_samCoef8[index1].z + 
			fCol4 * s_samCoef8[index1].w);
}*/

technique DownConverter_adv
{
	pass AntiAlias8H
	{
		VertexShader = compile vs_3_0	VS_AA();
		PixelShader = compile ps_3_0	PS_AA8H();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass AntiAlias8V
	{
		VertexShader = compile vs_3_0	VS_AA();
		PixelShader = compile ps_3_0	PS_AA8V();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass AnitAlias8HV
	{
	
	}
	
	pass Interpolate8H
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_I8H();
		
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Interpolate8V
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_I8V();
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Interpolate8HV
	{
	
	}
}

// method2

int4	tableOffsetH;	//(x,y)(sizeXY)
int4	tableOffsetV;	//(x,y)(sizeXY)

float4 PS_DownUp4H( float2 TexDes : TEXCOORD0 ) : COLOR
{
	int4 tableOffset = tableOffsetH;

	// 原图大小的浮点映射坐标
	float fRealSrcX = TexDes.x * fSrcRect.z + fSrcOffset.x;
	float2 TexSrc = float2(floor(fRealSrcX), TexDes.y) * float2(fBaseCoef.x, fBaseCoef.w) + float2(fBaseOffset.z, fSrcOffset.y);

	// 当前行
	float4 fCol_1 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x,0.f));
	float4 fCol0 = tex2D(g_samColor, TexSrc);
	float4 fCol1 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x,0.f));
	float4 fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.x,0.f));

	float index = (floor(frac(fRealSrcX) * COEF_4_V_COUNT) + tableOffset.y + 0.5f) / tableOffset.w;

	return (fCol_1 * tex2D(samCoef8, float2((tableOffset.x+0.5f)/tableOffset.z,index)).r + 
			fCol0 * tex2D(samCoef8, float2((tableOffset.x+1.5f)/tableOffset.z,index)).r)
			+
			(fCol1 * tex2D(samCoef8, float2((tableOffset.x+2.5f)/tableOffset.z,index)).r + 
			fCol2 * tex2D(samCoef8, float2((tableOffset.x+3.5f)/tableOffset.z,index)).r);
}

float4 PS_DownUp4V( float2 TexDes : TEXCOORD0 ) : COLOR
{
	int4 tableOffset = tableOffsetV;


	// 原图大小的浮点映射坐标
	float fRealSrcY = TexDes.y * fSrcRect.w + fSrcOffset.w;
	float2 TexSrc = float2(TexDes.x, floor(fRealSrcY)) * float2(fBaseCoef.z, fBaseCoef.y) + float2(fSrcOffset.z, fBaseOffset.w);

	float4 fLine_1 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 fLine0 = tex2D(g_samColor, TexSrc);
	float4 fLine1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	float4 fLine2 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseOffset.y));

	// 取系数
	float index = (floor(frac(fRealSrcY) * COEF_4_V_COUNT) + tableOffset.y + 0.5f) / tableOffset.w;
	
	return (fLine_1 * tex2D(samCoef8, float2((tableOffset.x+0.5f)/tableOffset.z,index)).r + 
			fLine0 * tex2D(samCoef8, float2((tableOffset.x+1.5f)/tableOffset.z,index)).r)
			+
			(fLine1 * tex2D(samCoef8, float2((tableOffset.x+2.5f)/tableOffset.z,index)).r + 
			fLine2 * tex2D(samCoef8, float2((tableOffset.x+3.5f)/tableOffset.z,index)).r);
}

float4 PS_DownUp8V( float2 TexDes : TEXCOORD0, uniform bool bYCbCr ) : COLOR
{
	int4 tableOffset = tableOffsetV;


	// 原图大小的浮点映射坐标
	float fRealSrcY = TexDes.y * fSrcRect.w + fSrcOffset.w;
	float2 TexSrc = float2(TexDes.x, floor(fRealSrcY)) * float2(fBaseCoef.z, fBaseCoef.y) + float2(fSrcOffset.z, fBaseOffset.w);

	float4 fLine_3 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y*3));
	float4 fLine_2 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseOffset.y));
	float4 fLine_1 = tex2D(g_samColor, TexSrc + float2(0.f, -fBaseCoef.y));
	float4 fLine0 = tex2D(g_samColor, TexSrc);
	float4 fLine1 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y));
	float4 fLine2 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseOffset.y));
	float4 fLine3 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*3));
	float4 fLine4 = tex2D(g_samColor, TexSrc + float2(0.f, fBaseCoef.y*4));

	// 取系数
	float index = (floor(frac(fRealSrcY) * COEF_4_V_COUNT) + tableOffset.y + 0.5f) / tableOffset.w;
	
	if ( bYCbCr )
	{
		return (fLine_3 * tex2D(samCoef8, float2((tableOffset.x+0.5f)/tableOffset.z,index)).rggr + 
				fLine_2 * tex2D(samCoef8, float2((tableOffset.x+1.5f)/tableOffset.z,index)).rggr + 
				fLine_1 * tex2D(samCoef8, float2((tableOffset.x+2.5f)/tableOffset.z,index)).rggr + 
				fLine0 * tex2D(samCoef8, float2((tableOffset.x+3.5f)/tableOffset.z,index)).rggr)
				+
				(fLine1 * tex2D(samCoef8, float2((tableOffset.x+4.5f)/tableOffset.z,index)).rggr + 
				fLine2 * tex2D(samCoef8, float2((tableOffset.x+5.5f)/tableOffset.z,index)).rggr + 
				fLine3 * tex2D(samCoef8, float2((tableOffset.x+6.5f)/tableOffset.z,index)).rggr + 
				fLine4 * tex2D(samCoef8, float2((tableOffset.x+7.5f)/tableOffset.z,index)).rggr);
	
	}
	else
	{
		return (fLine_3 * tex2D(samCoef8, float2((tableOffset.x+0.5f)/tableOffset.z,index)).r + 
				fLine_2 * tex2D(samCoef8, float2((tableOffset.x+1.5f)/tableOffset.z,index)).r + 
				fLine_1 * tex2D(samCoef8, float2((tableOffset.x+2.5f)/tableOffset.z,index)).r + 
				fLine0 * tex2D(samCoef8, float2((tableOffset.x+3.5f)/tableOffset.z,index)).r)
				+
				(fLine1 * tex2D(samCoef8, float2((tableOffset.x+4.5f)/tableOffset.z,index)).r + 
				fLine2 * tex2D(samCoef8, float2((tableOffset.x+5.5f)/tableOffset.z,index)).r + 
				fLine3 * tex2D(samCoef8, float2((tableOffset.x+6.5f)/tableOffset.z,index)).r + 
				fLine4 * tex2D(samCoef8, float2((tableOffset.x+7.5f)/tableOffset.z,index)).r);
	}
}

float4 PS_DownUp8H( float2 TexDes : TEXCOORD0, uniform bool bYCbCr ) : COLOR
{
	//TexDes += float2( 0.000001f, 0.000001f );

	int4 tableOffset = tableOffsetH;

	// 原图大小的浮点映射坐标
	float fRealSrcX = TexDes.x * fSrcRect.z + fSrcOffset.x;
	float2 TexSrc = float2(floor(fRealSrcX), TexDes.y) * float2(fBaseCoef.x, fBaseCoef.w) + float2(fBaseOffset.z, fSrcOffset.y);

	// 当前行
	float4 fCol_3 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x*3,0.f));
	float4 fCol_2 = tex2D(g_samColor, TexSrc + float2(-fBaseOffset.x,0.f));
	float4 fCol_1 = tex2D(g_samColor, TexSrc + float2(-fBaseCoef.x,0.f));
	float4 fCol0 = tex2D(g_samColor, TexSrc);
	float4 fCol1 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x,0.f));
	float4 fCol2 = tex2D(g_samColor, TexSrc + float2(fBaseOffset.x,0.f));
	float4 fCol3 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x*3,0.f));
	float4 fCol4 = tex2D(g_samColor, TexSrc + float2(fBaseCoef.x*4,0.f));

	float index = (floor( (frac(fRealSrcX)) * COEF_4_V_COUNT) + tableOffset.y + 0.5f) / tableOffset.w;
	/*return	float4(	
	floor( (frac(fRealSrcX)+0.000001f) * COEF_4_V_COUNT) * 16 / 256.f,
	(float)((int)(floor(TexDes.x * 1920)) % 256) / 255.f,
	(float)((int)(ceil(TexDes.x * 1920)) % 256) / 255.f, 
	0.f);*/

	if ( bYCbCr )
	{
		return (fCol_3 * tex2D(samCoef8, float2((tableOffset.x+0.5f)/tableOffset.z,index)).rggr + 
				fCol_2 * tex2D(samCoef8, float2((tableOffset.x+1.5f)/tableOffset.z,index)).rggr + 
				fCol_1 * tex2D(samCoef8, float2((tableOffset.x+2.5f)/tableOffset.z,index)).rggr + 
				fCol0 * tex2D(samCoef8, float2((tableOffset.x+3.5f)/tableOffset.z,index)).rggr)
				+
				(fCol1 * tex2D(samCoef8, float2((tableOffset.x+4.5f)/tableOffset.z,index)).rggr + 
				fCol2 * tex2D(samCoef8, float2((tableOffset.x+5.5f)/tableOffset.z,index)).rggr + 
				fCol3 * tex2D(samCoef8, float2((tableOffset.x+6.5f)/tableOffset.z,index)).rggr + 
				fCol4 * tex2D(samCoef8, float2((tableOffset.x+7.5f)/tableOffset.z,index)).rggr);
	
	}
	else
	{
		return (fCol_3 * tex2D(samCoef8, float2((tableOffset.x+0.5f)/tableOffset.z,index)).r + 
				fCol_2 * tex2D(samCoef8, float2((tableOffset.x+1.5f)/tableOffset.z,index)).r + 
				fCol_1 * tex2D(samCoef8, float2((tableOffset.x+2.5f)/tableOffset.z,index)).r + 
				fCol0 * tex2D(samCoef8, float2((tableOffset.x+3.5f)/tableOffset.z,index)).r)
				+
				(fCol1 * tex2D(samCoef8, float2((tableOffset.x+4.5f)/tableOffset.z,index)).r + 
				fCol2 * tex2D(samCoef8, float2((tableOffset.x+5.5f)/tableOffset.z,index)).r + 
				fCol3 * tex2D(samCoef8, float2((tableOffset.x+6.5f)/tableOffset.z,index)).r + 
				fCol4 * tex2D(samCoef8, float2((tableOffset.x+7.5f)/tableOffset.z,index)).r);
	}
}

float4 PS_Copy( float2 TexDes : TEXCOORD0 ) : COLOR
{
	return tex2D(g_samColor, TexDes);
}

technique DownUpConverter
{
	pass Inter4H
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_DownUp4H();
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Inter4V
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_DownUp4V();
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Inter8H
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_DownUp8H(false);
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Inter8V
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_DownUp8V(false);
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Inter8H_YCbCr
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_DownUp8H(true);
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Inter8V_YCbCr
	{
		VertexShader = compile vs_3_0	VS_I4A();
		PixelShader = compile ps_3_0	PS_DownUp8V(true);
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
	}
	
	pass Copy
	{
		VertexShader = compile vs_3_0	VS_AA();
		PixelShader = compile ps_3_0	PS_Copy();
	
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE; 	
	}
}
