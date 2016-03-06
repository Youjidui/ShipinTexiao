#pragma once

#include "../FxParam.h"

// CParamCubeTransDlg 对话框

class CParamCubeTransDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamCubeTransDlg)

public:
	CParamCubeTransDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamCubeTransDlg();

// 对话框数据
	enum { IDD = IDD_CUBE_TRANS };
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
