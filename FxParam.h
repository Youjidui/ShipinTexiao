#pragma once

#include "VideoBuffer.h"
#include <d3dx9math.h>

struct NegativeFxParam : FxParamBase
{

};

struct SonyBlurFxParam : FxParamBase
{
	float blurX;
	float blurY;
};

#define MIN_LENGTH      0.0f
#define MAX_LENGTH      1.0f
#define LENGTH_SCALE    2.0f
#define DEF_LENGTH      0.45f

#define YUV_ANGLE_ERROR (D3DX_PI/12.0f)
#define DEF_ANGLE       (D3DX_PI/12.0f)

struct ColorKeyParam : public FxParamBase
{
	DWORD version;
	float fKeyColor[4];	//r,g,b,a
	float fAngle1, fAngle2, fLength1, fLength2;
	float fSoftness, fBlurness, fFineTune;
	float fGrayMin,fGrayMax;
	BOOL bIsReverse;
	ColorKeyParam()
	{        
		ResetAllParam();
	}

	void ResetAllParam()
	{
		version = 0x0100;
		fKeyColor[0] = fKeyColor[1] = fKeyColor[2] = 0.0f; fKeyColor[3] = 1.0f;
		fAngle1 = D3DX_PI/2.0f - D3DX_PI/12.0f + YUV_ANGLE_ERROR;
		fAngle2 = D3DX_PI/2.0f + D3DX_PI/12.0f + YUV_ANGLE_ERROR;
		fLength1 = MIN_LENGTH / LENGTH_SCALE;
		fLength2 = MAX_LENGTH / LENGTH_SCALE;
		fSoftness = 0.0f;
		fBlurness = 0.0f;
		fFineTune = 0.0f;
		fGrayMin=0.0f;
		fGrayMax=1.0f;
		bIsReverse=FALSE;
	}	

};


struct AmoebaWipeFxParam : FxParamBase
{
	//progress
	float	fOffset;
	//bump and blur
	float	fPhase;
	float	fBumpDensity;
	float	fSlant;
	float	fHeight;
	float	fSoftEdge;
	int		nRandomSeed;
	//light
	float	fBrightness;
	float	fLightAngle;
	D3DCOLOR	crHColor;
	D3DCOLOR	crLColor;
};

// begin push param
struct PushFxParam : FxParamBase
{
	DWORD dwType ;
	float fTransition;
	BOOL  bReverse;
	float fFirstSoftness;
	DWORD dwFirstColor;
	float fSecondSoftness;
	DWORD dwSecondColor;
};

enum Push_Type{
   Right2Left = 0,
   Left2Right,
   Bottom2Top,
   Top2Bottom,
   BottomRight,
   BottomLeft,
   TopLeft,
   TopRight,


   NUM 
};
// end push param

struct BarmWipeFxParam : FxParamBase
{
	struct Pattern
	{
		int nPattern;
		BOOL bInvert;
		float fOffset;
		float fRotate;
		float fCenterX;
		float fCenterY;
		float fAspect;
		float fSoftWidth;
		float fBorderWidth;
		D3DCOLOR crBorderColor;

		Pattern()
			: nPattern(0), bInvert(FALSE), fOffset(0.f), fRotate(0.f), fCenterX(0.f), fCenterY(0.f),
			fAspect(1920.f/1080.f), fSoftWidth(0.f), fBorderWidth(0.f), crBorderColor(0xffc0c0c0)
		{		}
	};
	struct Modify
	{
		int		nMultipleType;
		BOOL	bFlip;
		int		nMultipleNumberX;
		int		nMultipleNumberY;
		BOOL	bOverlap;
		int		nDivideType;
		float   fDivideWidth;
	};
	struct General
	{
		float fTransparency;
		General()
		{
			fTransparency = 1.0f;
		}
	};

	Pattern     structPattern;
	Modify      structModify;
	General     structGeneral;   
	float fSlant;
} ;


// begin SonySlide

struct SonySlideFxParam : FxParamBase
{
	DWORD dwType;
	float fTransition;
	BOOL bReverse;
};

// end SonySlide

struct SonyMaskFxParam : FxParamBase	//SONY_MASK_INFO
{
	DWORD crBorder;
	BOOL      bInvert;
	BOOL      bHasBorder;
	float     fAlpha;
	int		  nArea;
	RECT      rcMask[3];
	int       nNumMask;
	int		  nMaskType;

	// add 2010年2月25日 星期四
	BOOL		bSufColor;
	DWORD       crSuface;
	float		fTransparency;
	BOOL	  bEnableAll;

	BOOL bBorderWithKey;

	SonyMaskFxParam()
	{
		bHasBorder = FALSE;
		bInvert = FALSE;
		crBorder = 0;
		fAlpha = 1.0f;
		nArea = 0;
		nNumMask = 0;
		nMaskType = 0;// 0 Berize 1 Fix Shape

		// 
		bSufColor = FALSE;
		crSuface = 0;
		fTransparency = 0.5f;
		bEnableAll = FALSE;
		bBorderWithKey = FALSE;
	}
};//SONY_MASK_INFO;

struct ChromaKeyFxParam : FxParamBase
{
	struct ChromaKeyAdjust
	{
		bool bOn;
		float fClip;
		float fGain;
		float fHue;
		float fAngle;
		int nFilter;
		float fSatCrop;
		float fDensity;
		bool bInvert;
		ChromaKeyAdjust()
			: bOn(true), fClip(100.f), fGain(100.f), fHue(0.f), fAngle(97.6f), nFilter(1), fDensity(100.f), bInvert(false) {}
	}paramCrkAdj;

	struct ChromaKeyPosition
	{
		float fLeft;
		float fRight;
		ChromaKeyPosition() : fLeft(0.f), fRight(0.f) {}
	}paramCrkPos;

	struct YBalance
	{
		bool bOn;
		float fClip;
		float fGain;
		float fLum;
		YBalance() : bOn(false), fClip(0.f), fGain(0.f), fLum(0.f) {}
	}paramYBal;

	struct ColorCancel
	{
		bool bOn;
		float fLum;
		float fSat;
		float fHue;
		int nFilter;
		ColorCancel(): bOn(false), fLum(0.f), fSat(0.f), fHue(0.f), nFilter(1) {}
	}paramCc;

	struct CancelKey
	{
		float fClip;
		float fGain;
		CancelKey() : fClip(0.f), fGain(0.f) {}
	}paramCcKey;

	struct CancelKeyPosition
	{
		float fLeft;
		float fRight;
		CancelKeyPosition() : fLeft(0.f), fRight(0.f) {}
	}paramCckPos;

	struct VectorWindow
	{
		float fSatCrop;
		float fAngle;
		VectorWindow() : fSatCrop(20.31f), fAngle(97.6f) {}
	}paramVecWnd;

	struct YBalanceMix
	{
		float fMix;
		YBalanceMix() : fMix(0.f) {}
	}paramYBalMix;

	struct ForegroundVideoAdjust
	{
		float fVGain;
		float fYGain;
		float fCGain;
		float fHue;
		ForegroundVideoAdjust() : fVGain(0.f), fYGain(0.f), fCGain(0.f), fHue(0.f) {}
	}paramFGVAdj;
};
