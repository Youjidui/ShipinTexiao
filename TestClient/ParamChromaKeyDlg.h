#pragma once

#include "../FxParam.h"

// CParamChromaKeyDlg �Ի���

class CParamChromaKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamChromaKeyDlg)

public:
	CParamChromaKeyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamChromaKeyDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_CHROMA_KEY };

	void SetParam(ChromaKeyFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	ChromaKeyFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckCcOn();
	afx_msg void OnBnClickedCheckCkInvert();
	afx_msg void OnBnClickedCheckColorKey();
	afx_msg void OnBnClickedCheckYbOn();
};
