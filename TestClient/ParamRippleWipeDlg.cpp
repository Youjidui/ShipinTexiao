// ParamRippleWipeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamRippleWipeDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamRippleWipeDlg 对话框

IMPLEMENT_DYNAMIC(CParamRippleWipeDlg, CDialog)

CParamRippleWipeDlg::CParamRippleWipeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamRippleWipeDlg::IDD, pParent)
{

}

CParamRippleWipeDlg::~CParamRippleWipeDlg()
{
}

void CParamRippleWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamRippleWipeDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_REVERSE, &CParamRippleWipeDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()


// CParamRippleWipeDlg 消息处理程序
static const float decimal_point_4 = 10000.f;
static const float decimal_point_3 = 1000.f;

BOOL CParamRippleWipeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	pBtn->SetCheck(m_pParam->transParam.bReverse);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fRipple_Width);
	pCtrl->SetRange(0.1f * decimal_point_4, 1 * decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fRipple_Width * decimal_point_4);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fNum_Wave);
	pCtrl->SetRange(0.5f*decimal_point_3, 20*decimal_point_3, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fNum_Wave * decimal_point_3);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fAmplitude );
	pCtrl->SetRange(0, 1*decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAmplitude * decimal_point_4);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fEllipticity);
	pCtrl->SetRange(-1 * decimal_point_4, 1 * decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fEllipticity * decimal_point_4);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fCenterX);
	pCtrl->SetRange(-3 * decimal_point_4, 3 * decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fCenterX * decimal_point_4);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fCenterY);
	pCtrl->SetRange(-3 * decimal_point_4, 3 * decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fCenterX * decimal_point_4);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fDistortion);
	pCtrl->SetRange(0, 1*decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fDistortion * decimal_point_4);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBrightness);
	pCtrl->SetRange(0, 1*decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBrightness * decimal_point_4);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fLight_Angle );
	pCtrl->SetRange(-360*100, 720*100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fLight_Angle * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PROGRESS);
	pCtrl->SetRange(0 * decimal_point_4, 1 * decimal_point_4, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->transParam.progress * decimal_point_4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamRippleWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_fRipple_Width:
		m_pParam->fRipple_Width = nPos / decimal_point_4;
		break;
	case IDC_SLIDER_fNum_Wave:
		m_pParam->fNum_Wave = nPos / decimal_point_3;
		break;
	case IDC_SLIDER_fAmplitude:
		m_pParam->fAmplitude = nPos / decimal_point_4;
		break;
	case IDC_SLIDER_fEllipticity:
		m_pParam->fEllipticity = nPos / decimal_point_4;
		break;
	case IDC_SLIDER_fCenterX:
		m_pParam->fCenterX = nPos / decimal_point_4;
		break;
	case IDC_SLIDER_fCenterY:
		m_pParam->fCenterY = nPos / decimal_point_4;
		break;
	case IDC_SLIDER_fDistortion:
		m_pParam->fDistortion = nPos / decimal_point_4;
		break;
	case IDC_SLIDER_fBrightness:
		m_pParam->fBrightness = nPos / decimal_point_4;
		break;
	case IDC_SLIDER_fLight_Angle:
		m_pParam->fLight_Angle = nPos / 100.f;
		break;
	case IDC_SLIDER_PROGRESS:
		m_pParam->transParam.progress = nPos / decimal_point_4;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_RIPPLE_WIPE, (LPARAM)m_pParam);
}

void CParamRippleWipeDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	m_pParam->transParam.bReverse = pBtn->GetCheck();
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_RIPPLE_WIPE, (LPARAM)m_pParam);
}

