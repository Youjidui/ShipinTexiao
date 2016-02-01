// ParamDipToColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamDipToColorDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)

const float MaxValue = 256.0f;

// CParamDipToColorDlg 对话框

IMPLEMENT_DYNAMIC(CParamDipToColorDlg, CDialog)

CParamDipToColorDlg::CParamDipToColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamDipToColorDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CParamDipToColorDlg::~CParamDipToColorDlg()
{
}

void CParamDipToColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamDipToColorDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamDipToColorDlg::OnBnClickedCheckbreverse)
END_MESSAGE_MAP()


// CParamDipToColorDlg 消息处理程序
void CParamDipToColorDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_R:
		m_pParam->fColor[0] = nPos / MaxValue;
		break;
	case IDC_SLIDER_G:
		m_pParam->fColor[1] = nPos / MaxValue;
		break;
	case IDC_SLIDER_B:
		m_pParam->fColor[2] = nPos / MaxValue;
		break;
	case IDC_SLIDER_A:
		m_pParam->fColor[3] = nPos / MaxValue;
		break; 
	case IDC_SLIDER_PROGRESS:
		m_pParam->fProcess = nPos / 10000.0f;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_DIP_TO_COLOR, (LPARAM)m_pParam);
}

BOOL CParamDipToColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->bReverse);

	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fColor[0] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fColor[1] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fColor[2] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_A);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fColor[3] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE1);
	pCtrl->SetRange(0, D3DX_PI * 1000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fProcess * 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamDipToColorDlg::OnBnClickedCheckbreverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->bReverse = pBtn->GetCheck();
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_DIP_TO_COLOR, (LPARAM)m_pParam);
}
