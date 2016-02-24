#include "StdAfx.h"
#include "ChromaKeyRender.h"

#pragma warning(disable:4244)

#define ACK_GAIN_COEF			2.097175f		// sqrt(log(1.0 + 256.0/224.0) / log(2.0)) / 0.5

#define RADIAN_TO_ANGLE				57.29578f	// 弧度到角度的变换
#define ANGLE_TO_RADIAN				0.017453f	// 角度到弧度的变换
#define ANGLE_2_PI					360			// 2 PI
#define ANGLE_2_PI_INV				0.002778
#define RADIAN_2_PI					6.283185f
#define RADIAN_PI					3.141593f	// PI
#define RADIAN_PI_INV				0.31831f
#define ANGLE_PI					180
#define ANGLE_PI_INV				0.005556

// 防止越界的宏
#define CM_IN_RANGE(x, start, end)		do{ \
										if((x)<(start)) { (x)=(start);}	\
										else if((x)>(end)) { (x)=(end); } \
										}while(0)

#define CM_IN_BETWEEN(x, start, end)	do{ \
										if((start)<(end)) { CM_IN_RANGE((x), (start), (end)); } \
										else { CM_IN_RANGE((x), (end), (start)); } \
										}while(0)


static const BYTE g_pFilterMapTable[][4] =
{
	//              { KPROC_KEYFILTSET, KPROC_SRCFILTSET, KPROC_CC_KEYFILTSET, KPROC_CC_SRCFILTSET }
	/* Filter==0 */ { 0xFF, 0xFF, 0xFF, 0xFF },	// <--don't use these values. ('cause just a padding)
	/* Filter==1 */ { 0, 0, 0, 0 },
	/* Filter==2 */ { 0, 1, 0, 1 },
	/* Filter==3 */ { 0, 2, 0, 2 },
	/* Filter==4 */ { 1, 2, 1, 2 },
	/* Filter==5 */ { 1, 3, 1, 3 },
	/* Filter==6 */ { 2, 3, 2, 3 },
	/* Filter==7 */ { 2, 4, 2, 4 },
	/* Filter==8 */ { 3, 4, 3, 4 },
	/* Filter==9 */ { 3, 5, 3, 5 }
};


struct NSACKParamTmpData	// 中间变量存储结构
{
	BOOL		bKey;
	BOOL		bInvert;
	BOOL		bCC;
	BOOL		bYBal;
	float		keyClip;
	float		keyGain;
	float		keySin;
	float		keyCos;
	float		keySlopeInv;
	int			nKeyKeyFilter;
	int			nKeySrcFilter;
	float		keySatCrop;
	float		keyYBalMix;
	float		keyYBalClip;
	float		keyYBalGain;
	float		keyYBalOffset;
	float		keyDensity;
	float		keyLeftPos;
	float		keyRightPos;
	float		ccClip;
	float		ccGain;
	float		ccSlopeInv;
	int			nCCKeyFilter;
	int			nCCSrcFilter;
	float		ccSatCrop;
	float		ccYBalMix;
	float		ccLeftPos;
	float		ccRightPos;
	float		fvYGain;
	float		fvSin;
	float		fvCos;
	float		fvCancelY;
	float		fvCancelU;
	float		fvCancelV;
};



