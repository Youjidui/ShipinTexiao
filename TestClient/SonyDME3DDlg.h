#pragma once

#include "../FxParam.h"
// CSonyDME3DDlg 对话框

class CSonyDME3DDlg : public CDialog
{
	DECLARE_DYNAMIC(CSonyDME3DDlg)

public:
	CSonyDME3DDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSonyDME3DDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_SONY_DME3D };
	void SetParam(SonyDME3DTransfromFxPrarm* pParam) {m_pParam = pParam;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	SonyDME3DTransfromFxPrarm* m_pParam;
	
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckBfliter();
};
