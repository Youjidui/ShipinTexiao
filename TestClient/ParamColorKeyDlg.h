#pragma once

#include "../FxParam.h"


// CParamColorKeyDlg �Ի���

class CParamColorKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamColorKeyDlg)

public:
	CParamColorKeyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamColorKeyDlg();

// �Ի�������
	enum { IDD = IDD_COLOR_KEY };

	void SetParam(ColorKeyParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	ColorKeyParam* m_pParam;
	HCURSOR m_hOldCursor;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGetKeyColorFromScreen();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
