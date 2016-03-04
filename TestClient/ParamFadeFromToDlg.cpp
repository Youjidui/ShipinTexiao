// ParamFadeFromToDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamFadeFromToDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)
#pragma warning(disable:4996)

// CParamFadeFromToDlg 对话框

IMPLEMENT_DYNAMIC(CParamFadeFromToDlg, CDialog)

CParamFadeFromToDlg::CParamFadeFromToDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamFadeFromToDlg::IDD, pParent)
	, m_pParam(NULL)
	, m_pszEffectName(NULL)
{

}

CParamFadeFromToDlg::~CParamFadeFromToDlg()
{
}

void CParamFadeFromToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamFadeFromToDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamFadeFromToDlg::OnBnClickedCheckReverse)
	ON_BN_CLICKED(IDC_CHECK_FromOrTo, &CParamFadeFromToDlg::OnBnClickedCheckFromOrTo)
	ON_EN_CHANGE(IDC_EDIT_FadeColor, &CParamFadeFromToDlg::OnEnChangeEditFadecolor)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CParamFadeFromToDlg 消息处理程序
BOOL CParamFadeFromToDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//SetParamOnWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamFadeFromToDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	ASSERT(m_pParam);
	unsigned char vColor[4];
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_fTransition:
		m_pParam->fTransition = nPos / (10000.f);
		break;
	case IDC_SLIDER_A:
		memcpy(vColor, &m_pParam->cFadeColor, sizeof(vColor));
		vColor[3] = nPos;
		memcpy(&m_pParam->cFadeColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_R:
		memcpy(vColor, &m_pParam->cFadeColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->cFadeColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_G:
		memcpy(vColor, &m_pParam->cFadeColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->cFadeColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_B:
		memcpy(vColor, &m_pParam->cFadeColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->cFadeColor, vColor, sizeof(vColor));
		break;
	}

	ASSERT(m_pszEffectName);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)m_pszEffectName, (LPARAM)m_pParam);
}

void CParamFadeFromToDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->bReverse = pBtn->GetCheck();
	ASSERT(m_pszEffectName);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)m_pszEffectName, (LPARAM)m_pParam);
}

void CParamFadeFromToDlg::OnBnClickedCheckFromOrTo()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_FromOrTo);
	//m_pParam->bFromOrTo = pBtn->GetCheck();
}


void CParamFadeFromToDlg::OnEnChangeEditFadecolor()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_FadeColor);
	CString str;
	pEdit->GetWindowText(str);
	_stscanf(str, _T("%08x"), &m_pParam->cFadeColor);
}

void CParamFadeFromToDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		SetParamOnWindow();
	}
}

void CParamFadeFromToDlg::SetParamOnWindow()
{
	ASSERT(m_pParam);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->bReverse);
	//pBtn = (CButton*)GetDlgItem(IDC_CHECK_FromOrTo);
	//pBtn->SetCheck(m_pParam->bFromOrTo);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fTransition);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fTransition * 10000);
	//CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_FadeColor);
	//CString str;
	//str.Format(_T("%08x"), m_pParam->cFadeColor);
	//pEdit->SetWindowText(str);
	unsigned char vColor[4];
	memcpy(&vColor, &m_pParam->cFadeColor, sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_A);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[3]);
}
