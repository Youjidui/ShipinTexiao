// ****************************************************************************
// ������	������ɫ�ռ�֮��ı任��ʽ��ӿڶ���
//			����ϵͳ��Ҫ��ֻ���� CPU һ�ױ任���壬�κ�ʹ����ɫ�任�ĵط�������
//			������ͷ�ļ���ȷ������ϵͳ����ɫ��ͳһ�ԡ�
// ·����	TPCommonData\TPBufferOperate\TPColorSpaceConv.h
// �޸ģ�	��־�� @ 2007-07-24
// ****************************************************************************

///////////////////////////////////////////////////////////////////////////////
// ������������������Ϊ��ͳһ GPU ������ RGB �� YUV ����ɫ�ռ�任
// ����ʹ���� RGB �� YUV �任�� GPU ���붼���������ͷ�ļ�
// ���� GPU ����������ԣ�������ͷ�ļ������ڸ��� FXO �� PSO �ж��������±���
// ��ʹ����Щ GPU ����ǰ����Ҫ�� CPU �������Ȼ�ȡת����ʽ��Ȼ�����ñ���

float4x4 matRGBA2YUVA;
float4x4 matYUVA2RGBA;

///////////////////////////////////////////////////////////////////////////////
// UV ƫ�ƶ���

#define CS_UV_OFFSET		128.f / 255.f
#define CS_UV_OFFSET_2		256.f / 255.f

#define cYUYVMode			float4(0.0f, CS_UV_OFFSET, 0.0f, CS_UV_OFFSET)
#define cYUVAMode			float4(0.0f, CS_UV_OFFSET, CS_UV_OFFSET, 0.0f)
#define CS_YUYV_OFFSET		cYUYVMode
#define CS_YUVA_OFFSET		cYUVAMode
#define LUM_LIMIT_16_235(y) ( (y) * (235.0f - 16.0f) / 255.0f + 16 / 255.0f)
#define LUM_LIMIT_16_235_INV(y)	( ((y)-16/255.f) * 255.f / (235.f-16.f) )

///////////////////////////////////////////////////////////////////////////////
// �ӿڶ���

// YUVA �� RGBA ֮��ı任

float CS_RGB2Y(in float3 rgb)
{
	return dot(matRGBA2YUVA[0], rgb);
}

float CS_RGB2U(in float3 rgb)
{
	return dot(matRGBA2YUVA[1], rgb);
}

float CS_RGB2V(in float3 rgb)
{
	return dot(matRGBA2YUVA[2], rgb);
}

float4 CS_RGBA2YUVA(in float4 rgba)
{
	float3 yuv = mul(matRGBA2YUVA, rgba.rgb);
	return float4(yuv, rgba.a);
}

float3 CS_RGB2YUV(in float3 rgb)
{
	return mul(matRGBA2YUVA, rgb);
}

void CS_RGBA2YUYVA(in float4 rgba0, in float4 rgba1, out float4 yuyv, out float4 aaaa)
{
	float3 yuv0 = mul(matRGBA2YUVA, rgba0.rgb);
	float y1 = dot(matRGBA2YUVA[0], rgba1.rgb);
	yuyv = float4(y1, yuv0.g, yuv0.r, yuv0.b);
	aaaa = float4(rgba1.a, rgba0.a, rgba0.a, rgba1.a);
}

// UV ��ֵ
void CS_RGBA2YUYVA_IPUV(in float4 rgba0, in float4 rgba1, out float4 yuyv, out float4 aaaa)
{
	float3 yuv0 = mul(matRGBA2YUVA, rgba0.rgb);
	float3 yuv1 = mul(matRGBA2YUVA, rgba1.rgb);
	float2 uv = (yuv0.gb + yuv1.gb) * 0.5f;
	yuyv = float4(yuv1.r, uv.x, yuv0.r, uv.y);
	aaaa = float4(rgba1.a, rgba0.a, rgba0.a, rgba1.a);
}

float4 CS_RGBA2YUYV(in float4 rgb0, in float4 rgb1)
{
	float3 yuv0 = mul(matRGBA2YUVA, rgb0.rgb);
	float y1 = dot(matRGBA2YUVA[0], rgb1.rgb);
	return float4(y1, yuv0.g, yuv0.r, yuv0.b);
}

// UV ��ֵ
float4 CS_RGBA2YUYV_IPUV(in float4 rgb0, in float4 rgb1)
{
	float3 yuv0 = mul(matRGBA2YUVA, rgb0.rgb);
	float3 yuv1 = mul(matRGBA2YUVA, rgb1.rgb);
	float2 uv = (yuv0.gb + yuv1.gb) * 0.5f;
	return float4(yuv1.r, uv.x, yuv0.r, uv.y);
}

float3 CS_YUV2RGB(in float3 yuv)
{
	return saturate(mul(matYUVA2RGBA, yuv));
}

float4 CS_YUVA2RGBA(in float4 yuva)
{
	float3 rgb = saturate(mul(matYUVA2RGBA, yuva.xyz));
	return float4(rgb, yuva.a);
}

void CS_YUYVA2RGBA(in float4 yuyv, in float4 aaaa, out float4 rgba0, out float4 rgba1)
{
	rgba0.rgb = mul(matYUVA2RGBA, yuyv.zyw);
	rgba0.a = aaaa.y;
	rgba1.rgb = mul(matYUVA2RGBA, yuyv.xyw);
	rgba1.a = aaaa.a;
}

///////////////////////////////////////////////////////////////////////////////
// ���ݾɰ汾

#define ColorSpace_RGBToYUV			CS_RGB2YUV
#define ColorSpace_RGBAToYUYU_AAAA	CS_RGBA2YUYVA
#define ColorSpace_YUVToRGB			CS_YUV2RGB
#define ColorSpace_YUYVToRGBA		CS_YUYVA2RGBA

///////////////////////////////////////////////////////////////////////////////
// �����ӿ�

float3 multiplyKernel(in float3 s0, in float3 s1)
{
    float3 rColor = s0 * s1 + 0.5 / 256.0;
    return (rColor * (1.0 / 256.0) + rColor);
}

// ���������
float GPURand(in float2 f2Pos, in float fSeed)
{
	float fRand = f2Pos.y*2107.f + f2Pos.x*417.f + fSeed*6807.f;

	fRand = fmod(fRand*fRand, 16807.f);
	fRand = fmod(fRand*fRand, 16807.f);
	fRand = fmod(fRand*fRand, 16807.f);
	
	fRand = fRand/16807.f;
	//fRand = cos(fRand*3.1415926f)*0.5f+0.5f;
	return fRand;
}