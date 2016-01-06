#pragma once

#include "../FxParam.h"

// CParamDuoPageRollDlg �Ի���

class CParamDuoPageRollDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamDuoPageRollDlg)

public:
	CParamDuoPageRollDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamDuoPageRollDlg();

// �Ի�������
	enum { IDD = IDD_DUO_PAGE_ROLL };
	void SetParam(DuoPageRollFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DuoPageRollFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckUseForeGround();
};
