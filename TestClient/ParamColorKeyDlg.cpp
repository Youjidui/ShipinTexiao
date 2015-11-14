// ParamColorKeyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamColorKeyDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)

const float MaxValue = 256.0f;

// CParamColorKeyDlg 对话框

IMPLEMENT_DYNAMIC(CParamColorKeyDlg, CDialog)

CParamColorKeyDlg::CParamColorKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamColorKeyDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CParamColorKeyDlg::~CParamColorKeyDlg()
{
	delete m_pParam;
}

void CParamColorKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamColorKeyDlg, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CParamColorKeyDlg 消息处理程序


void CParamColorKeyDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
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
		m_pParam->fKeyColor[0] = nPos / MaxValue;
		break;
	case IDC_SLIDER_G:
		m_pParam->fKeyColor[1] = nPos / MaxValue;
		break;
	case IDC_SLIDER_B:
		m_pParam->fKeyColor[2] = nPos / MaxValue;
		break;
	case IDC_SLIDER_A:
		m_pParam->fKeyColor[3] = nPos / MaxValue;
		break; 
	case IDC_SLIDER_ANGLE1:
		m_pParam->fAngle1 = nPos / 1000.0f;
		break;
	case IDC_SLIDER_LENGTH1:
		m_pParam->fLength1 = nPos;
		break;
	case IDC_SLIDER_ANGLE2:
		m_pParam->fAngle2 = nPos / 1000.0f;
		break;
	case IDC_SLIDER_LENGTH2:
		m_pParam->fLength2 = nPos;
		break;
	case IDC_SLIDER_SOFTNESS:
		m_pParam->fSoftness = nPos / MaxValue;
		break;
	case IDC_SLIDER_BLURNESS:
		m_pParam->fBlurness = nPos * 10 / MaxValue;
		break;
	case IDC_SLIDER_FINE_TUNE:
		m_pParam->fFineTune = nPos / MaxValue;
		break;
	case IDC_SLIDER_GRAY_MIN:
		m_pParam->fGrayMin = nPos / MaxValue;
		break;
	case IDC_SLIDER_GRAY_MAX:
		m_pParam->fGrayMax = nPos / MaxValue;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_COLOR_KEY, (LPARAM)m_pParam);
}

BOOL CParamColorKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyColor[0] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyColor[1] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyColor[2] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_A);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyColor[3] * MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE1);
	pCtrl->SetRange(0, D3DX_PI * 1000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAngle1 * 1000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LENGTH1);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fLength1* MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE2);
	pCtrl->SetRange(0, D3DX_PI * 1000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAngle2 * 1000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LENGTH2);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fLength2* MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SOFTNESS);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fSoftness* MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BLURNESS);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBlurness* MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FINE_TUNE);
	pCtrl->SetRange(-255, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fFineTune* MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GRAY_MIN);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fGrayMin* MaxValue);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GRAY_MAX);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fGrayMax* MaxValue);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
