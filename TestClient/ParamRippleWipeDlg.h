#pragma once

#include "../FxParam.h"

// CParamRippleWipeDlg 对话框

class CParamRippleWipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamRippleWipeDlg)

public:
	CParamRippleWipeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamRippleWipeDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_RIPPLE_WIPE };
	void SetParam(RippleWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	RippleWipeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
