#pragma once

#include "VideoBuffer.h"
#include <d3dx9math.h>

struct NegativeFxParam : FxParamBase
{

};

struct ColorConvertFxParam : FxParamBase
{
	enum _color_convert_direction { RGBA2YUVA, YUVA2RGBA }
	convert_dir;
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

struct BasicWipeFxParam : FxParamBase
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

		Modify() :
			nMultipleType(0),
			bFlip(FALSE),
			nMultipleNumberX(1),
			nMultipleNumberY(1),
			bOverlap(FALSE),
			nDivideType(1),
			fDivideWidth(0.f)
		{
		}
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
};

struct BarmWipeFxParam : BasicWipeFxParam
{
	float fSlant;

	BarmWipeFxParam() : fSlant(0.f) {}
} ;

struct MatrixWipeFxParam :public BasicWipeFxParam
{
	float	fTitleSize;
	int		nSequence;

	MatrixWipeFxParam() : fTitleSize(0.5f), nSequence(0) {}
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
	//NSCEParamDef.cpp  line:990
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
			: bOn(true), fClip(100.f), fGain(100.f), fHue(0.f), fAngle(97.59578851f), nFilter(1), fSatCrop(20.3125f), fDensity(100.f), bInvert(false) {}
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

struct PageRollFxParam : public FxParamBase
{	
	struct Geometry
	{
		float fAngle;
		float fRadius;

		Geometry() : fAngle(160.f), fRadius(0.35f) 	{}
	};
	struct Lighting
	{
		float fAnglePhi;
		float fAngleTheta;
		float fLightRatio;
		float fFrontHighLight;
		float fFrontSmoothness;
		float fRearHighLight;
		float fRearSmoothness;

		Lighting() : fAnglePhi(220.f), fAngleTheta(270.f), fLightRatio(0.2f), fFrontHighLight(10.f), fFrontSmoothness(15.f), fRearHighLight(70.f), fRearSmoothness(15.f) {}
	};
	struct Rear {
		BOOL bUseForeGround;
		float fMatteRatio;
		D3DCOLOR crMaatte;

		Rear() : bUseForeGround(TRUE), fMatteRatio(0.0f), crMaatte(0xffc0c0c0) {}
	};
	struct Transition {
		float fTransition;
		BOOL bReverse;

		Transition() : fTransition(0.0f), bReverse(FALSE) {}
	};

	Geometry structGeometry;
	Lighting structLight;
	Rear     structRear;
	Transition structTrans;
	BOOL bPageRoll;

	PageRollFxParam()
		: bPageRoll(true)
	{
	}
};

struct QuadPageRollFxParam : public PageRollFxParam
{
	float	fAngle[4];
	int		nStepPattern;
	int		nGroupPattern;

	QuadPageRollFxParam()
	{
		fAngle[0] = 45.f;
		fAngle[1] = 315.f;
		fAngle[2] = 135.f;
		fAngle[3] = 225.f;

		nStepPattern = 0;
		nGroupPattern = 0;
	}
};

struct SonyDME3DTransfromFxPrarm : public FxParamBase
{
	float fPreSizeX;
	float fPreSizeY;
	float fSkewX;
	float fSkewY;

	struct  _Local 
	{
		float fLocationX;
		float fLocationY;
		float fLocationZ;

		float fSpinX;
		float fSpinY;
		float fSpinZ;

		float fRotationX;
		float fRotationY;
		float fRotationZ;

		float fSrcSpinX;
		float fSrcSpinY;
		float fSrcSpinZ;

		float fAxisX;
		float fAxisY;
		float fAxisZ;

	}sLocal;

	struct _Global  
	{
		float fLocationX;
		float fLocationY;
		float fLocationZ;

		float fSpinX;
		float fSpinY;
		float fSpinZ;

		float fRotationX;
		float fRotationY;
		float fRotationZ;

		float fSrcSpinX;
		float fSrcSpinY;
		float fSrcSpinZ;

		float fAxisX;
		float fAxisY;
		float fAxisZ;

	}sGlobal;

	bool  bFilter;
	float fPerspective;

	float fPostSizeX;
	float fPostSizeY;
	float fPostLocationX;
	float fPostLocationY;
};

struct RingsFxParam : public FxParamBase
{
	int			nPattern;
	float		fTranslate;
	float		fRandomTranslate;
	float		fRotate;
	float		fCenterX;
	float		fCenterY;
	float		fAspect;
	float		fWidth;
	float		fRandomWidth;
	float		fSpiral;
	float		fRandomPixel;		
	int			nEffectNo;
};

#define _3D_CUBE_TRANS

struct CubeFxParam : public FxParamBase
{
	struct _Transform
	{
		float fLocalTranslateX;
		float fLocalTranslateY;
		float fLocalTranslateZ;

		float fLocalRotateX;
		float fLocalRotateY;
		float fLocalRotateZ;

		float fScaleX;
		float fScaleY;
		float fScaleZ;

