#pragma once

#include "../FxParam.h"
#include "ParamBasicWipeDlg.h"

class CParamRevolvingWipeDlg : public CParamBasicWipeDlg
{
	typedef CParamBasicWipeDlg _baseClass;

	DECLARE_DYNAMIC(CParamRevolvingWipeDlg)

public:
	CParamRevolvingWipeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamRevolvingWipeDlg();

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
