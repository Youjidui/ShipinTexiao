#pragma once

#include "../FxParam.h"

// CParamCubeTransDlg �Ի���

class CParamCubeTransDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamCubeTransDlg)

public:
	CParamCubeTransDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamCubeTransDlg();

// �Ի�������
	enum { IDD = IDD_CUBE_TRANS };
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
