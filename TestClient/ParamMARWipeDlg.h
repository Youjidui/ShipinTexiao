#pragma once

#include "../FxParam.h"
#include "ParamBasicWipeDlg.h"

// CParamMARWipeDlg 对话框

class CParamMARWipeDlg : public CParamBasicWipeDlg
{
	typedef CParamBasicWipeDlg _baseClass;

	DECLARE_DYNAMIC(CParamMARWipeDlg)

public:
	CParamMARWipeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamMARWipeDlg();

	// 对话框数据
	//enum { IDD = IDD_PARAM_BARM_WIPE };
	void SetParam(BasicWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
