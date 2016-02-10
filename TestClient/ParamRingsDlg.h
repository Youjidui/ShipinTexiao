#pragma once

#include "../FxParam.h"

// CParamRingsDlg 对话框

class CParamRingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamRingsDlg)

public:
	CParamRingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamRingsDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_RINGS };
	void SetParam(RingsFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	RingsFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
