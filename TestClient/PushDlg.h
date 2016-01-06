#pragma once

#include "../FxParam.h"

// PushDlg �Ի���

class CParamPushDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamPushDlg)

public:
	CParamPushDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamPushDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_PUSH };

	void SetParam(PushFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	PushFxParam* m_pParam;
		
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeComboDwtype();
	afx_msg void OnBnClickedCheckReverse();
};
