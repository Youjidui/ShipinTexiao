// ParamFadeFromToDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamFadeFromToDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamFadeFromToDlg �Ի���

IMPLEMENT_DYNAMIC(CParamFadeFromToDlg, CDialog)

CParamFadeFromToDlg::CParamFadeFromToDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamFadeFromToDlg::IDD, pParent)
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
END_MESSAGE_MAP()


// CParamFadeFromToDlg ��Ϣ�������
BOOL CParamFadeFromToDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->bReverse);
	pBtn = (CButton*)GetDlgItem(IDC_CHECK_FromOrTo);
	pBtn->SetCheck(m_pParam->bFromOrTo);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fTransition);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fTransition * 10000);
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_FadeColor);
	CString str;
	str.Format(_T("%08x"), m_pParam->cFadeColor);
	pEdit->SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CParamFadeFromToDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_fTransition:
		m_pParam->fTransition = nPos / (10000.f);
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_FADE_FROM_TO, (LPARAM)m_pParam);
}

void CParamFadeFromToDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->bReverse = pBtn->GetCheck();
}

void CParamFadeFromToDlg::OnBnClickedCheckFromOrTo()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_FromOrTo);
	m_pParam->bFromOrTo = pBtn->GetCheck();
}


void CParamFadeFromToDlg::OnEnChangeEditFadecolor()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_FadeColor);
	CString str;
	pEdit->GetWindowText(str);
	_stscanf(str, _T("%08x"), &m_pParam->cFadeColor);
}
