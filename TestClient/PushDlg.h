#pragma once


// PushDlg �Ի���

class CPushDlg : public CDialog
{
	DECLARE_DYNAMIC(CPushDlg)

public:
	CPushDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPushDlg();

// �Ի�������
	enum { IDD = IDD_PARAM_PUSH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
