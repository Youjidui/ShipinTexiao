#pragma once

#include "../FxParam.h"

// CPageRollParamDlg �Ի���

class CParamPageRollDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamPageRollDlg)

public:
	CParamPageRollDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamPageRollDlg();

// �Ի�������
	enum { IDD = IDD_PAGE_ROLL };
	void SetParam(PageRollFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	PageRollFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckUseForeGround();
};
