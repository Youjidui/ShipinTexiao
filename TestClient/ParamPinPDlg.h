#pragma once

#include "../FxParam.h"

// CParamPinPDlg 对话框

class CParamPinPDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamPinPDlg)

public:
	CParamPinPDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamPinPDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_PINP };

	void SetParam(SonyPinPFxParam* pParam) {m_pParam = pParam;}

protected:
	SonyPinPFxParam* m_pParam;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckbenableshadow();
	afx_msg void OnBnClickedCheckbfilter();
	afx_msg void OnEnChangeEditcbackgroundsepiacolor();
	afx_msg void OnEnChangeEditcbordercolor();
	afx_msg void OnEnChangeEditcshadowcolor();
};