void CALLBACK NSCE_ACK_CalcParams(OUT NSACKParamTmpData* pTmpData,		// 中间变量
								  IN ChromaKeyFxParam* pParamData)		// 参数
{
	ChromaKeyFxParam calcData;

	// 参数传递
	memset(&calcData, 0, sizeof(calcData));
	// Key
	if (pParamData->paramCrkAdj.bOn)
	{
		calcData.paramCrkAdj.fClip = pParamData->paramCrkAdj.fClip;
		calcData.paramCrkAdj.fGain = pParamData->paramCrkAdj.fGain;
		calcData.paramCrkAdj.fHue = pParamData->paramCrkAdj.fHue;
		calcData.paramCrkAdj.fAngle = pParamData->paramCrkAdj.fAngle;
		calcData.paramCrkAdj.nFilter = pParamData->paramCrkAdj.nFilter;
		calcData.paramCrkAdj.fSatCrop = pParamData->paramCrkAdj.fSatCrop;
		calcData.paramCrkAdj.fDensity = pParamData->paramCrkAdj.fDensity;
	}
	else
	{
		calcData.paramCrkAdj.fClip = 100.f;
		calcData.paramCrkAdj.fGain = 100.f;
		calcData.paramCrkAdj.fHue = 0.f;
		calcData.paramCrkAdj.fAngle = 97.59578851f;
		calcData.paramCrkAdj.nFilter = 1;
		calcData.paramCrkAdj.fSatCrop = 20.3125f;
		calcData.paramCrkAdj.fDensity = 100.f;
	}
	if (pParamData->paramCrkAdj.bInvert)
	{
		calcData.paramCrkPos.fLeft = pParamData->paramCrkPos.fRight;
		calcData.paramCrkPos.fRight = pParamData->paramCrkPos.fLeft;
	}
	else
	{
		calcData.paramCrkPos.fLeft = pParamData->paramCrkPos.fLeft;
		calcData.paramCrkPos.fRight = pParamData->paramCrkPos.fRight;
	}
	// CC
	if (pParamData->paramCc.bOn)
	{
		calcData.paramCcKey.fClip = pParamData->paramCcKey.fClip;
		calcData.paramCcKey.fGain = pParamData->paramCcKey.fGain;
		calcData.paramVecWnd.fAngle = pParamData->paramVecWnd.fAngle;
		calcData.paramVecWnd.fSatCrop = pParamData->paramVecWnd.fSatCrop;
		calcData.paramCc.fLum = pParamData->paramCc.fLum;
		calcData.paramCc.fSat = pParamData->paramCc.fSat;
		calcData.paramCc.fHue = pParamData->paramCc.fHue;
		calcData.paramCc.nFilter = pParamData->paramCc.nFilter;
	}
	else
	{
		calcData.paramCc.nFilter = 1;
	}
	calcData.paramCckPos.fLeft = pParamData->paramCckPos.fLeft;
	calcData.paramCckPos.fRight = pParamData->paramCckPos.fRight;
	// YBAL
	memcpy(&(calcData.paramYBal), &(pParamData->paramYBal), sizeof(ChromaKeyFxParam::YBalance));
	// Y Balance Mix
	calcData.paramYBalMix.fMix = pParamData->paramYBalMix.fMix;
	// FV
	memcpy(&(calcData.paramFGVAdj), &(pParamData->paramFGVAdj), sizeof(ChromaKeyFxParam::ForegroundVideoAdjust));


	//////////////////////////////////////////////////////////////////////////
	// 开始计算
	pTmpData->bKey = pParamData->paramCrkAdj.bOn;
	pTmpData->bInvert = pParamData->paramCrkAdj.bInvert;
	pTmpData->bYBal = pParamData->paramYBal.bOn;
	pTmpData->bCC = pParamData->paramCc.bOn;
	// Key Clip
	pTmpData->keyClip = calcData.paramCrkAdj.fClip * 0.01f;
	// CC Clip
	pTmpData->ccClip = calcData.paramCcKey.fClip * 0.01f;
	// Key Gain
	float tmpGain = (calcData.paramCrkAdj.fGain + 100.f) * ACK_GAIN_COEF * 0.005f;
	tmpGain *= tmpGain;
	pTmpData->keyGain = (powf(2.f, tmpGain) - 1.f);
	// CC Gain
	if (calcData.paramCcKey.fGain >= 99.951172f)
	{
		pTmpData->ccGain = 2048.f;
	}
	else
	{
		pTmpData->ccGain = 100.f / (100.f - calcData.paramCcKey.fGain);
	}
	// Key Hue
	float tmpHue = calcData.paramCrkAdj.fHue * RADIAN_PI * ANGLE_PI_INV;
	pTmpData->keySin = sinf(tmpHue);
	pTmpData->keyCos = cosf(tmpHue);
	// Key Angle
	if (calcData.paramCrkAdj.fAngle < 0.02f)
	{
		pTmpData->keySlopeInv = 32.f;
	}
	else
	{
		pTmpData->keySlopeInv = tanf((RADIAN_PI - calcData.paramCrkAdj.fAngle * ANGLE_TO_RADIAN) * 0.5f);
		CM_IN_RANGE(pTmpData->keySlopeInv, 0.f, 32.f);
	}
	// CC Angle
	if (calcData.paramVecWnd.fAngle < 0.02f)
	{
		pTmpData->ccSlopeInv = 32.f;
	}
	else
	{
		pTmpData->ccSlopeInv = tanf((RADIAN_PI - calcData.paramVecWnd.fAngle * ANGLE_TO_RADIAN) * 0.5f);
		CM_IN_RANGE(pTmpData->ccSlopeInv, 0.f, 32.f);
	}
	// Key Filter
	pTmpData->nKeyKeyFilter = g_pFilterMapTable[calcData.paramCrkAdj.nFilter][0];
	pTmpData->nKeySrcFilter = g_pFilterMapTable[calcData.paramCrkAdj.nFilter][1];
	// CC Filter
	pTmpData->nCCKeyFilter = g_pFilterMapTable[calcData.paramCc.nFilter][2];
	pTmpData->nCCSrcFilter = g_pFilterMapTable[calcData.paramCc.nFilter][3];
	// Key SatCrop
	pTmpData->keySatCrop = calcData.paramCrkAdj.fSatCrop * 0.01f;
	// CC SatCrop
	pTmpData->ccSatCrop = calcData.paramVecWnd.fSatCrop * 0.01f;
	// Key Density
	pTmpData->keyDensity = calcData.paramCrkAdj.fDensity * 0.01f;
	// Key Pos
	pTmpData->keyLeftPos = calcData.paramCrkPos.fLeft;
	pTmpData->keyRightPos = calcData.paramCrkPos.fRight;
	// CC Pos
	pTmpData->ccLeftPos = calcData.paramCckPos.fLeft;
	pTmpData->ccRightPos = calcData.paramCckPos.fRight;
	// FV
	tmpGain = (calcData.paramFGVAdj.fVGain * 0.01f + 1.f);
	tmpHue = calcData.paramFGVAdj.fHue * ANGLE_TO_RADIAN;
	pTmpData->fvYGain = tmpGain * (calcData.paramFGVAdj.fYGain * 0.01f + 1.f);
	tmpGain *= (calcData.paramFGVAdj.fCGain * 0.01f + 1.f);
	pTmpData->fvSin = tmpGain * sinf(tmpHue);
	pTmpData->fvCos = tmpGain * cosf(tmpHue);
	pTmpData->fvCancelY = pTmpData->fvYGain * calcData.paramCc.fLum * 0.0085547f;
	tmpHue = calcData.paramCc.fHue * ANGLE_TO_RADIAN;
	tmpGain = calcData.paramCc.fSat * 0.01f;
	pTmpData->fvCancelU = cosf(tmpHue) * tmpGain;
	pTmpData->fvCancelV = sinf(tmpHue) * tmpGain;
	// Y Bal
	pTmpData->keyYBalMix = (pTmpData->bYBal) ? 0.44921875f : 0.f;
	pTmpData->keyYBalClip = 1.f - calcData.paramYBal.fClip * 0.01f;
	pTmpData->keyYBalGain = (powf(2.f, (calcData.paramYBal.fGain + 100.f) * 0.03f) - 1.f) * 0.0625f;
	pTmpData->keyYBalOffset = calcData.paramYBal.fLum * 0.01f;
	pTmpData->ccYBalMix = calcData.paramYBalMix.fMix * 0.0022456f;
}






