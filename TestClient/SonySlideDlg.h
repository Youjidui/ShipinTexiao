#pragma once

#include "../FxParam.h"

// CSonySlideDlg 对话框

class CSonySlideDlg : public CDialog
{
	DECLARE_DYNAMIC(CSonySlideDlg)

public:
	CSonySlideDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSonySlideDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SONY_SLIDE };

	void SetParam(SonySlideFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	SonySlideFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDwtype();
	afx_msg void OnBnClickedCheckReverse();
};
