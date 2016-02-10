// ParamRingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamRingsDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)


// CParamRingsDlg 对话框

IMPLEMENT_DYNAMIC(CParamRingsDlg, CDialog)

CParamRingsDlg::CParamRingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamRingsDlg::IDD, pParent)
{

}

CParamRingsDlg::~CParamRingsDlg()
{
}

void CParamRingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamRingsDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamRingsDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()

static const float decimal_point = 100.f;

// CParamRingsDlg 消息处理程序
BOOL CParamRingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	pBtn->SetCheck(m_pParam->bReverse);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PROGRESS);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fTranslate * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fRandomTranslate);
	pCtrl->SetRange(0*decimal_point, 1*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fRandomTranslate * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fRotate);
	pCtrl->SetRange(-3.14f*decimal_point, 3.14f*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fRotate * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fCenterX);
	pCtrl->SetRange(-1 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fCenterX * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fCenterY);
	pCtrl->SetRange(-1 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fCenterY * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fAspect);
	pCtrl->SetRange(-1 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAspect * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fWidth);
	pCtrl->SetRange(2*3.14f/160*decimal_point, 2*3.14f/8*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fWidth * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fRandomWidth);
	pCtrl->SetRange(0 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fRandomWidth * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fSpiral);
	pCtrl->SetRange(0, 10);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fSpiral * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fRandomPixel);
	pCtrl->SetRange(0 * decimal_point, 1 * decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fRandomPixel * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nPattern);
	pCtrl->SetRange(0, 4);
	if(m_pParam)	pCtrl->SetPos(m_pParam->nPattern * decimal_point);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamRingsDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_PROGRESS:
		m_pParam->fTranslate = nPos / (10000.f);
		break;
	case IDC_SLIDER_fRandomTranslate:
		m_pParam->fRandomTranslate = nPos / decimal_point;
		break;
	case IDC_SLIDER_fRotate:
		m_pParam->fRotate = nPos / decimal_point;
		break;
	case IDC_SLIDER_fCenterX:
		m_pParam->fCenterX = nPos / decimal_point;
		break;
	case IDC_SLIDER_fCenterY:
		m_pParam->fCenterY = nPos / decimal_point;
		break;
	case IDC_SLIDER_fAspect:
		m_pParam->fAspect = nPos / decimal_point;
		break;
	case IDC_SLIDER_fWidth:
		m_pParam->fWidth = nPos / decimal_point;
		break;
	case IDC_SLIDER_fRandomWidth:
		m_pParam->fRandomWidth = nPos / decimal_point;
		break;
	case IDC_SLIDER_fSpiral:
		m_pParam->fSpiral = nPos;
		break;
	case IDC_SLIDER_fRandomPixel:
		m_pParam->fRandomPixel = nPos / decimal_point;
		break;
	case IDC_SLIDER_nPattern:
		m_pParam->nPattern = nPos / decimal_point;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_RINGS, (LPARAM)m_pParam);
}

void CParamRingsDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	m_pParam->bReverse = pBtn->GetCheck();
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_RINGS, (LPARAM)m_pParam);
}

