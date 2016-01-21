#pragma once

#include "../FxParam.h"

// CParamPageRotationDlg �Ի���

class CParamPageRotationDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamPageRotationDlg)

public:
	CParamPageRotationDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamPageRotationDlg();

// �Ի�������
	enum { IDD = IDD_PAGE_ROTATION };
	void SetParam(PageRotationFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	PageRotationFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
