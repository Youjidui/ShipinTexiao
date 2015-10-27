#pragma once


// PushDlg 对话框

class CPushDlg : public CDialog
{
	DECLARE_DYNAMIC(CPushDlg)

public:
	CPushDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPushDlg();

// 对话框数据
	enum { IDD = IDD_PARAM_PUSH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
