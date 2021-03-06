#pragma once

#include "VideoBuffer.h"
#include <d3dx9math.h>


//common basic parameter
struct TransformParam{
	float fLocalTranslateX;		//-10.000 - 10.000
	float fLocalTranslateY;		//-10.000 - 10.000
	float fLocalTranslateZ;		//-5.00 - 300.00

	float fLocalRotateX;		//-5.0000 - 5.0000
	float fLocalRotateY;		//-5.0000 - 5.0000
	float fLocalRotateZ;		//-5.0000 - 5.0000

	float fScaleX;				//0 - 10.000
	float fScaleY;				//0 - 10.000
	float fScaleZ;				//0 - 10.000

	float fWorldTranslateX;		//-10.000 - 10.000
	float fWorldTranslateY;		//-10.000 - 10.000
	float fWorldTranslateZ;		//-5.00 - 300.00

	float fWorldRotateX;		//-5.0000 - 5.0000
	float fWorldRotateY;		//-5.0000 - 5.0000
	float fWorldRotateZ;		//-5.0000 - 5.0000

	void identity()
	{
		fLocalRotateX = fLocalRotateY = fLocalRotateZ = 0.f;
		fLocalTranslateX = fLocalTranslateY = fLocalTranslateZ = 0.f;
		fScaleX = fScaleY = fScaleZ = 1.f;
		fWorldTranslateX = fWorldTranslateY = fWorldTranslateZ = 0.f;
		fWorldRotateX = fWorldRotateY = fWorldRotateZ = 0.f;
	}
	TransformParam() { identity(); }
};

struct LightingParam
{
	BOOL bEnabled;
	float fDirectionX;		//90.f,-90.f, 0.001f,5,3,0.f
	float fDirectionY;		//90.f,-90.f, 0.001f,5,3,0.f
	float fDiffuse;			//2.f, 0, 0.0001f, 5, 4, 0.f
	float fAmbient;			//1.f, 0, 0.0001f, 5, 4, 1.f
	LightingParam():
	bEnabled(FALSE),
		fDirectionX(0.f),
		fDirectionY(0.f),
		fDiffuse(0.f),
		fAmbient(1.f)
	{	}
};

struct TransitionParam
{
	float progress;				//1.f, 0, 0.0001f, 5, 4, 0.5f
	BOOL bReverse;

	TransitionParam() : progress(0), bReverse(FALSE) {}
};

