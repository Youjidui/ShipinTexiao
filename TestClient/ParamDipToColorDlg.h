#pragma once

#include "../EffDipToColor/DipToColorRender.h"

// CParamDipToColorDlg 对话框

class CParamDipToColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamDipToColorDlg)

public:
	CParamDipToColorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamDipToColorDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_DIP_TO_COLOR };

	void SetParam(DipToColorTransFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DipToColorTransFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedCheckbreverse();
};
