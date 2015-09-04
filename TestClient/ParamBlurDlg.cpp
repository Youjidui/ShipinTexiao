// ParamBlurDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamBlurDlg.h"
#include <tchar.h>
#include "EffectName.h"
#include "CommonMessage.h"


// CParamBlurDlg 对话框

IMPLEMENT_DYNAMIC(CParamBlurDlg, CDialog)

CParamBlurDlg::CParamBlurDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamBlurDlg::IDD, pParent)
{

}

CParamBlurDlg::~CParamBlurDlg()
{
}

void CParamBlurDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamBlurDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_BLUR_X, &CParamBlurDlg::OnEnChangeEditBlurX)
	ON_EN_CHANGE(IDC_EDIT_BLUR_Y, &CParamBlurDlg::OnEnChangeEditBlurY)
END_MESSAGE_MAP()


// CParamBlurDlg 消息处理程序

void CParamBlurDlg::OnEnChangeEditBlurX()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	//GetDlgItem(IDC_EDIT_BLUR_X, );
	GetDlgItemText(IDC_EDIT_BLUR_X, str);
	_stscanf(str, _T("%f"), &m_pParam->blurX);
	//m_pParam->blurX = _ttoi(str);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_BLUR, (LPARAM)m_pParam);
}

//UM_EFFECT_PARAMETER_CHANGE
void CParamBlurDlg::OnEnChangeEditBlurY()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	CString str;
	GetDlgItemText(IDC_EDIT_BLUR_Y, str);
	_stscanf(str, _T("%f"), &m_pParam->blurY);
	//m_pParam->blurY = _ttoi(str);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_BLUR, (LPARAM)m_pParam);
}

BOOL CParamBlurDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	TCHAR szText[16];
	_stprintf(szText, _T("%f"), m_pParam->blurX);
	SetDlgItemText(IDC_EDIT_BLUR_X, szText);
	_stprintf(szText, _T("%f"), m_pParam->blurY);
	SetDlgItemText(IDC_EDIT_BLUR_Y, szText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
