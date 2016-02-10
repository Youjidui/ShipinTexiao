#pragma once

#include "../FxParam.h"

// CParamRingsDlg �Ի���

class CParamRingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamRingsDlg)

public:
	CParamRingsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamRingsDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_RINGS };
	void SetParam(RingsFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	RingsFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
