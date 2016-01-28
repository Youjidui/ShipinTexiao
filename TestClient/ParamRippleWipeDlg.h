#pragma once

#include "../FxParam.h"

// CParamRippleWipeDlg �Ի���

class CParamRippleWipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamRippleWipeDlg)

public:
	CParamRippleWipeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamRippleWipeDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_RIPPLE_WIPE };
	void SetParam(RippleWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	RippleWipeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
