// ParamChromaKeyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamChromaKeyDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamChromaKeyDlg 对话框

IMPLEMENT_DYNAMIC(CParamChromaKeyDlg, CDialog)

CParamChromaKeyDlg::CParamChromaKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamChromaKeyDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CParamChromaKeyDlg::~CParamChromaKeyDlg()
{
	//deleted in CEffectBar::OnDestroy
	//delete m_pParam;
}

void CParamChromaKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamChromaKeyDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_CC_ON, &CParamChromaKeyDlg::OnBnClickedCheckCcOn)
	ON_BN_CLICKED(IDC_CHECK_CK_INVERT, &CParamChromaKeyDlg::OnBnClickedCheckCkInvert)
	ON_BN_CLICKED(IDC_CHECK_COLOR_KEY, &CParamChromaKeyDlg::OnBnClickedCheckColorKey)
	ON_BN_CLICKED(IDC_CHECK_YB_ON, &CParamChromaKeyDlg::OnBnClickedCheckYbOn)
END_MESSAGE_MAP()


// CParamChromaKeyDlg 消息处理程序

const float decimal_point = 100.0f;

BOOL CParamChromaKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CButton* pBtn = NULL;
	//pBtn = (CButton*)GetDlgItem(IDC_CHECK_COLOR_KEY);
	//pBtn->SetCheck(m_pParam->paramCrkAdj.bOn);
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CK_CLIP);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkAdj.fClip * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CK_GAIN);
	pCtrl->SetRange(-100*decimal_point, 100*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkAdj.fGain * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CK_HUE);
	pCtrl->SetRange(0, 360*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkAdj.fHue * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CK_SAT_CROP);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkAdj.fSatCrop*decimal_point);
	//pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CK_FILTER);
	//pCtrl->SetRange(0, 9);
	//if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkAdj.nFilter);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CK_ANGLE);
	pCtrl->SetRange(0, 180 * decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkAdj.fAngle*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DENSITY);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkAdj.fDensity*decimal_point);
	pBtn = (CButton*)GetDlgItem(IDC_CHECK_CK_INVERT);
	pBtn->SetCheck(m_pParam->paramCrkAdj.bInvert);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_REGION_LEFT);
	pCtrl->SetRange(0 * 10000,  1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramRgn.fLeft*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_REGION_TOP);
	pCtrl->SetRange(0 * 10000, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramRgn.fTop*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_REGION_RIGHT);
	pCtrl->SetRange(0 * 10000,  1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramRgn.fRight*10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_REGION_BOTTOM);
	pCtrl->SetRange(0 * 10000, 1 * 10000, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramRgn.fBottom*10000);

	//pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CKP_LEFT);
	//pCtrl->SetRange(-4 * 10000,  4 * 10000, TRUE);
	//if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkPos.fLeft*10000);
	//pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CKP_RIGHT);
	//pCtrl->SetRange(-4 * 10000, 4 * 10000, TRUE);
	//if(m_pParam)	pCtrl->SetPos(m_pParam->paramCrkPos.fRight*10000);


	pBtn = (CButton*)GetDlgItem(IDC_CHECK_YB_ON);
	pBtn->SetCheck(m_pParam->paramYBal.bOn);
	
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_YB_CLIP);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramYBal.fClip*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_YB_GAIN);
	pCtrl->SetRange(-100*decimal_point, 100*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramYBal.fGain*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_YB_LUM);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramYBal.fLum*decimal_point);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK_CC_ON);
	pBtn->SetCheck(m_pParam->paramCc.bOn);
	
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CC_LUM);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCc.fLum*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CC_SAT);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCc.fSat*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CC_HUE);
	pCtrl->SetRange(0, 360*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCc.fHue*decimal_point);
	//pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CC_FILTER);
	//pCtrl->SetRange(0, 9);
	//if(m_pParam)	pCtrl->SetPos(m_pParam->paramCc.nFilter);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CC_CLIP);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCcKey.fClip*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CC_GAIN);
	pCtrl->SetRange(-100*decimal_point, 100*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramCcKey.fGain*decimal_point);

	//pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CCP_LEFT);
	//pCtrl->SetRange(-3 * 10000,  3 * 10000, TRUE);
	//if(m_pParam)	pCtrl->SetPos(m_pParam->paramCckPos.fLeft*10000);
	//pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CCP_RIGHT);
	//pCtrl->SetRange(-3 * 10000, 3 * 10000, TRUE);
	//if(m_pParam)	pCtrl->SetPos(m_pParam->paramCckPos.fRight*10000);


	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VW_SAT_CROP);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramVecWnd.fSatCrop*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VW_ANGLE);
	pCtrl->SetRange(0, 180* decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramVecWnd.fAngle*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VW_MIX);
	pCtrl->SetRange(0, 100*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramYBalMix.fMix*decimal_point);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VA_VGAIN);
	pCtrl->SetRange(-100*decimal_point, 100*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramFGVAdj.fVGain*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VA_yGAIN);
	pCtrl->SetRange(-100*decimal_point, 100*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramFGVAdj.fYGain*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VA_CGAIN);
	pCtrl->SetRange(-100*decimal_point, 100*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramFGVAdj.fCGain*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VA_HUE);
	pCtrl->SetRange(-180*decimal_point, 180*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->paramFGVAdj.fHue*decimal_point);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamChromaKeyDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_CK_CLIP:
		m_pParam->paramCrkAdj.fClip = nPos / decimal_point;
		break;
	case IDC_SLIDER_CK_GAIN:
		m_pParam->paramCrkAdj.fGain = nPos / decimal_point;
		break;
	case IDC_SLIDER_CK_HUE:
		m_pParam->paramCrkAdj.fHue = nPos / decimal_point;
		break;
	case IDC_SLIDER_CK_SAT_CROP:
		m_pParam->paramCrkAdj.fSatCrop = nPos / decimal_point;
		break; 
	//case IDC_SLIDER_CK_FILTER:
	//	m_pParam->paramCrkAdj.nFilter = nPos;
	//	break;
	case IDC_SLIDER_CK_ANGLE:
		m_pParam->paramCrkAdj.fAngle = nPos / decimal_point;
		break;
	case IDC_SLIDER_DENSITY:
		m_pParam->paramCrkAdj.fDensity = nPos/decimal_point;
		break;
	//case IDC_SLIDER_CKP_LEFT:
	//	m_pParam->paramCrkPos.fLeft = nPos/10000.f;
	//	break;
	//case IDC_SLIDER_CKP_RIGHT:
	//	m_pParam->paramCrkPos.fRight = nPos/10000.f;
	//	break;
	case IDC_SLIDER_REGION_LEFT:
		m_pParam->paramRgn.fLeft = nPos/10000.f;
		break;
	case IDC_SLIDER_REGION_RIGHT:
		m_pParam->paramRgn.fRight = nPos/10000.f;
		break;
	case IDC_SLIDER_REGION_TOP:
		m_pParam->paramRgn.fTop = nPos/10000.f;
		break;
	case IDC_SLIDER_REGION_BOTTOM:
		m_pParam->paramRgn.fBottom = nPos/10000.f;
		break;
	case IDC_SLIDER_YB_CLIP:
		m_pParam->paramYBal.fClip = nPos / decimal_point;
		break;
	case IDC_SLIDER_YB_GAIN:
		m_pParam->paramYBal.fGain = nPos/decimal_point;
		break;
	case IDC_SLIDER_YB_LUM:
		m_pParam->paramYBal.fLum = nPos / decimal_point;
		break;
	case IDC_SLIDER_CC_LUM:
		m_pParam->paramCc.fLum = nPos * decimal_point;
		break;
	case IDC_SLIDER_CC_SAT:
		m_pParam->paramCc.fSat = nPos / decimal_point;
		break;
	case IDC_SLIDER_CC_HUE:
		m_pParam->paramCc.fHue = nPos / decimal_point;
		break;
	//case IDC_SLIDER_CC_FILTER:
	//	m_pParam->paramCc.nFilter = nPos;
	//	break;
	case IDC_SLIDER_CC_CLIP:
		m_pParam->paramCcKey.fClip = nPos / decimal_point;
		break;
	case IDC_SLIDER_CC_GAIN:
		m_pParam->paramCcKey.fGain = nPos / decimal_point;
		break;
	//case IDC_SLIDER_CCP_LEFT:
	//	m_pParam->paramCckPos.fLeft = nPos/10000.f;
	//	break;
	//case IDC_SLIDER_CCP_RIGHT:
	//	m_pParam->paramCckPos.fRight = nPos/10000.f;
	//	break;
	case IDC_SLIDER_VW_SAT_CROP:
		m_pParam->paramVecWnd.fSatCrop = nPos / decimal_point;
		break;
	case IDC_SLIDER_VW_ANGLE:
		m_pParam->paramVecWnd.fAngle = nPos / decimal_point;
		break;
	case IDC_SLIDER_VW_MIX:
		m_pParam->paramYBalMix.fMix = nPos / decimal_point;
		break;
	case IDC_SLIDER_VA_VGAIN:
		m_pParam->paramFGVAdj.fVGain = nPos / decimal_point;
		break;
	case IDC_SLIDER_VA_yGAIN:
		m_pParam->paramFGVAdj.fYGain = nPos / decimal_point;
		break;
	case IDC_SLIDER_VA_CGAIN:
		m_pParam->paramFGVAdj.fCGain = nPos / decimal_point;
		break;
	case IDC_SLIDER_VA_HUE:
		m_pParam->paramFGVAdj.fHue = nPos / decimal_point;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_CHROMA_KEY, (LPARAM)m_pParam);
}

void CParamChromaKeyDlg::OnBnClickedCheckCcOn()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CC_ON);
	m_pParam->paramCc.bOn = pBtn->GetCheck();

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_CHROMA_KEY, (LPARAM)m_pParam);
}

void CParamChromaKeyDlg::OnBnClickedCheckCkInvert()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_CK_INVERT);
	m_pParam->paramCrkAdj.bInvert = pBtn->GetCheck();

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_CHROMA_KEY, (LPARAM)m_pParam);
}

void CParamChromaKeyDlg::OnBnClickedCheckColorKey()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_COLOR_KEY);
	//m_pParam->paramCrkAdj.bOn = pBtn->GetCheck();

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_CHROMA_KEY, (LPARAM)m_pParam);
}

void CParamChromaKeyDlg::OnBnClickedCheckYbOn()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_YB_ON);
	m_pParam->paramYBal.bOn = pBtn->GetCheck();

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_CHROMA_KEY, (LPARAM)m_pParam);
}