ChromaKeyRender::ChromaKeyRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_PS_CRK_RGB32(NULL)
, m_PS_CRK_YUVA(NULL)
{
}

ChromaKeyRender::~ChromaKeyRender(void)
{
}

bool ChromaKeyRender::Init( CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);

	//TODO: no file. need to copy VS_DirectOut_2stages_V3.vsh
	//m_VS_DirectOut_2stages_V3 = pResMan->CreateVertexShader(pDevice, _T("Shaders/VS_DirectOut_2stages_V3.vsh"));
	m_VS_DirectOut_2stages_V3 = pResMan->CreateVertexShader(pDevice, _T("Shaders/VS_DirectOut_2stages_V2.vsh"));
	ASSERT(m_VS_DirectOut_2stages_V3);
	m_PS_CRK_RGB32 = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_CRK_RGBA_QUICK.psh"));
	ASSERT(m_PS_CRK_RGB32);
	m_PS_CRK_YUVA = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_CRK_YUVA.psh"));
	ASSERT(m_PS_CRK_YUVA );
	return true;
}

void ChromaKeyRender::Uninit()
{

}

bool ChromaKeyRender::Render(CVideoBuffer*pDstDef, CVideoBuffer *pSrcDef, FxParamBase* pParam)
{
	RESET_RENDER_TARGET(m_pEngine);
	return RenderArea(pDstDef, pSrcDef, pParam);
}

