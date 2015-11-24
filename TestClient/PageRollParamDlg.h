#pragma once

#include "../FxParam.h"

// CPageRollParamDlg 对话框

class CPageRollParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CPageRollParamDlg)

public:
	CPageRollParamDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageRollParamDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };
	void SetParam(PageRollFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	PageRollFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckUseForeGround();
};
