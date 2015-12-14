#pragma once

#include "../FxParam.h"

// CParamQuadPageRollDlg 对话框

class CParamQuadPageRollDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamQuadPageRollDlg)

public:
	CParamQuadPageRollDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamQuadPageRollDlg();

// 对话框数据
	enum { IDD = IDD_QUAD_PAGE_ROLL };
	void SetParam(QuadPageRollFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	QuadPageRollFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckUseForeGround();
};
