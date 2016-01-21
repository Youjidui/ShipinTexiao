#pragma once

#include "../FxParam.h"
#include "ParamBasicWipeDlg.h"

// CParamFanWipeDlg �Ի���

class CParamFanWipeDlg : public CParamBasicWipeDlg
{
	typedef CParamBasicWipeDlg _baseClass;

	DECLARE_DYNAMIC(CParamFanWipeDlg)

public:
	CParamFanWipeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamFanWipeDlg();

	// �Ի�������
	//enum { IDD = IDD_PARAM_BARM_WIPE };
	void SetParam(BasicWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
