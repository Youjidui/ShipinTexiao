// ParamSony3DBrokenGlassDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamSony3DBrokenGlassDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamSony3DBrokenGlassDlg 对话框

IMPLEMENT_DYNAMIC(CParamSony3DBrokenGlassDlg, CDialog)

CParamSony3DBrokenGlassDlg::CParamSony3DBrokenGlassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamSony3DBrokenGlassDlg::IDD, pParent)
{

}

CParamSony3DBrokenGlassDlg::~CParamSony3DBrokenGlassDlg()
{
}

void CParamSony3DBrokenGlassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamSony3DBrokenGlassDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamSony3DBrokenGlassDlg::OnBnClickedCheckReverse)
	ON_BN_CLICKED(IDC_CHECK_bEnabled, &CParamSony3DBrokenGlassDlg::OnBnClickedCheckEnabled)
END_MESSAGE_MAP()


// CParamSony3DBrokenGlassDlg 消息处理程序

BOOL CParamSony3DBrokenGlassDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->bReverse);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PROGRESS);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->progress * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_divideX);
	pCtrl->SetRange(1, 40, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->divideX);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_divideY);
	pCtrl->SetRange(1, 40, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->divideY);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fallingDirection);
	pCtrl->SetRange(0, 3, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fallingDirection);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_rotateType);
	pCtrl->SetRange(0, 3, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->rotateType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_centerX);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->centerX * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_centerY);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->centerY * 10000);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK_bEnabled);
	pBtn->SetCheck(m_pParam->lightingPackParam.bEnabled);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_lightXDegree);
	pCtrl->SetRange(-90 * 1000, 90 * 1000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->lightingPackParam.lightXDegree * 1000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_lightYDegree);
	pCtrl->SetRange(-90 * 1000, 90 * 1000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->lightingPackParam.lightYDegree * 1000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_diffuse2);
	pCtrl->SetRange(0, 2 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->lightingPackParam.diffuse2 * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ambient1);
	pCtrl->SetRange(0, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->lightingPackParam.ambient1 * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fovDegree);
	pCtrl->SetRange(10 * 1000, 120 * 1000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->perspPackParam.fovDegree * 1000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamSony3DBrokenGlassDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_PROGRESS:
		m_pParam->progress = nPos / (10000.f);
		break;
	case IDC_SLIDER_divideX:
		m_pParam->divideX = nPos;
		break;
	case IDC_SLIDER_divideY:
		m_pParam->divideY = nPos;
		break;
	case IDC_SLIDER_fallingDirection:
		m_pParam->fallingDirection = nPos;
		break;
	case IDC_SLIDER_rotateType:
		m_pParam->rotateType = nPos;
		break;
	case IDC_SLIDER_centerX:
		m_pParam->centerX = nPos / (10000.f);
		break;
	case IDC_SLIDER_centerY:
		m_pParam->centerY = nPos / (10000.f);
		break;
	case IDC_SLIDER_lightXDegree:
		m_pParam->lightingPackParam.lightXDegree = nPos / (1000.f);
		break;
	case IDC_SLIDER_lightYDegree:
		m_pParam->lightingPackParam.lightYDegree = nPos / (1000.f);
		break;
	case IDC_SLIDER_diffuse2:
		m_pParam->lightingPackParam.diffuse2 = nPos / (10000.f);
		break;
	case IDC_SLIDER_ambient1:
		m_pParam->lightingPackParam.ambient1 = nPos / (10000.f);
		break;
	case IDC_SLIDER_fovDegree:
		m_pParam->perspPackParam.fovDegree = nPos / (1000.f);
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}

void CParamSony3DBrokenGlassDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->bReverse = pBtn->GetCheck();
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}

void CParamSony3DBrokenGlassDlg::OnBnClickedCheckEnabled()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bEnabled);
	m_pParam->lightingPackParam.bEnabled = pBtn->GetCheck();
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}


