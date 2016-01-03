// ParamSonyBarnSlideDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamSonyBarnSlideDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)


// CParamSonyBarnSlideDlg 对话框

IMPLEMENT_DYNAMIC(CParamSonyBarnSlideDlg, CDialog)

CParamSonyBarnSlideDlg::CParamSonyBarnSlideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamSonyBarnSlideDlg::IDD, pParent)
{

}

CParamSonyBarnSlideDlg::~CParamSonyBarnSlideDlg()
{
}

void CParamSonyBarnSlideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamSonyBarnSlideDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse1, &CParamSonyBarnSlideDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()

static float decimal_point = 100.f;

// CParamSonyBarnSlideDlg 消息处理程序
BOOL CParamSonyBarnSlideDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse1);
	pBtn->SetCheck(m_pParam->bReverse);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nType);
	pCtrl->SetRange(0, 2);
	if(m_pParam)	pCtrl->SetPos(m_pParam->nType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fSlant);
	pCtrl->SetRange(-45*decimal_point, 45*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fSlant * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fTrans);
	pCtrl->SetRange(0, 10000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fTrans * 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamSonyBarnSlideDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_nType:
		m_pParam->nType = nPos ;
		break;
	case IDC_SLIDER_fSlant:
		m_pParam->fSlant = nPos / decimal_point;
		break;
	case IDC_SLIDER_fTrans:
		m_pParam->fTrans = nPos / 10000.f;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_BARN_SLIDE, (LPARAM)m_pParam);
}

void CParamSonyBarnSlideDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse1);
	m_pParam->bReverse = pBtn->GetCheck();
}

