#pragma once

#include "../FxParam.h"

// CPageRollParamDlg 对话框

class CParamPageRollDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamPageRollDlg)

public:
	CParamPageRollDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamPageRollDlg();

// 对话框数据
	enum { IDD = IDD_PAGE_ROLL };
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
