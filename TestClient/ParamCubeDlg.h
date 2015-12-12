#pragma once

#include "../FxParam.h"

// CParamCubeDlg �Ի���

class CParamCubeDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamCubeDlg)

public:
	CParamCubeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamCubeDlg();

// �Ի�������
	enum { IDD = IDD_CUBE };

	void SetParam(CubeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CubeFxParam* m_pParam;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckLight();
	afx_msg void OnBnClickedCheckbreverse();
};
