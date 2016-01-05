#pragma once

#include "../FxParam.h"

// CParamDissolveDlg 对话框

class CParamDissolveDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamDissolveDlg)

public:
	CParamDissolveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamDissolveDlg();

// 对话框数据
	enum { IDD = IDD_DISSOLVE };
	void SetParam(DissolveFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DissolveFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
