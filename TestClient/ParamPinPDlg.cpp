// ParamPinPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamPinPDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)
#pragma warning(disable:4996)

// CParamPinPDlg 对话框

IMPLEMENT_DYNAMIC(CParamPinPDlg, CDialog)

CParamPinPDlg::CParamPinPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamPinPDlg::IDD, pParent)
{

}

CParamPinPDlg::~CParamPinPDlg()
{
}

void CParamPinPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamPinPDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bEnableShadow, &CParamPinPDlg::OnBnClickedCheckbenableshadow)
	ON_BN_CLICKED(IDC_CHECK_bFilter, &CParamPinPDlg::OnBnClickedCheckbfilter)
	ON_EN_CHANGE(IDC_EDIT_cBackGroundSepiaColor, &CParamPinPDlg::OnEnChangeEditcbackgroundsepiacolor)
	ON_EN_CHANGE(IDC_EDIT_cBorderColor, &CParamPinPDlg::OnEnChangeEditcbordercolor)
	ON_EN_CHANGE(IDC_EDIT_cShadowColor, &CParamPinPDlg::OnEnChangeEditcshadowcolor)
END_MESSAGE_MAP()


// CParamPinPDlg 消息处理程序

static float decimal_point = 10000.f;

BOOL CParamPinPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton* pBtn = NULL;
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fScaleX);
	pCtrl->SetRange(1, 100* 100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fScaleX * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fScaleY);
	pCtrl->SetRange(1, 100*100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fScaleY * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fPositionX);
	pCtrl->SetRange(-2 * 10000, 2*10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fPositionX * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fPositionY);
	pCtrl->SetRange(-2 * 10000, 2*10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fPositionY * 10000);
	pBtn = (CButton*)GetDlgItem(IDC_CHECK_bFilter);
	pBtn->SetCheck(m_pParam->bFilter);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBorderWidth);
	pCtrl->SetRange(0, 10000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBorderWidth*10000);
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_cBorderColor);
	CString str;
	str.Format(_T("%08x"), m_pParam->cBorderColor);
	pEdit->SetWindowText(str);
	pBtn = (CButton*)GetDlgItem(IDC_CHECK_bEnableShadow);
	pBtn->SetCheck(m_pParam->bEnableShadow);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_cShadowColor);
	str.Format(_T("%08x"), m_pParam->cShadowColor);
	pEdit->SetWindowText(str);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fShadowDropDistance);
	pCtrl->SetRange(0, 2*10000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fShadowDropDistance*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fShadowDropLightAngle);
	pCtrl->SetRange(-720*100, 720* 100);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fShadowDropLightAngle*100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fShadowDropTransparency);
	pCtrl->SetRange(0, 1*10000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fShadowDropTransparency*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fShadowDropSoftness);
	pCtrl->SetRange(0, 1*10000);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fShadowDropSoftness*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_dwTrailDecayType);
	pCtrl->SetRange(0,1);
	if(m_pParam)	pCtrl->SetPos(m_pParam->dwTrailDecayType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_iTrailDecayTime);
	pCtrl->SetRange(0,30);
	if(m_pParam)	pCtrl->SetPos(m_pParam->iTrailDecayTime);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_dwBackGroundType);
	pCtrl->SetRange(0,3, TRUE);		//1,3 or 0,3
	if(m_pParam)	pCtrl->SetPos(m_pParam->dwBackGroundType);
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_cBackGroundSepiaColor);
	str.Format(_T("%08x"), m_pParam->cBackGroundSepiaColor);
	pEdit->SetWindowText(str);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBackGoundDensity);
	pCtrl->SetRange(0, 100 * 100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBackGoundDensity*100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBoundsLeft);
	pCtrl->SetRange(0, 2 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBoundsLeft*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBoundsTop);
	pCtrl->SetRange(0, 2 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBoundsTop*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBoundsRight);
	pCtrl->SetRange(0, 2 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBoundsRight*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBoundsBottom);
	pCtrl->SetRange(0, 2 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBoundsBottom*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fTansparency);
	pCtrl->SetRange(0, 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fTansparency*10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyScaleX1);
	pCtrl->SetRange(1, 100* 100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyScaleX[0] * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyScaleX2);
	pCtrl->SetRange(1, 100* 100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyScaleX[1] * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyScaleY1);
	pCtrl->SetRange(1, 100*100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyScaleY[0] * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyScaleY2);
	pCtrl->SetRange(1, 100*100, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyScaleY[1] * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyPositionX1);
	pCtrl->SetRange(-2 * 10000, 2*10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyPositionX[0] * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyPositionX2);
	pCtrl->SetRange(-2 * 10000, 2*10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyPositionX[1] * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyPositionY1);
	pCtrl->SetRange(-2 * 10000, 2*10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyPositionY[0] * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fKeyPositionY2);
	pCtrl->SetRange(-2 * 10000, 2*10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fKeyPositionY[1] * 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nKeyCount);
	pCtrl->SetRange(0, 10*30, TRUE);	//??
	if(m_pParam)	pCtrl->SetPos(m_pParam->nKeyCount*30);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamPinPDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_fScaleX:
		m_pParam->fScaleX = nPos / 100.f;
		break;
	case IDC_SLIDER_fScaleY:
		m_pParam->fScaleY = nPos / 100.f;
		break;
	case IDC_SLIDER_fPositionX:
		m_pParam->fPositionX = nPos / 10000.f;
		break;
	case IDC_SLIDER_fPositionY:
		m_pParam->fPositionY = nPos / 10000.f;
		break; 
	case IDC_SLIDER_fBorderWidth:
		m_pParam->fBorderWidth = nPos / 10000.f;
		break;
	case IDC_SLIDER_fShadowDropDistance:
		m_pParam->fShadowDropDistance = nPos / 10000.f;
		break;
	case IDC_SLIDER_fShadowDropLightAngle:
		m_pParam->fShadowDropLightAngle = nPos/100.f;
		break;
	case IDC_SLIDER_fShadowDropTransparency:
		m_pParam->fShadowDropTransparency = nPos/10000.f;
		break;
	case IDC_SLIDER_fShadowDropSoftness:
		m_pParam->fShadowDropSoftness = nPos/10000.f;
		break;
	case IDC_SLIDER_dwTrailDecayType:
		m_pParam->dwTrailDecayType = nPos;
		break;
	case IDC_SLIDER_iTrailDecayTime:
		m_pParam->iTrailDecayTime = nPos;
		break;
	case IDC_SLIDER_dwBackGroundType:
		m_pParam->dwBackGroundType = nPos;
		break;
	case IDC_SLIDER_fBackGoundDensity:
		m_pParam->fBackGoundDensity = nPos * 100.f;
		break;
	case IDC_SLIDER_fBoundsLeft:
		m_pParam->fBoundsLeft = nPos / decimal_point;
		break;
	case IDC_SLIDER_fBoundsTop:
		m_pParam->fBoundsTop = nPos / decimal_point;
		break;
	case IDC_SLIDER_fBoundsRight:
		m_pParam->fBoundsRight = nPos / decimal_point;
		break;
	case IDC_SLIDER_fBoundsBottom:
		m_pParam->fBoundsBottom = nPos / decimal_point;
		break;
	case IDC_SLIDER_fTansparency:
		m_pParam->fTansparency = nPos / decimal_point;
		break;
	case IDC_SLIDER_fKeyScaleX1:
		m_pParam->fKeyScaleX[0] = nPos/100.f;
		break;
	case IDC_SLIDER_fKeyScaleX2:
		m_pParam->fKeyScaleX[1] = nPos/100.f;
		break;
	case IDC_SLIDER_fKeyScaleY1:
		m_pParam->fKeyScaleY[0] = nPos / 100.f;
		break;
	case IDC_SLIDER_fKeyScaleY2:
		m_pParam->fKeyScaleY[1] = nPos / 100.f;
		break;
	case IDC_SLIDER_fKeyPositionX1:
		m_pParam->fKeyPositionX[0] = nPos / decimal_point;
		break;
	case IDC_SLIDER_fKeyPositionX2:
		m_pParam->fKeyPositionX[1] = nPos / decimal_point;
		break;
	case IDC_SLIDER_fKeyPositionY1:
		m_pParam->fKeyPositionY[0] = nPos / decimal_point;
		break;
	case IDC_SLIDER_fKeyPositionY2:
		m_pParam->fKeyPositionY[1] = nPos / decimal_point;
		break;
	case IDC_SLIDER_nKeyCount:
		m_pParam->nKeyCount = nPos / 30.f;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_PINP, (LPARAM)m_pParam);
}


