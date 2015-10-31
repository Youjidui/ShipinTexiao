#pragma once

#include "../FxParam.h"

// PushDlg 对话框

class CPushDlg : public CDialog
{
	DECLARE_DYNAMIC(CPushDlg)

public:
	CPushDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPushDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_PUSH };

	void SetParam(PushFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	PushFxParam* m_pParam;
		
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeComboDwtype();
	afx_msg void OnBnClickedCheckReverse();
};
