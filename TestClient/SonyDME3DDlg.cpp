// SonyDME3DDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "SonyDME3DDlg.h"


// CSonyDME3DDlg 对话框

IMPLEMENT_DYNAMIC(CSonyDME3DDlg, CDialog)

CSonyDME3DDlg::CSonyDME3DDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSonyDME3DDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CSonyDME3DDlg::~CSonyDME3DDlg()
{
}

void CSonyDME3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSonyDME3DDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_BFLITER, &CSonyDME3DDlg::OnBnClickedCheckBfliter)
END_MESSAGE_MAP()


// CSonyDME3DDlg 消息处理程序
BOOL CSonyDME3DDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_PRESIZE_X);		//Progress
	pCtrl->SetRange(0, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_PRESIZE_Y);		//Progress
	pCtrl->SetRange(0, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SKEW_X);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SKEW_Y);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_LOCATION_X);
	pCtrl->SetRange(-30000, 30000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_LOCATION_Y);
	pCtrl->SetRange(-30000, 30000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_LOCATION_Z);
	pCtrl->SetRange(-30000, 30000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_SPIN_X);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_SPIN_Y);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_SPIN_Z);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_ROTATION_X);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_ROTATION_Y);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_ROTATION_Z);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_AXIS_X);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_AXIS_Y);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_AXIS_Z);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_LOCATION_X2);
	pCtrl->SetRange(-30000, 30000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_LOCATION_Y2);
	pCtrl->SetRange(-30000, 30000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_LOCATION_Z2);
	pCtrl->SetRange(-30000, 30000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_SPIN_X2);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_SPIN_Y2);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_SPIN_Z2);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_ROTATION_X2);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_ROTATION_Y2);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_ROTATION_Z2);
	pCtrl->SetRange(-5000, 5000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_AXIS_X2);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_AXIS_Y2);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_L_AXIS_Z2);
	pCtrl->SetRange(-10000, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_PERSPECTIVE);
	pCtrl->SetRange(-12000, 1000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_POST_SIZE_X);
	pCtrl->SetRange(0, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_POST_SIZE_Y);
	pCtrl->SetRange(0, 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_POST_LOCATION_X);
	pCtrl->SetRange(-30000, 30000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_POST_LOCATION_Y);
	pCtrl->SetRange(-30000, 30000);

	return TRUE;
}


void CSonyDME3DDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
		case IDC_PRESIZE_X:
			m_pParam->fPreSizeX = nPos / 1000.f;
			break;
		case IDC_PRESIZE_Y:
			m_pParam->fPreSizeY = nPos / 1000.f;
			break;
		case IDC_SKEW_X:
			m_pParam->fSkewX = nPos / 1000.f;
			break;
		case IDC_SKEW_Y:
			m_pParam->fSkewY = nPos / 1000.f;
			break;
		case IDC_L_LOCATION_X:
			m_pParam->sLocal.fLocationX = nPos / 100.f;
			break;
		case IDC_L_LOCATION_Y:
			m_pParam->sLocal.fLocationY = nPos / 100.f;
			break;
		case IDC_L_LOCATION_Z:
			m_pParam->sLocal.fLocationZ = nPos / 100.f;
			break;
		case IDC_L_SPIN_X:
			m_pParam->sLocal.fSpinX = nPos / 1000.f;
			break;
		case IDC_L_SPIN_Y:
			m_pParam->sLocal.fSpinY = nPos / 1000.f;
			break;
		case IDC_L_SPIN_Z:
			m_pParam->sLocal.fSpinZ = nPos / 1000.f;
			break;
		case IDC_L_ROTATION_X:
			m_pParam->sLocal.fRotationX = nPos / 1000.f;
			break;
		case IDC_L_ROTATION_Y:
			m_pParam->sLocal.fRotationY = nPos / 1000.f;
			break;
		case IDC_L_ROTATION_Z:
			m_pParam->sLocal.fRotationZ = nPos / 1000.f;
			break;
		case IDC_L_AXIS_X:
			m_pParam->sLocal.fAxisX = nPos / 1000.f;
			break;
		case IDC_L_AXIS_Y:
			m_pParam->sLocal.fAxisY = nPos / 1000.f;
			break;
		case IDC_L_AXIS_Z:
			m_pParam->sLocal.fAxisZ = nPos / 1000.f;
			break;
		case IDC_L_LOCATION_X2:
			m_pParam->sGlobal.fLocationX = nPos / 100.f;
			break;
		case IDC_L_LOCATION_Y2:
			m_pParam->sGlobal.fLocationY = nPos / 100.f;
			break;
		case IDC_L_LOCATION_Z2:
			m_pParam->sGlobal.fLocationZ = nPos / 100.f;
			break;
		case IDC_L_SPIN_X2:
			m_pParam->sGlobal.fSpinX = nPos / 1000.f;
			break;
		case IDC_L_SPIN_Y2:
			m_pParam->sGlobal.fSpinY = nPos / 1000.f;
			break;
		case IDC_L_SPIN_Z2:
			m_pParam->sGlobal.fSpinZ = nPos / 1000.f;
			break;
		case IDC_L_ROTATION_X2:
			m_pParam->sGlobal.fRotationX = nPos / 1000.f;
			break;
		case IDC_L_ROTATION_Y2:
			m_pParam->sGlobal.fRotationY = nPos / 1000.f;
			break;
		case IDC_L_ROTATION_Z2:
			m_pParam->sGlobal.fRotationZ = nPos / 1000.f;
			break;
		case IDC_L_AXIS_X2:
			m_pParam->sGlobal.fAxisX = nPos / 1000.f;
			break;
		case IDC_L_AXIS_Y2:
			m_pParam->sGlobal.fAxisY = nPos / 1000.f;
			break;
		case IDC_L_AXIS_Z2:
			m_pParam->sGlobal.fAxisZ = nPos / 1000.f;
			break;
		case IDC_PERSPECTIVE:
			m_pParam->fPerspective = nPos / 100.f;
			break;
		case IDC_POST_SIZE_X:
			m_pParam->fPostSizeX = nPos / 1000.f;
			break;
		case IDC_POST_SIZE_Y:
			m_pParam->fPostSizeY = nPos / 1000.f;
			break;
		case IDC_POST_LOCATION_X:
			m_pParam->fPostLocationX = nPos / 100.f;
			break;
		case IDC_POST_LOCATION_Y:
			m_pParam->fPostLocationY = nPos / 100.f;
			break;
	}
}

void CSonyDME3DDlg::OnBnClickedCheckBfliter()
{
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_BFLITER);
	int lStatus = pCheckBox->GetCheck();
	if (lStatus == 0)
	{
		m_pParam->bFilter = FALSE;
	}
	else 
	{
		m_pParam->bFilter = TRUE;
	}
}
