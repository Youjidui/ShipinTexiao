#pragma once

#include "../FxParam.h"
// CParamSony3DBrokenGlass �Ի���

class CParamSony3DBrokenGlassTrans : public CDialog
{
	DECLARE_DYNAMIC(CParamSony3DBrokenGlassTrans)

public:
	CParamSony3DBrokenGlassTrans(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamSony3DBrokenGlassTrans();

// �Ի�������
	enum { IDD = IDD_PARAM_SONY_3D_BROKEN_GLASS_TRANS };
	void SetParam(Sony3DBrokenGlassFxParam* pParam) {m_pParam = pParam;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	Sony3DBrokenGlassFxParam* m_pParam;
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedReverse();
	afx_msg void OnBnClickedEnabled();
};
