#pragma once

#include "../FxParam.h"

// CParamDuoPageRollDlg 对话框

class CParamDuoPageRollDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamDuoPageRollDlg)

public:
	CParamDuoPageRollDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamDuoPageRollDlg();

// 对话框数据
	enum { IDD = IDD_DUO_PAGE_ROLL };
	void SetParam(DuoPageRollFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DuoPageRollFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckUseForeGround();
};