//FX parameter

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
	float fKeyColor[4];	//b,g,r,a
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
		int nPattern;				//different wipe has different value range
		BOOL bInvert;
		float fOffset;				//1.f, 0, 0.0001f
		float fRotate;
		float fCenterX;				//2.0f, -2.0f, 0.0001f
		float fCenterY;				//2.0f, -2.0f, 0.0001f
		float fAspect;
		float fSoftWidth;			//5.0f, 0.0f,  0.0001f, 5, 4, 0.0f
		float fBorderWidth;			//5.0f, 0.0f,  0.0001f, 5, 4, 0.0f
		D3DCOLOR crBorderColor;

		Pattern()
			: nPattern(0), bInvert(FALSE), fOffset(0.f), fRotate(0.f), fCenterX(0.f), fCenterY(0.f),
			fAspect(1920.f/1080.f), fSoftWidth(0.f), fBorderWidth(0.f), crBorderColor(0xff000000)
		{		}
	};
	struct Modify
	{
		int		nMultipleType;		//3, 0, 1	??
		BOOL	bFlip;
		int		nMultipleNumberX;	//20, 1, 1
		int		nMultipleNumberY;	//20, 1, 1
		BOOL	bOverlap;
		int		nDivideType;		//5, 1,  1
		float   fDivideWidth;		//1.f, 0, 0.0001f

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
		float fTransparency;		//1.0f, 0.0f,  0.0001f
		General() : fTransparency(1.0f)
		{}
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
		//bool bOn;
		float fClip;		//0, 100.00
		float fGain;		//-100.00, 100.00
		float fHue;			//0, 360.00
		float fAngle;		//0, 180.00
		//int nFilter;		//0, 9
		float fSatCrop;		//0, 100.00
		float fDensity;		//0, 100.00
		bool bInvert;
		ChromaKeyAdjust()
			//: bOn(true)
			: fClip(100.f), fGain(100.f), fHue(0.f), fAngle(97.59578851f)//, nFilter(1)
			, fSatCrop(20.3125f), fDensity(100.f), bInvert(false) {}
	}paramCrkAdj;

	struct Region
	{
		float fLeft;		//0, 1.0000
		float fTop;			//0, 1.0000
		float fRight;		//0, 1.0000
		float fBottom;		//0, 1.0000
		Region() : fLeft(0.f), fTop(0.f), fRight(1.f), fBottom(1.f) {}
	}paramRgn;

	//struct ChromaKeyPosition
	//{
	//	float fLeft;
	//	float fRight;
	//	ChromaKeyPosition() : fLeft(0.f), fRight(0.f) {}
	//}paramCrkPos;

	struct YBalance
	{
		bool bOn;
		float fClip;		//0, 100.00
		float fGain;		//-100.00, 100.00
		float fLum;			//0, 100.00
		YBalance() : bOn(false), fClip(0.f), fGain(0.f), fLum(0.f) {}
	}paramYBal;

	struct ColorCancel
	{
		bool bOn;
		float fLum;			//0, 100.00
		float fSat;			//0, 100.00
		float fHue;			//0, 360.00
		//int nFilter;		//0, 9
		ColorCancel(): bOn(false), fLum(0.f), fSat(0.f), fHue(0.f)/*, nFilter(1)*/ {}
	}paramCc;

	struct CancelKey
	{
		float fClip;
		float fGain;
		CancelKey() : fClip(0.f), fGain(0.f) {}
	}paramCcKey;

	//struct CancelKeyPosition
	//{
	//	float fLeft;
	//	float fRight;
	//	CancelKeyPosition() : fLeft(0.f), fRight(0.f) {}
	//}paramCckPos;

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

struct PageRotationFxParam : public FxParamBase
{
	int	nPattern;		//0,1

	struct Transition
	{
		float fTransition;		//progress 0 - 10000
		BOOL bReverse;

		Transition() : fTransition(0.f), bReverse(false) {}
	};
	Transition structTrans;

	PageRotationFxParam() : nPattern(0) {}
} ;

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
		float fTransition;		//progress 0 - 10000
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


struct DuoPageRollFxParam : public PageRollFxParam
{	
	float	fAngle[2];		//720.f, 360, 0.01f
	int		nSplitPattern;	//0 - 1
	int		nStepPattern;	//0 - 2

	DuoPageRollFxParam()
	{
		fAngle[0] = 45.f;
		fAngle[1] = 315.f;

		nSplitPattern = 0;
		nStepPattern = 0;
	}
} ;

struct QuadPageRollFxParam : public PageRollFxParam
{
	float	fAngle[4];		//720.f, 360, 0.01f
	int		nStepPattern;	//0 - 2
	int		nGroupPattern;	//0 - 2

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

		_Local() : fLocationX(0.f), fLocationY(0.f), fLocationZ(0.f)
		, fSpinX(0.f), fSpinY(0.f), fSpinZ(0.f)
		, fRotationX(0.f), fRotationY(0.f), fRotationZ(0.f)
		, fSrcSpinX(0.f), fSrcSpinY(0.f), fSrcSpinZ(0.f)
		, fAxisX(0.f), fAxisY(0.f), fAxisZ(0.f)
		{}
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

		_Global() : fLocationX(0.f), fLocationY(0.f), fLocationZ(0.f)
			, fSpinX(0.f), fSpinY(0.f), fSpinZ(0.f)
			, fRotationX(0.f), fRotationY(0.f), fRotationZ(0.f)
			, fSrcSpinX(0.f), fSrcSpinY(0.f), fSrcSpinZ(0.f)
			, fAxisX(0.f), fAxisY(0.f), fAxisZ(0.f)
		{}
	}sGlobal;

	bool  bFilter;
	float fPerspective;

	float fPostSizeX;
	float fPostSizeY;
	float fPostLocationX;
	float fPostLocationY;

	SonyDME3DTransfromFxPrarm()
		: fPreSizeX(1.0f), fPreSizeY(1.0f), fSkewX(0.f), fSkewY(0.f), bFilter(TRUE), fPerspective(30.f)
		, fPostSizeX(1.f), fPostSizeY(1.f), fPostLocationX(0.f), fPostLocationY(0.f)
	{	}
};

