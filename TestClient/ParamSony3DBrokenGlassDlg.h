#pragma once

#include "../FxParam.h"

// CParamSony3DBrokenGlassDlg �Ի���

class CParamSony3DBrokenGlassDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamSony3DBrokenGlassDlg)

public:
	CParamSony3DBrokenGlassDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamSony3DBrokenGlassDlg();

// �Ի�������
	enum { IDD = IDD_SONY3D_BROKEN_GLASS };
	void SetParam(Sony3DBrokenGlassFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	Sony3DBrokenGlassFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
	afx_msg void OnBnClickedCheckEnabled();
};
