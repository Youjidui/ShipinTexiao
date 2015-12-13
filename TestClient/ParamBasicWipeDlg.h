#pragma once

#include "../FxParam.h"

class CParamBasicWipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamBasicWipeDlg)

public:
	CParamBasicWipeDlg(LPCTSTR pEffectName, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamBasicWipeDlg();

	// 对话框数据
	enum { IDD = IDD_PARAM_BASIC_WIPE };

protected:
	void SetParam(BasicWipeFxParam* pParam) {m_pParam = pParam;}
	//void SetEffectName(LPCTSTR pEffectName){m_EffectName = pEffectName;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BasicWipeFxParam* m_pParam;
	LPCTSTR m_EffectName;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
