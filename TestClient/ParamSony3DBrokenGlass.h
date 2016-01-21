#pragma once

#include "../FxParam.h"
// CParamSony3DBrokenGlass �Ի���

class CParamSony3DBrokenGlass : public CDialog
{
	DECLARE_DYNAMIC(CParamSony3DBrokenGlass)

public:
	CParamSony3DBrokenGlass(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamSony3DBrokenGlass();

// �Ի�������
	enum { IDD = IDD_PARAM_SONY_3D_BROKEN_GLASS };
	void SetParam(Sony3DBrokenGlassEffectParam* pParam) {m_pParam = pParam;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	Sony3DBrokenGlassEffectParam* m_pParam;
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedReverse();
	afx_msg void OnBnClickedEnabled();
};
