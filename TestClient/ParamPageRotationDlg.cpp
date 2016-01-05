// ParamPageRotationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamPageRotationDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamPageRotationDlg 对话框

IMPLEMENT_DYNAMIC(CParamPageRotationDlg, CDialog)

CParamPageRotationDlg::CParamPageRotationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamPageRotationDlg::IDD, pParent)
{

}

CParamPageRotationDlg::~CParamPageRotationDlg()
{
}

void CParamPageRotationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamPageRotationDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamPageRotationDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()


// CParamPageRotationDlg 消息处理程序
BOOL CParamPageRotationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->structTrans.bReverse);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fTransition);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->structTrans.fTransition * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nPattern);
	pCtrl->SetRange(0, 1);
	if(m_pParam)	pCtrl->SetPos(m_pParam->nPattern);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamPageRotationDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_fTransition:
		m_pParam->structTrans.fTransition = nPos / (10000.f);
		break;
	case IDC_SLIDER_nPattern:
		m_pParam->nPattern = nPos;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_PAGE_ROTATION, (LPARAM)m_pParam);
}

void CParamPageRotationDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->structTrans.bReverse = pBtn->GetCheck();
}

