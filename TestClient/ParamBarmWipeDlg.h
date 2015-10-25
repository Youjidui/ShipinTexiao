#pragma once

#include "../FxParam.h"

// CParamBarmWipeDlg �Ի���

class CParamBarmWipeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamBarmWipeDlg)

public:
	CParamBarmWipeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamBarmWipeDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_BARM_WIPE };
	void SetParam(BarmWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BarmWipeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
