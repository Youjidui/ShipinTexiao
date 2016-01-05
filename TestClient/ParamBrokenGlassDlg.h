#pragma once

#include "../FxParam.h"

// CParamBrokenGlassDlg 对话框

class CParamBrokenGlassDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamBrokenGlassDlg)

public:
	CParamBrokenGlassDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamBrokenGlassDlg();

// 对话框数据
	enum { IDD = IDD_BROKEN_GLASS };
	void SetParam(BrokenGlassFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BrokenGlassFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
