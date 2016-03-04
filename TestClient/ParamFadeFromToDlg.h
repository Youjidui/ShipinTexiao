#pragma once

#include "../FxParam.h"

// CParamFadeFromToDlg �Ի���

class CParamFadeFromToDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamFadeFromToDlg)

public:
	CParamFadeFromToDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamFadeFromToDlg();

// �Ի�������
	enum { IDD = IDD_FADE_FROM_TO };
	void SetParam(SonyFadeFromToFxParam* pParam) {m_pParam = pParam;}
	void SetEffectName(LPCTSTR pszEffectName) {m_pszEffectName = pszEffectName;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	SonyFadeFromToFxParam* m_pParam;
	LPCTSTR m_pszEffectName;
	void SetParamOnWindow();

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckFromOrTo();
public:
	afx_msg void OnEnChangeEditFadecolor();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
