#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ParamBlurDlg.h"
#include "ParamColorKeyDlg.h"
#include "ParamAmoebaWipeDlg.h"
#include "ParamBarmWipeDlg.h"
#include "ParamMatrixWipeDlg.h"
#include "PushDlg.h"
#include "ParamChromaKeyDlg.h"
#include "SonySlideDlg.h"
#include "PageRollParamDlg.h"
#include "ParamQuadPageRollDlg.h"
#include "ParamCubeDlg.h"
#include "SonyDME3DDlg.h"
#include "ParamPinpDlg.h"
#include "ParamSonyBarnSlideDlg.h"


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
	CParamColorKeyDlg m_colorKeyDlg;
	CParamAmoebaWipeDlg m_amoebaWipeDlg;
	CParamBarmWipeDlg m_barmWipeDlg;
	CParamMatrixWipeDlg m_matrixWipeDlg;
	CPushDlg m_pushDlg;
	CParamChromaKeyDlg m_chromaKeyDlg;
	CSonySlideDlg m_SonySlideDlg;
	CPageRollParamDlg m_PageRollDlg;
	CParamQuadPageRollDlg m_QuadPageRollDlg;
	CParamCubeDlg m_cubeTransDlg;
	CSonyDME3DDlg m_sonyDME3DDlg;
	CParamPinPDlg m_sonyPinpDlg;
	CParamSonyBarnSlideDlg m_barnSlideDlg;

	afx_msg void OnBnClickedParameters();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeEffects();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
