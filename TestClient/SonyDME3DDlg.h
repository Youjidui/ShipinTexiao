#pragma once

#include "../FxParam.h"
// CSonyDME3DDlg �Ի���

class CSonyDME3DDlg : public CDialog
{
	DECLARE_DYNAMIC(CSonyDME3DDlg)

public:
	CSonyDME3DDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSonyDME3DDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_SONY_DME3D };
	void SetParam(SonyDME3DTransfromFxPrarm* pParam) {m_pParam = pParam;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	SonyDME3DTransfromFxPrarm* m_pParam;
	
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckBfliter();
};
