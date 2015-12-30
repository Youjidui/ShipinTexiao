#pragma once

#include "../FxParam.h"

// CParamPinPDlg �Ի���

class CParamPinPDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamPinPDlg)

public:
	CParamPinPDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamPinPDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_PINP };

	void SetParam(SonyPinPFxParam* pParam) {m_pParam = pParam;}

protected:
	SonyPinPFxParam* m_pParam;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckbenableshadow();
	afx_msg void OnBnClickedCheckbfilter();
	afx_msg void OnEnChangeEditcbackgroundsepiacolor();
	afx_msg void OnEnChangeEditcbordercolor();
	afx_msg void OnEnChangeEditcshadowcolor();
};
