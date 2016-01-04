// ParamBrokenGlassDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamBrokenGlassDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)


// CParamBrokenGlassDlg 对话框

IMPLEMENT_DYNAMIC(CParamBrokenGlassDlg, CDialog)

CParamBrokenGlassDlg::CParamBrokenGlassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamBrokenGlassDlg::IDD, pParent)
{

}

CParamBrokenGlassDlg::~CParamBrokenGlassDlg()
{
}

void CParamBrokenGlassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamBrokenGlassDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamBrokenGlassDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()


static const float decimal_point = 100.f;

// CParamBrokenGlassDlg 消息处理程序
BOOL CParamBrokenGlassDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->prm_bReverse);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_movement);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_movement * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_movementRandom);
	pCtrl->SetRange(0*decimal_point, 1*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_movementRandom * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_rotate );
	pCtrl->SetRange(-3.14f*decimal_point, 3.14f*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_rotate * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_centerX);
	pCtrl->SetRange(-1 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_centerX * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_centerY);
	pCtrl->SetRange(-1 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_centerY * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_aspect);
	pCtrl->SetRange(-1 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_aspect * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_width);
	pCtrl->SetRange(2*3.14f/160*decimal_point, 2*3.14f/8*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_width * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_widthRandom);
	pCtrl->SetRange(0 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_widthRandom * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_angleBlock );
	pCtrl->SetRange(0*decimal_point, 3.14f*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_angleBlock * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_randomPix);
	pCtrl->SetRange(0 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->prm_randomPix * decimal_point);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamBrokenGlassDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_movement:
		m_pParam->prm_movement = nPos / (10000.f);
		break;
	case IDC_SLIDER_movementRandom:
		m_pParam->prm_movementRandom = nPos / decimal_point;
		break;
	case IDC_SLIDER_rotate:
		m_pParam->prm_rotate = nPos / decimal_point;
		break;
	case IDC_SLIDER_centerX:
		m_pParam->prm_centerX = nPos / decimal_point;
		break;
	case IDC_SLIDER_centerY:
		m_pParam->prm_centerY = nPos / decimal_point;
		break;
	case IDC_SLIDER_aspect:
		m_pParam->prm_aspect = nPos / decimal_point;
		break;
	case IDC_SLIDER_width:
		m_pParam->prm_width = nPos / decimal_point;
		break;
	case IDC_SLIDER_widthRandom:
		m_pParam->prm_widthRandom = nPos / decimal_point;
		break;
	case IDC_SLIDER_angleBlock:
		m_pParam->prm_angleBlock = nPos / decimal_point;
		break;
	case IDC_SLIDER_randomPix:
		m_pParam->prm_randomPix = nPos / decimal_point;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_BROKEN_GLASS, (LPARAM)m_pParam);
}

void CParamBrokenGlassDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->prm_bReverse = pBtn->GetCheck();
}

