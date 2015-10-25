#pragma once

#include "../FxParam.h"

// CParamBarmWipeDlg 对话框

class CParamBarmWipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamBarmWipeDlg)

public:
	CParamBarmWipeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamBarmWipeDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_BARM_WIPE };
	void SetParam(BarmWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BarmWipeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
