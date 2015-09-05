#pragma once

#include "../FxParam.h"

// CParamAmoebaWipeDlg �Ի���

class CParamAmoebaWipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamAmoebaWipeDlg)

public:
	CParamAmoebaWipeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamAmoebaWipeDlg();

// �Ի�������
	enum { IDD = IDD_AMOEBA_WIPE };
	void SetParam(AmoebaWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	AmoebaWipeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
