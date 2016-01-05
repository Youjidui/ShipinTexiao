#pragma once

#include "../FxParam.h"

// CParamDissolveDlg �Ի���

class CParamDissolveDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamDissolveDlg)

public:
	CParamDissolveDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamDissolveDlg();

// �Ի�������
	enum { IDD = IDD_DISSOLVE };
	void SetParam(DissolveFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DissolveFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