void CParamPinPDlg::OnBnClickedCheckbenableshadow()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bEnableShadow);
	m_pParam->bEnableShadow = pBtn->GetCheck();
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_PINP, (LPARAM)m_pParam);
}

void CParamPinPDlg::OnBnClickedCheckbfilter()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bFilter);
	m_pParam->bFilter = pBtn->GetCheck();
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_PINP, (LPARAM)m_pParam);
}

void CParamPinPDlg::OnEnChangeEditcbackgroundsepiacolor()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_cBackGroundSepiaColor);
	CString str;
	pEdit->GetWindowText(str);
	_stscanf(str, _T("%08x"), &m_pParam->cBackGroundSepiaColor);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_PINP, (LPARAM)m_pParam);
}

void CParamPinPDlg::OnEnChangeEditcbordercolor()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_cBorderColor);
	CString str;
	pEdit->GetWindowText(str);
	_stscanf(str, _T("%08x"), &m_pParam->cBorderColor);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_PINP, (LPARAM)m_pParam);
}

void CParamPinPDlg::OnEnChangeEditcshadowcolor()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_cShadowColor);
	CString str;
	pEdit->GetWindowText(str);
	_stscanf(str, _T("%08x"), &m_pParam->cShadowColor);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_PINP, (LPARAM)m_pParam);
}
