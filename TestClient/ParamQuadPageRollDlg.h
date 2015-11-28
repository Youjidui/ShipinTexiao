#pragma once

#include "../FxParam.h"

// CParamQuadPageRollDlg �Ի���

class CParamQuadPageRollDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamQuadPageRollDlg)

public:
	CParamQuadPageRollDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamQuadPageRollDlg();

// �Ի�������
	enum { IDD = IDD_QUAD_PAGE_ROLL };
	void SetParam(QuadPageRollFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	QuadPageRollFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckUseForeGround();
};
