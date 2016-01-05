#pragma once

#include "../FxParam.h"

// CParamBrokenGlassDlg �Ի���

class CParamBrokenGlassDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamBrokenGlassDlg)

public:
	CParamBrokenGlassDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamBrokenGlassDlg();

// �Ի�������
	enum { IDD = IDD_BROKEN_GLASS };
	void SetParam(BrokenGlassFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BrokenGlassFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
