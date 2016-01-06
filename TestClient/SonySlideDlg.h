#pragma once

#include "../FxParam.h"

// CSonySlideDlg �Ի���

class CParamSonySlideDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamSonySlideDlg)

public:
	CParamSonySlideDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamSonySlideDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SONY_SLIDE };

	void SetParam(SonySlideFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	SonySlideFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDwtype();
	afx_msg void OnBnClickedCheckReverse();
};
