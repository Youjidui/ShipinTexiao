#pragma once

#include "../FxParam.h"

// CParamBlurDlg 对话框

class CParamBlurDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamBlurDlg)

public:
	CParamBlurDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamBlurDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

	void SetParam(SonyBlurFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	SonyBlurFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditBlurX();
	afx_msg void OnEnChangeEditBlurY();
	virtual BOOL OnInitDialog();
};
