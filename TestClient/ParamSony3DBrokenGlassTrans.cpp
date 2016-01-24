// ParamSony3DBrokenGlass.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamSony3DBrokenGlassTrans.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamSony3DBrokenGlassTrans 对话框

IMPLEMENT_DYNAMIC(CParamSony3DBrokenGlassTrans, CDialog)

CParamSony3DBrokenGlassTrans::CParamSony3DBrokenGlassTrans(CWnd* pParent /*=NULL*/)
: CDialog(CParamSony3DBrokenGlassTrans::IDD, pParent)
{

}

CParamSony3DBrokenGlassTrans::~CParamSony3DBrokenGlassTrans()
{
}

void CParamSony3DBrokenGlassTrans::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamSony3DBrokenGlassTrans, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_REVERSE, &CParamSony3DBrokenGlassTrans::OnBnClickedReverse)
	ON_BN_CLICKED(IDC_ENABLED, &CParamSony3DBrokenGlassTrans::OnBnClickedEnabled)
END_MESSAGE_MAP()


// CParamSony3DBrokenGlassTrans 消息处理程序
BOOL CParamSony3DBrokenGlassTrans::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PROGRESS);
	pCtrl->SetRange(0, 10000, TRUE);
	pCtrl->SetPos(m_pParam->progress * 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_DIVIDE_X);
	pCtrl->SetRange(1, 40, TRUE);
	pCtrl->SetPos(m_pParam->divideX);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_DIVIDE_Y);
	pCtrl->SetRange(1, 40, TRUE);
	pCtrl->SetPos(m_pParam->divideY);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_FALLING_DIRECTION);
	pCtrl->SetRange(1, 4, TRUE);
	pCtrl->SetPos(m_pParam->fallingDirection);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_CENTER_X);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->centerX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_CENTER_Y);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->centerY * 1000.f);


	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LIGHT_X_DEGREE);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.fDirectionX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LIGHT_Y_DEGREE);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.fDirectionY * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_DIFFUSE_2);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.fDiffuse * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_AMBIENT_1);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.fAmbient * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_FOV_DEGREE);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->perspPackParam.fovDegree * 1000.f);

	return TRUE;
}

void CParamSony3DBrokenGlassTrans::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_PROGRESS:
		m_pParam->progress = nPos / 10000.f;
		break;
	case IDC_DIVIDE_X:
		m_pParam->divideX = nPos;
		break;
	case IDC_DIVIDE_Y:
		m_pParam->divideY = nPos;
		break;
	case IDC_FALLING_DIRECTION:
		m_pParam->fallingDirection = nPos;
		break;
	case IDC_CENTER_X:
		m_pParam->centerX = nPos / 1000.f;
		break;
	case IDC_CENTER_Y:
		m_pParam->centerY = nPos / 1000.f;
		break;
	case IDC_LIGHT_X_DEGREE:
		m_pParam->lightingPackParam.fDirectionX = nPos / 1000.f;
		break;
	case IDC_LIGHT_Y_DEGREE:
		m_pParam->lightingPackParam.fDirectionY = nPos / 1000.f;
		break;
	case IDC_DIFFUSE_2:
		m_pParam->lightingPackParam.fDiffuse = nPos / 1000.f;
		break;
	case IDC_AMBIENT_1:
		m_pParam->lightingPackParam.fAmbient = nPos / 1000.f;
		break;
	case IDC_FOV_DEGREE:
		m_pParam->perspPackParam.fovDegree = nPos / 1000.f;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}
void CParamSony3DBrokenGlassTrans::OnBnClickedReverse()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_REVERSE);
	int lStatus = pCheckBox->GetCheck();
	m_pParam->bReverse = (lStatus == BST_CHECKED);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}

void CParamSony3DBrokenGlassTrans::OnBnClickedEnabled()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_ENABLED);
	int lStatus = pCheckBox->GetCheck();
	m_pParam->lightingPackParam.bEnabled = (lStatus == BST_CHECKED);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}