struct RingsFxParam : public FxParamBase
{
	//progress
	float		fTranslate;		//1.f, 0.f, 0.0001f, 5, 4, 0.f
	bool  bReverse;
	float		fRandomTranslate;	////1.f, 0.f, 0.01f, 3, 2, 0.f	 the same as BrokenGlass
	float		fRotate;		//720.f, -720.f, 0.01f, 5, 2, 0.f  //3.1415926f, -3.1415926f, 0.01f, 3, 2, 0.f  the same as BrokenGlass
	float		fCenterX;		//2.f, -2.f, 0.01f, 3, 2, 0.f	 the same as BrokenGlass
	float		fCenterY;		//2.f, -2.f, 0.01f, 3, 2, 0.f	 the same as BrokenGlass
	float		fAspect;		//1.f, -1.f, 0.01f, 3, 2, 0.f	 the same as BrokenGlass
	float		fWidth;			//1.f, 0.f, 0.0001f, 3, 2, 0.2f
	float		fRandomWidth;	//1.f, -1.f, 0.01f, 3, 2, 0.f
	float		fSpiral;		//180.f, -180.f, 0.01f, 5, 2, 80.f	//10, 0, 1,  3
	float		fRandomPixel;	//1.f, 0.f, 0.01f, 3, 2, 0.f	 the same as BrokenGlass	
	int			nPattern;		//0-8
	int			nEffectNo;

	RingsFxParam() : fTranslate(0.f), bReverse(false), fRandomTranslate(0.f), fRotate(0.f)
		, fCenterX(0.f), fCenterY(0.f), fAspect(0.f), fWidth(0.2f), fRandomWidth(0.f)
		, fSpiral(80.f), fRandomPixel(0.f), nPattern(0), nEffectNo(0)
	{
		cbSize = sizeof(RingsFxParam);
		strcpy_s(FxType, sizeof(FxType), "Rings");
	}
};

#define _3D_CUBE_TRANS

struct CubeFxParam : public FxParamBase
{
	struct ShapeParam
	{
		int			nDiveX;			//1 - 20
		int			nDiveY;			//1 - 20
		float		fIntervalX;		//1.000 - 10.000
		float		fIntervalY;		//1.000 - 10.000
		D3DCOLOR	crSlideColor;
		float		fSlideTransparency;	//0 - 1.0000
#ifdef _3D_CUBE_TRANS
		int			nDirecttion;	//0 - 3		//�ϣ��£���,��
		int			nRotate;		//1 - 2		//CW ˳ʱ��, CCW ��ʱ��
#endif
		ShapeParam():
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

#ifndef _3D_CUBE_TRANS		//not defined
	TransformParam trans;
#endif
	LightingParam light;
	ShapeParam shape;

	float fPerspective;				//10.00 - 120.00
#ifdef _3D_CUBE_TRANS
	D3DCOLOR	crBackgroundColor;
	float		fBackgroundTransparency;	//0.0000 - 1.0000
	float       fTransition;		//0.0000 - 1.0000
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
	float            fBoundsLeft;		//(24):    TPPARAM_FLOAT_FMT     gFloat2Default0     = { 0, 2.f, 0, 0.0001f, 5, 4, 0.f};
	float            fBoundsTop;		//as above
	float            fBoundsRight;		//as above
	float            fBoundsBottom;		//as above
	float            fTansparency;		////1.f, 0.f, 0.0001f,5, 4, 0.3f
	//key TODO: remove them or change them?
	float			 fKeyScaleX[2];
	float			 fKeyScaleY[2];
	float			 fKeyPositionX[2];
	float			 fKeyPositionY[2];
	int				 nKeyCount;

