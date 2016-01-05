// ParamDissolveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamDissolveDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamDissolveDlg 对话框

IMPLEMENT_DYNAMIC(CParamDissolveDlg, CDialog)

CParamDissolveDlg::CParamDissolveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamDissolveDlg::IDD, pParent)
{

}

CParamDissolveDlg::~CParamDissolveDlg()
{
}

void CParamDissolveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamDissolveDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamDissolveDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()


// CParamDissolveDlg 消息处理程序
BOOL CParamDissolveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->prm_bReverse);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_process);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_process * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_mixType);
	pCtrl->SetRange(0, 4);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_mixType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_subtractr);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_subtractr * 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamDissolveDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_process:
		m_pParam->prm_process = nPos / (10000.f);
		break;
	case IDC_SLIDER_mixType:
		m_pParam->prm_mixType = nPos;
		break;
	case IDC_SLIDER_subtractr:
		m_pParam->prm_subtractr = nPos / (10000.f);
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_DISSOLVE, (LPARAM)m_pParam);
}

void CParamDissolveDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->prm_bReverse = pBtn->GetCheck();
}

