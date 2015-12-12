#pragma once

#include "../FxParam.h"

// CParamCubeDlg 对话框

class CParamCubeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamCubeDlg)

public:
	CParamCubeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamCubeDlg();

// 对话框数据
	enum { IDD = IDD_CUBE };

	void SetParam(CubeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CubeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckLight();
	afx_msg void OnBnClickedCheckbreverse();
};
