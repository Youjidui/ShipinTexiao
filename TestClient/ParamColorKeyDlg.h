#pragma once

#include "../FxParam.h"


// CParamColorKeyDlg 对话框

class CParamColorKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamColorKeyDlg)

public:
	CParamColorKeyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamColorKeyDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

	void SetParam(ColorKeyParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	ColorKeyParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