	SonyPinPFxParam() : fScaleX(70), fScaleY(70), fPositionX(0), fPositionY(0), bFilter(true),
		fBorderWidth(0), cBorderColor(D3DCOLOR_RGBA(235, 235, 235, 255)), bEnableShadow(false),
		cShadowColor(D3DCOLOR_RGBA(16, 16, 16, 255)), fShadowDropDistance(0.15f), fShadowDropLightAngle(135.f),
		fShadowDropTransparency(0.3f), fShadowDropSoftness(0), dwTrailDecayType(0), iTrailDecayTime(0),
		dwBackGroundType(0), cBackGroundSepiaColor(0xff4b3e15), fBackGoundDensity(100.f), fBoundsLeft(0.f),
		fBoundsTop(0.f), fBoundsRight(0.f), fBoundsBottom(0.f)
	{
		fKeyScaleX[0] = fKeyScaleX[1] = fKeyScaleY[0] = fKeyScaleY[1] = 70.f;
		fKeyPositionX[0] = fKeyPositionX[1] = fKeyPositionY[0] = fKeyPositionY[1] = 0.f;
		nKeyCount = 0;
	}
};

struct SonyFilterFxParam : public FxParamBase
{
	float fSacleX;
	float fSacleY;
	int nTaps;
	RECT rcBound;

	enum POS_Def
	{ 
		POS_NOCHANGE = 0,
		//POS_STRETCH_TO_FILL,
		//POS_STRETCH_FITX,
		//POS_STRETCH_FITY,
		//POS_CENTER_NOCHANGE,
		POS_CUSTOM,
		//POS_SONY_STRETCH,
		//POS_NO_CONVERT,
		//POS_FRAME_SIZE_CONVERT,
		POS_SONY_PINP
	}emPosDefMode;
	float OffsetX;
	float OffsetY;
	float vector4[4];
	//D3DXVECTOR4 vector4;
};

struct SonyBarnSlideFxParam : public FxParamBase
{
	int nType;		//"0=Horz\n1=Vert\n2=Cross"
	float fSlant;	//45.f, -45.f, 0.01f, 5, 3, 0.f
	float fTrans;	//progress 0 - 10000
	bool  bReverse;

	SonyBarnSlideFxParam() : nType(0), fSlant(0.f), fTrans(0.f), bReverse(false) {}
};

struct BrokenGlassFxParam : public FxParamBase
{
	//float prm_movement;				//1.f, -1.f, 0.01f, 3, 2, 0.f
	//Progress
	float prm_movement;				//1.f, 0.f, 0.0001f, 5, 4, 0.f
	bool  prm_bReverse;
	float prm_movementRandom;		//1.f, 0.f, 0.01f, 3, 2, 0.f		 the same as Rings

	float prm_rotate;				//720.f, -720.f, 0.01f, 5, 2, 0.f	the same as Rings

	float prm_centerX;				//2.f, -2.f, 0.01f, 3, 2, 0.f		 the same as Rings
	float prm_centerY;				//2.f, -2.f, 0.01f, 3, 2, 0.f		 the same as Rings
	float prm_aspect;				//1.f, -1.f, 0.01f, 3, 2, 0.f		 the same as Rings

	float prm_width;				//1.f, 0.f, 0.01f, 3, 2, 0.2f		the same as Rings
	float prm_widthRandom;			//1.f, -1.f, 0.01f, 3, 2, 0.f		 the same as Rings


	float prm_angleBlock;			//180.00, -180.00 (80)

	float prm_randomPix;			//1.f, 0.f, 0.01f, 3, 2, 0.f		 the same as Rings


	BrokenGlassFxParam() : prm_movement(0.f), prm_bReverse(false), prm_movementRandom(0.f), prm_rotate(0.f)
		, prm_centerX(0.f), prm_centerY(0.f), prm_aspect(0.f), prm_width(0.2f)
		, prm_widthRandom(0.f), prm_angleBlock(80.0f), prm_randomPix(0.f)
	{	}
} ;

struct DissolveFxParam : public FxParamBase
{
	float prm_process;		//1.f, 0.f, 0.0001f, 5, 4, 0.f	progress
	int	  prm_mixType;		//0 - 4
	float prm_subtractr;	//1.0f, 0.0f, 0.0001f, 5, 4, 0.5f
	bool  prm_bReverse;

