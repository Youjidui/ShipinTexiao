#pragma once

#include "../FxParam.h"

// CParamSonyBarnSlideDlg �Ի���

class CParamSonyBarnSlideDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamSonyBarnSlideDlg)

public:
	CParamSonyBarnSlideDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamSonyBarnSlideDlg();

// �Ի�������
	enum { IDD = IDD_SONY_BARN_SLIDE };
	void SetParam(SonyBarnSlideFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	SonyBarnSlideFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckReverse();
};
