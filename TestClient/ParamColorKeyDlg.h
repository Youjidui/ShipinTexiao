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
	enum { IDD = IDD_DIALOG3 };

	void SetParam(ColorKeyParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	ColorKeyParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
