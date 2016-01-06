// SonySlideDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "SonySlideDlg.h"


// CSonySlideDlg 对话框

IMPLEMENT_DYNAMIC(CParamSonySlideDlg, CDialog)

CParamSonySlideDlg::CParamSonySlideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamSonySlideDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CParamSonySlideDlg::~CParamSonySlideDlg()
{
}

void CParamSonySlideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamSonySlideDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DWTYPE, &CParamSonySlideDlg::OnCbnSelchangeComboDwtype)
	ON_BN_CLICKED(IDC_CHECK_REVERSE, &CParamSonySlideDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()

BOOL CParamSonySlideDlg::OnInitDialog()
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

void CParamSonySlideDlg::OnCbnSelchangeComboDwtype()
{
	CComboBox* pCombolBox = (CComboBox*)GetDlgItem(IDC_COMBO_DWTYPE);
	int index = pCombolBox->GetCurSel();
	m_pParam->dwType = index;
}

void CParamSonySlideDlg::OnBnClickedCheckReverse()
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
