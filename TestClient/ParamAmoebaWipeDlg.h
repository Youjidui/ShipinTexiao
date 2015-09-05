#pragma once

#include "../FxParam.h"

// CParamAmoebaWipeDlg 对话框

class CParamAmoebaWipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamAmoebaWipeDlg)

public:
	CParamAmoebaWipeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamAmoebaWipeDlg();

// 对话框数据
	enum { IDD = IDD_AMOEBA_WIPE };
	void SetParam(AmoebaWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	AmoebaWipeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
