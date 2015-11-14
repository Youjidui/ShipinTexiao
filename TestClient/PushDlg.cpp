// PushDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "PushDlg.h"


// PushDlg 对话框

IMPLEMENT_DYNAMIC(CPushDlg, CDialog)

CPushDlg::CPushDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPushDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CPushDlg::~CPushDlg()
{
	//deleted in CEffectBar::OnDestroy
	//delete m_pParam;
}

void CPushDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPushDlg, CDialog)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_DWTYPE, &CPushDlg::OnCbnSelchangeComboDwtype)
	ON_BN_CLICKED(IDC_CHECK_REVERSE, &CPushDlg::OnBnClickedCheckReverse)
END_MESSAGE_MAP()


// PushDlg 消息处理程序

BOOL CPushDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* pCombolBox = (CComboBox*)GetDlgItem(IDC_COMBO_DWTYPE);
	pCombolBox->InsertString(0, _T("Right2Left"));
	pCombolBox->InsertString(1, _T("Left2Right"));
	pCombolBox->InsertString(2, _T("Bottom2Top"));
	pCombolBox->InsertString(3, _T("Top2Bottom"));
	pCombolBox->InsertString(4, _T("BottomRight"));
	pCombolBox->InsertString(5, _T("BottomLeft"));
	pCombolBox->InsertString(6, _T("TopLeft"));
	pCombolBox->InsertString(7, _T("TopRight"));

	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FIRSTSOFTNESS);		//Progress
	pCtrl->SetRange(0, 100);  	//1.f, 0, 0.01f

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FIRSTCOLOR_R);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FIRSTCOLOR_G);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FIRSTCOLOR_B);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FIRSTCOLOR_A);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SECONDSOFTNESS);		//Progress
	pCtrl->SetRange(0, 100);  	//1.f, 0, 0.01f

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SECONDCOLOR_R);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SECONDCOLOR_G);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SECONDCOLOR_B);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SECONDCOLOR_A);		//Progress
	pCtrl->SetRange(0, 255);  	//0 - 255
	return TRUE;

}

void CPushDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	char vColor[4];

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_FIRSTSOFTNESS:
		m_pParam->fFirstSoftness = nPos / 100.f;
		break;
	case IDC_SLIDER_SECONDSOFTNESS:
		m_pParam->fSecondSoftness = nPos / 100.f;
		break;
	case IDC_SLIDER_FIRSTCOLOR_R:
		memcpy(vColor, &m_pParam->dwFirstColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->dwFirstColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_FIRSTCOLOR_G:
		memcpy(vColor, &m_pParam->dwFirstColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->dwFirstColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_FIRSTCOLOR_B:
		memcpy(vColor, &m_pParam->dwFirstColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->dwFirstColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_FIRSTCOLOR_A:
		memcpy(vColor, &m_pParam->dwFirstColor, sizeof(vColor));
		vColor[3] = nPos;
		memcpy(&m_pParam->dwFirstColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_SECONDCOLOR_R:
		memcpy(vColor, &m_pParam->dwSecondColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->dwSecondColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_SECONDCOLOR_G:
		memcpy(vColor, &m_pParam->dwSecondColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->dwSecondColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_SECONDCOLOR_B:
		memcpy(vColor, &m_pParam->dwSecondColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->dwSecondColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_SECONDCOLOR_A:
		memcpy(vColor, &m_pParam->dwSecondColor, sizeof(vColor));
		vColor[3] = nPos;
		memcpy(&m_pParam->dwSecondColor, vColor, sizeof(vColor));
		break;
	}

}
void CPushDlg::OnCbnSelchangeComboDwtype()
{
	CComboBox* pCombolBox = (CComboBox*)GetDlgItem(IDC_COMBO_DWTYPE);
	int index = pCombolBox->GetCurSel();
	m_pParam->dwType = index;
}

void CPushDlg::OnBnClickedCheckReverse()
{
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	int lStatus = pCheckBox->GetCheck();
	if (lStatus == 0)
	{
		m_pParam->bReverse = FALSE;
	}
	else 
	{
		m_pParam->bReverse = TRUE;
	}

}
