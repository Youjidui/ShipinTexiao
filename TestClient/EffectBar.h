#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ParamBlurDlg.h"
#include "ParamColorKeyDlg.h"
#include "ParamAmoebaWipeDlg.h"
#include "ParamBarmWipeDlg.h"
#include "ParamFanWipeDlg.h"
#include "ParamMARWipeDlg.h"
#include "ParamMatrixWipeDlg.h"
#include "ParamRevolvingWipeDlg.h"
#include "ParamRippleWipeDlg.h"
#include "PushDlg.h"
#include "ParamChromaKeyDlg.h"
#include "SonySlideDlg.h"
#include "PageRollParamDlg.h"
#include "ParamDuoPageRollDlg.h"
#include "ParamQuadPageRollDlg.h"
#include "ParamCubeTransDlg.h"
#include "SonyDME3DDlg.h"
#include "ParamPinpDlg.h"
#include "ParamSonyBarnSlideDlg.h"
#include "ParamBrokenGlassDlg.h"
#include "ParamRingsDlg.h"
#include "ParamDissolveDlg.h"
#include "ParamFadeFromToDlg.h"
#include "ParamPageRotationDlg.h"
#include "ParamSony3DBrokenGlassTrans.h"
#include "ParamDipToColorDlg.h"

// CEffectBar 对话框

class CEffectBar : public CDialog
{
	DECLARE_DYNAMIC(CEffectBar)

public:
	CEffectBar(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEffectBar();

// 对话框数据
	enum { IDD = IDD_DIALOGBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void SetProgress(CSliderCtrl* pCtrl);
	void OnProgressChange(int nPos);

	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_ctrlEffects;
	CSliderCtrl m_ctrlProgress;
	CParamBlurDlg m_blurDlg;
	//CParamColorKeyDlg m_colorKeyDlg;
	CParamAmoebaWipeDlg m_amoebaWipeDlg;
	CParamBarmWipeDlg m_barmWipeDlg;
	CParamFanWipeDlg m_fanWipeDlg;
	CParamMARWipeDlg m_marWipeDlg;
	CParamMatrixWipeDlg m_matrixWipeDlg;
	CParamRevolvingWipeDlg m_revolvingWipeDlg;
	CParamRippleWipeDlg m_rippleWipeDlg;
	CParamPushDlg m_pushDlg;
	CParamChromaKeyDlg m_chromaKeyDlg;
	CParamSonySlideDlg m_SonySlideDlg;
	CParamPageRollDlg m_PageRollDlg;
	CParamDuoPageRollDlg m_duoPageRollDlg;
	CParamQuadPageRollDlg m_QuadPageRollDlg;
	CParamCubeTransDlg m_cubeTransDlg;
	CSonyDME3DDlg m_sonyDME3DDlg;
	CParamPinPDlg m_sonyPinpDlg;
	CParamBrokenGlassDlg m_brokenGlassDlg;
	CParamRingsDlg m_ringsDlg;
	CParamSonyBarnSlideDlg m_barnSlideDlg;
	CParamDissolveDlg m_dissolveDlg;
	CParamFadeFromToDlg m_fadeFromToDlg;
	CParamPageRotationDlg m_pageRotationDlg;
	CParamSony3DBrokenGlassTrans m_sony3DBrokenGlassDlg;
	CParamDipToColorDlg m_dipToColorDlg;

	afx_msg void OnBnClickedParameters();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeEffects();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