		float fWorldTranslateX;
		float fWorldTranslateY;
		float fWorldTranslateZ;

		float fWorldRotateX;
		float fWorldRotateY;
		float fWorldRotateZ;

		_Transform() : 
			fLocalTranslateX(0.f),
			fLocalTranslateY(0.f),
			fLocalTranslateZ(0.f),

			fLocalRotateX(0.f),
			fLocalRotateY(0.f),
			fLocalRotateZ(0.f),

			fScaleX(1.f),
			fScaleY(1.f),
			fScaleZ(1.f),

			fWorldTranslateX(0.f),
			fWorldTranslateY(0.f),
			fWorldTranslateZ(0.f),

			fWorldRotateX(0.f),
			fWorldRotateY(0.f),
			fWorldRotateZ(0.f)
		{
		}
	};
	struct _Light 
	{
		BOOL bEnable;
		float fDirectionX;
		float fDirectionY;
		float fDiffuse;
		float fAmbient;
		_Light():
			bEnable(FALSE),
			fDirectionX(0.f),
			fDirectionY(0.f),
			fDiffuse(0.f),
			fAmbient(1.f)
		{
		}
	};
	struct _Shape 
	{
		int			nDiveX;
		int			nDiveY;
		float		fIntervalX;
		float		fIntervalY;
		D3DCOLOR	crSlideColor;
		float		fSlideTransparency;
#ifdef _3D_CUBE_TRANS
		int			nDirecttion;
		int			nRotate;
#endif
		_Shape():
			nDiveX(1),
			nDiveY(1),
			fIntervalX(1),
			fIntervalY(1),
			crSlideColor(0xff000000),
			fSlideTransparency(1.f),
#ifdef _3D_CUBE_TRANS
			nDirecttion(0),
			nRotate(1)
#endif
		{}
	};

	_Transform trans;
	_Light light;
	_Shape shape;

	float fPerspective;
#ifdef _3D_CUBE_TRANS
	D3DCOLOR	crBackgroundColor;
	float		fBackgroundTransparency;
	float       fTransition;
	BOOL		bReverse;
#endif

	CubeFxParam():
		fPerspective(30.f),
		crBackgroundColor(0xff000000),
		fBackgroundTransparency(1.f),
		fTransition(0.f),
		bReverse(FALSE)
	{
	}
};

struct SonyPinPFxParam : public FxParamBase
{
	BOOL             bLink_Scale;
	float			 fScaleX;		//100, 0.01, 0.01, 70
	float			 fScaleY;		//as above
	float			 fPositionX;	//2.0000, -2.0000, 0.0001, 0
	float			 fPositionY;	//as above
	BOOL             bFilter;		//true
	float            fBorderWidth;	//(23):    TPPARAM_FLOAT_FMT     gFloatDefault0     = { 0, 1.f, 0, 0.0001f, 5, 4, 0.f};
	D3DCOLOR	     cBorderColor;	//(37):    TPPARAM_SINGLECOLOR_FMT  gColor235 = { 0, D3DCOLOR_RGBA(235, 235, 235, 255) };
	BOOL             bEnableShadow;	//false
	D3DCOLOR	     cShadowColor;	//(38):    TPPARAM_SINGLECOLOR_FMT  gColor16 = { 0, D3DCOLOR_RGBA(16, 16, 16, 255) };
	float            fShadowDropDistance;		// 2.f, 0.f, 0.0001f,5, 4, 0.15f
	float            fShadowDropLightAngle;		//720.f, -720.f, 0.01f,5, 2, 135.f
	float            fShadowDropTransparency;	//1.f, 0.f, 0.0001f,5, 4, 0.3f
	float            fShadowDropSoftness;		//(23):    TPPARAM_FLOAT_FMT     gFloatDefault0     = { 0, 1.f, 0, 0.0001f, 5, 4, 0.f};
	DWORD        	 dwTrailDecayType;
	INT			     iTrailDecayTime;			//30, 0, 1, 0
	DWORD	         dwBackGroundType;
	D3DCOLOR		 cBackGroundSepiaColor;		//0xff4b3e15  };	// a,r,g,b
	float            fBackGoundDensity;			//100.f, 0.f, 0.01f,5, 2, 100.f
	BOOL             bLinkCrop;
	float            fBoundsLeft;		//(24):    TPPARAM_FLOAT_FMT     gFloat2Default0     = { 0, 2.f, 0, 0.0001f, 5, 4, 0.f};
	float            fBoundsBottom;		//as above
	float            fBoundsRight;		//as above
	float            fBoundsTop;		//as above
	float            fTansparency;		////1.f, 0.f, 0.0001f,5, 4, 0.3f
	//key TODO: remove them or change them?
	float			 fKeyScaleX[2];
	float			 fKeyScaleY[2];
	float			 fKeyPositionX[2];
	float			 fKeyPositionY[2];
	int				 nKeyCount;
} ;

