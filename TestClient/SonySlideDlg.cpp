// SonySlideDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "SonySlideDlg.h"


// CSonySlideDlg 对话框

IMPLEMENT_DYNAMIC(CSonySlideDlg, CDialog)

CSonySlideDlg::CSonySlideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSonySlideDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CSonySlideDlg::~CSonySlideDlg()
{
}

void CSonySlideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSonySlideDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DWTYPE, &CSonySlideDlg::OnCbnSelchangeComboDwtype)
	ON_BN_CLICKED(IDC_CHECK_REVERSE, &CSonySlideDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()

BOOL CSonySlideDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* pCombolBox = (CComboBox*)GetDlgItem(IDC_COMBO_DWTYPE);
	pCombolBox->InsertString(0, _T("Right2Left"));
	pCombolBox->InsertString(1, _T("Left2Right"));
	pCombolBox->InsertString(2, _T("Bottom2Top"));
	pCombolBox->InsertString(3, _T("Top2Bottom"));
	pCombolBox->InsertString(4, _T("BottomRight"));
	pCombolBox->InsertString(5, _T("BottomLeft"));
	pCombolBox->InsertString(6, _T("TopLeft"));
	pCombolBox->InsertString(7, _T("TopRight"));

	return TRUE;

}

// CSonySlideDlg 消息处理程序

void CSonySlideDlg::OnCbnSelchangeComboDwtype()
{
	CComboBox* pCombolBox = (CComboBox*)GetDlgItem(IDC_COMBO_DWTYPE);
	int index = pCombolBox->GetCurSel();
	m_pParam->dwType = index;
}

void CSonySlideDlg::OnBnClickedCheckReverse()
{
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	int lStatus = pCheckBox->GetCheck();
	if (lStatus == 0)
	{
		m_pParam->bReverse = FALSE;
	}
	else 
	{
		m_pParam->bReverse = TRUE;
	}
}