bool ChromaKeyRender::Render(CVideoBuffer*pDstDef, CVideoBuffer *pSrcA, CVideoBuffer* pSrcB, FxParamBase* pParam)
{
	RESET_RENDER_TARGET(m_pEngine);
	CVideoBuffer* pTempDef = m_pEngine->CreateRenderTargetBuffer();//CreateVideoBuffer(pDst->GetVideoBufferInfo());
	bool bOK = RenderArea(pTempDef, pSrcA, pParam);
	ASSERT(bOK);
	bOK = m_pEngine->BlendCompose(pDstDef, pSrcB, pTempDef);
	ASSERT(bOK);
	bool bOK2 = m_pEngine->GetVideoBufferManager()->ReleaseVideoBuffer(pTempDef);
	ASSERT(bOK2);
	return bOK;
}

bool ChromaKeyRender::RenderArea(CVideoBuffer*pDstDef, CVideoBuffer *pSrcDef, FxParamBase* pParamRaw)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVBM = m_pEngine->GetVideoBufferManager();
	//CGPURender::Base_Render(pSrcDef, pDstDef);
	ChromaKeyFxParam* pParam = (ChromaKeyFxParam*)pParamRaw;

	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo();

	// 如果不需要特技处理
	bool bYUYV = false;		//(pSrcDef->IsYUV16Buffer());
	bool bAlpha = false;	//(pSrcDef->pAlpha);
	//bool bYUYVA = bYUYV&&bAlpha;
	bool bYUVA = false;	//(pSrcDef->bufferFormat == FMT_YUVA32);

	//if( pDstDef->IsYUV16Buffer() && pDstDef->pAlpha==NULL )
	//{
	//	pDstDef->pAlpha = m_pEngine->AllocateVideoAlphaRT();
	//	assert(pDstDef->pAlpha);
	//}

	// select pixel shader
	CPixelShader* pixelshader = m_PS_CRK_RGB32;
	//if ( bYUYV )
	//	pixelshader = bAlpha ? m_PS_CRK_YUYV_AP : m_PS_CRK_YUYV_NA;
	//else 
	if (bYUVA)
		pixelshader = m_PS_CRK_YUVA;
	pixelshader->SetYUVA2RGBAMatrix();

	// 2.1 setRT和标志位设定	
	//pDstDef->bRenderToFullTarget = false;
	//pDstDef->bDiscardAlpha = false;
	//SetRenderTarget(0, pDstDef->handle, pDstDef->COLOR_BLACK(), 0x0);
	bool bOK = m_pEngine->SetRenderTarget(pDstDef);
	ASSERT(bOK);

	// 2.2.1 constant
	//D3DXMATRIX matWVP = (m_matView * m_matProj);
	D3DXMATRIX *matView = NULL, *matProj= NULL;
	pResMan->GetPerspectiveMatrix( &matView, &matProj);
	D3DXMATRIX matWVP = *matView * *matProj;	
	HRESULT hr = pDevice->SetVertexShaderConstantF(0, (float*)&matWVP, 4);
	ASSERT(SUCCEEDED(hr));
	D3DXMATRIX matTexSrc;
	bOK = ::GenerateMatrix(pSrcDef, &matTexSrc, mat_Image);
	ASSERT(bOK);
	hr = pDevice->SetVertexShaderConstantF(4, (float*)&matTexSrc, 4);
	ASSERT(SUCCEEDED(hr));

	// 参数传递
	NSACKParamTmpData   tmpData;
	NSCE_ACK_CalcParams(&tmpData, pParam);
	TRACE(_T("paramCrkAdj.fAngle:%f, keySlopeInv:%f, ccSlopeInv:%f\n"), pParam->paramCrkAdj.fAngle, tmpData.keySlopeInv, tmpData.ccSlopeInv);
	// 	float4		fCoefU		: register(c20);	// [ keyCos, -keySin, fvCos, fvSin ]
	// 	float4		fCoefV		: register(c21);	// [ keySin, keyCos, -fvSin, fvCos ]
	//float4		fCoefYK		: register(c26);	// [ keyYBalMix * keyGain, ccYBalMix * ccGain, keySlopeInv * keyGain, ccSlopeInv * ccGain ]
	//float4		fYKOff		: register(c27);	// [ keyClip * keyGain, ccClip * ccGain, 0.f, 0.f ]
	//float4		fGain		: register(c28);	// [ -keyGain, -ccGain, 0.f, 0.f ]
	D3DXVECTOR4	fCoefU( tmpData.keyCos, -tmpData.keySin, tmpData.fvCos, tmpData.fvSin );
	D3DXVECTOR4	fCoefV( tmpData.keySin, tmpData.keyCos, -tmpData.fvSin, tmpData.fvCos );
	D3DXVECTOR4 fCoefYK( tmpData.keyYBalMix * tmpData.keyGain, tmpData.ccYBalMix * tmpData.ccGain, tmpData.keySlopeInv * tmpData.keyGain, tmpData.ccSlopeInv * tmpData.ccGain );
	D3DXVECTOR4 fYKOff( tmpData.keyClip * tmpData.keyGain, tmpData.ccClip * tmpData.ccGain, 0.f, 0.f);
	D3DXVECTOR4 fGain( -tmpData.keyGain, -tmpData.ccGain, 0.f, 0.f );
	D3DXVECTOR4 fCoefY, fCoefYBal, fCoefCan, fSatCrop, fInvert, fInvCoef;
	if( !bYUYV && !bYUVA)
	{
		// 	float4		fCoefY		: register(c22);	// [ keyYBalGain, fvYGain, 0.f, 0.f ]
		// 	float4		fCoefYBal	: register(c23);	// [ -keyYBalOffset * keyYBalGain, 0.f, 0.f, -keyYBalClip * keyYBalGain]
		// 	float4		fCoefCan	: register(c24);	// [ 0.f, fvCancelY, fvCancelU, fvCancelV] * bCC
		// 	float4		fSatCrop	: register(c25);	// [ 0.f, 0.f, -keySatCrop, -ccSatCrop ]
		// 	float4		fInvert		: register(c29);	// [ bInvert, -1.f, 0.f, 0.f ]
		//	float4		fInvCoef	: register(c30);	// [ (bInvert ? -1 : 1) * keyDensity, 1.f, 0.f, 0.f ]
		fCoefY = D3DXVECTOR4( tmpData.keyYBalGain, tmpData.fvYGain, 0.f, 0.f );
		fCoefYBal = D3DXVECTOR4( -tmpData.keyYBalOffset * tmpData.keyYBalGain, 0.f, 0.f, -tmpData.keyYBalClip * tmpData.keyYBalGain );
		fCoefCan = D3DXVECTOR4( 0.f, tmpData.fvCancelY * tmpData.bCC, tmpData.fvCancelU * tmpData.bCC, tmpData.fvCancelV * tmpData.bCC );
		fSatCrop = D3DXVECTOR4( 0.f, 0.f, -tmpData.keySatCrop, -tmpData.ccSatCrop );
		fInvert = D3DXVECTOR4( tmpData.bInvert, -1.f, 0.f, 0.f );
		fInvCoef = D3DXVECTOR4( (tmpData.bInvert) ? -tmpData.keyDensity : tmpData.keyDensity, 1.f, 0.f, 0.f );
	}
	else
	{
		//float4		fCoefY		: register(c22);	// [ keyYBalGain, fvYGain, keyYBalGain, fvYGain ]
		//float4		fCoefYBal	: register(c23);	// [ -keyYBalOffset * keyYBalGain, 0.f, -keyYBalOffset * keyYBalGain, 0.f ]
		//float4		fCoefCan	: register(c24);	// [ fvCancelY, fvCancelY, fvCancelU, fvCancelV] * bCC
		//float4		fSatCrop	: register(c25);	// [ -keyYBalClip * keyYBalGain, -keyYBalClip * keyYBalGain, -keySatCrop, -ccSatCrop ]
		//float4		fInvert		: register(c29);	// [ bInvert, -1.f, bInvert, -1.f ]
		//float4		fInvCoef	: register(c30);	// [ (bInvert ? -1 : 1) * keyDensity, 1.f, (bInvert ? -1 : 1) * keyDensity, 1.f ]
		fCoefY = D3DXVECTOR4( tmpData.keyYBalGain, tmpData.fvYGain, tmpData.keyYBalGain, tmpData.fvYGain );
		fCoefYBal = D3DXVECTOR4( -tmpData.keyYBalOffset * tmpData.keyYBalGain, 0.f, -tmpData.keyYBalOffset * tmpData.keyYBalGain, 0.f );
		fCoefCan = D3DXVECTOR4( tmpData.fvCancelY * tmpData.bCC, tmpData.fvCancelY * tmpData.bCC, tmpData.fvCancelU * tmpData.bCC, tmpData.fvCancelV * tmpData.bCC );
		fSatCrop = D3DXVECTOR4( -tmpData.keyYBalClip * tmpData.keyYBalGain, -tmpData.keyYBalClip * tmpData.keyYBalGain, -tmpData.keySatCrop, -tmpData.ccSatCrop );
		fInvert = D3DXVECTOR4( tmpData.bInvert, -1.f, tmpData.bInvert, -1.f );
		fInvCoef = D3DXVECTOR4( (tmpData.bInvert) ? -tmpData.keyDensity : tmpData.keyDensity, 1.f, (tmpData.bInvert) ? -tmpData.keyDensity : tmpData.keyDensity, 1.f );
		if (bYUYV)
		{
			int pSrcDef_GetImageWidth = biSrc.nWidth;
			D3DXVECTOR2 fPixWidth = D3DXVECTOR2(1.f / pSrcDef_GetImageWidth, 0.f);
			hr = pDevice->SetPixelShaderConstantF(31, fPixWidth, 1);
			ASSERT(SUCCEEDED(hr));
		}
	}
	hr = pDevice->SetPixelShaderConstantF(20, fCoefU, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(21, fCoefV, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(22, fCoefY, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(23, fCoefYBal, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(24, fCoefCan, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(25, fSatCrop, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(26, fCoefYK, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(27, fYKOff, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(28, fGain, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(29, fInvert, 1);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetPixelShaderConstantF(30, fInvCoef, 1);

	// 2.2.2 texture
	hr = pDevice->SetTexture(0, pSrcDef->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	ASSERT(SUCCEEDED(hr));
	//if( bYUYVA )
	//{
	//	pDevice->SetSourceTexture(1, pSrcDef->pAlpha);
	//	pDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	//	pDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	//	pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//	pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//}

	// 2.2.3 render state
	hr = pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	ASSERT(SUCCEEDED(hr));
	hr = pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	ASSERT(SUCCEEDED(hr));

	// 2.2.4 shader
	//CVertexShader* vertexshader = pResMan->CreateVertexShader(pDevice, _T("Shaders/VSDirectOut_2stages_V3.vsh"));
	CVertexShader* vertexshader = m_VS_DirectOut_2stages_V3;
	hr = pDevice->SetPixelShader(pixelshader->GetPixelShaderPtr());
	ASSERT(SUCCEEDED(hr));

	// 2.3 render
	if ( SUCCEEDED(pDevice->BeginScene()) )
	{
		bOK = m_pQuadMesh->DrawMesh(0, vertexshader->GetVertexShaderPtr());
		ASSERT(bOK);
		hr = pDevice->EndScene();
		ASSERT(SUCCEEDED(hr));
	}

	pDevice->SetPixelShader(NULL);
	pDevice->SetTexture(0, NULL);
	pDevice->SetTexture(1, NULL);
	pDevice->SetTexture(2, NULL);
	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	//D3DXSaveSurfaceToFile(_T("./ChromaKey.dds"), D3DXIFF_DDS, pDstDef->GetSurface(), NULL, NULL);

	return true;
}