	DissolveFxParam() : prm_process(0.f), prm_mixType(0), prm_subtractr(0.5f), prm_bReverse(false) {}
};

struct SonyFadeFromToFxParam :public FxParamBase
{
	D3DCOLOR cFadeColor;
	float fTransition;		//1.f, 0.f, 0.0001f, 5, 4, 0.f	progress
	//bool bFromOrTo;			//true = from, false = to
	bool  bReverse;

	//SonyFadeFromToFxParam() : cFadeColor(0xffffffff), fTransition(0.f), bFromOrTo(true), bReverse(false) {}
	SonyFadeFromToFxParam() : cFadeColor(0xffffffff), fTransition(0.f), bReverse(false) {}
} ;

struct DiffuseWipeFxParam : public FxParamBase
{
	float		fWidth;
	float		fAngle;
	float		fTranslate;
	
	int nBlockWidth;
	int nBlockHeight;
};

// begin Sony3DBrokenGlassEffect Param


struct Sony3DBrokenGlassFxParam : public FxParamBase
{

	struct PerspectiveParam
	{
		float fovDegree;		//120.f, 10.f, 0.001f, 5, 3, 30.f

		PerspectiveParam() : fovDegree(30.f) {}
	};


	float progress;				//1.f, 0, 0.0001f, 5, 4, 0.5f
	BOOL bReverse;

	int divideX, divideY;		//40, 1, 1, 10
	int fallingDirection;		// 0 bottom, 1 top, 2 inside, 3 outside
	int rotateType;

	float centerX, centerY;		//1.f, 0, 0.0001f, 5, 4, 0.5f ??

	//TransformParam	transformPackParam;
	LightingParam	lightingPackParam;
	PerspectiveParam	perspPackParam;

	Sony3DBrokenGlassFxParam() : progress(0.5f), bReverse(false), divideX(10), divideY(10), fallingDirection(0), rotateType(0), centerX(0.5f), centerY(0.5f) {}
};


// end Sony3DBrokenGlassEffect Param


struct RippleWipeFxParam : public FxParamBase
{
	float fRipple_Width;		//1.0f, 0.10f,  0.0001f, 5, 4, 0.3f
	float fNum_Wave;			//20.0f, 0.50f, 0.001f, 5, 3, 5.0f
	float fAmplitude;			//1.0f, 0.0f, 0.0001f, 5, 4, 0.5f
	float fEllipticity;			//1.0f, -1.0f, 0.0001f, 5, 4, 0.18f
	float fCenterX;				//3.0f, -3.0f, 0.0001f, 5, 4, 0.0f
	float fCenterY;				//3.0f, -3.0f, 0.0001f, 5, 4, -0.5f
	float fDistortion;			//1.0f, 0.0f, 0.0001f, 5, 4, 0.1380f
	float fBrightness;			//1.0f, 0.0f,  0.0001f, 5, 4, 0.5f
	float fLight_Angle;			//720.0f, -360.0f, 0.01f, 5, 2, 135.0f
	//float fOffset;
	//bool bRevert;
	TransitionParam transParam;

	RippleWipeFxParam() : fRipple_Width(0.3f), fNum_Wave(5.0f), fAmplitude(0.5f), fEllipticity(0.18f), fCenterX(0.0f), fCenterY(-0.5f), fDistortion(0.138f), fBrightness(0.5f), fLight_Angle(135.f) {}
};

//EffectRenderAll.h

struct TPERParamSmoothNoise
{
	float fAmplitude;	// [-1,1]
	float fFrequency;	// [0,1]
	float fPhase;		// [-infinite,infinite]
	UINT  uInterpolationType;	// 0 Near; 1 Linear; 2 Lagrange
};

struct TPERParamMelt
{
	float fAmplitude;	// [0,1]
	float fPosition;	// height[-0.5f,0.5f]
	float fRotateRadian;
	float fSlantRadian;	// corresponds to degree[-30,30]
	float fRandomPix;	// [0,1]
	float fDensity;		// [0,1]
	float fDensityPos;	// [0,1]
	bool  bSecondeFiled;
	TPERParamSmoothNoise paramSmoothNoise;
};

