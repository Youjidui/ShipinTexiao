#pragma once

#include "../FxParam.h"

// CParamBlurDlg �Ի���

class CParamBlurDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamBlurDlg)

public:
	CParamBlurDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamBlurDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

	void SetParam(SonyBlurFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	SonyBlurFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditBlurX();
	afx_msg void OnEnChangeEditBlurY();
	virtual BOOL OnInitDialog();
};
