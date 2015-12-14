// ParamQuadPageRollDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamQuadPageRollDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable: 4244)

// CParamQuadPageRollDlg 对话框

IMPLEMENT_DYNAMIC(CParamQuadPageRollDlg, CDialog)

CParamQuadPageRollDlg::CParamQuadPageRollDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamQuadPageRollDlg::IDD, pParent)
{

}

CParamQuadPageRollDlg::~CParamQuadPageRollDlg()
{
}

void CParamQuadPageRollDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamQuadPageRollDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_REVERSE, &CParamQuadPageRollDlg::OnBnClickedCheckReverse)
	ON_BN_CLICKED(IDC_CHECK_USE_FORE_GROUND, &CParamQuadPageRollDlg::OnBnClickedCheckUseForeGround)
END_MESSAGE_MAP()


// CParamQuadPageRollDlg 消息处理程序
BOOL CParamQuadPageRollDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TRANSITION);		//Progress
	pCtrl->SetRange(0, 10000);	//1.f, 0, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structTrans.fTransition * 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE_LT);
	pCtrl->SetRange(-36000, 72000);	//720.f, 360, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAngle[0] * 100);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE_RT);
	pCtrl->SetRange(-36000, 72000);	//720.f, 360, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAngle[1] * 100);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE_RB);
	pCtrl->SetRange(-36000, 72000);	//720.f, 360, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAngle[2] * 100);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE_LB);
	pCtrl->SetRange(-36000, 72000);	//720.f, 360, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fAngle[3] * 100);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_STEP_PATTERN);
	pCtrl->SetRange(0, 2);
	if(m_pParam)	pCtrl->SetPos(m_pParam->nStepPattern);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GROUP_PATTERN);
	pCtrl->SetRange(-36000, 72000);	//720.f, 360, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->nGroupPattern);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_RADIUS);
	pCtrl->SetRange(1000, 10000);	//1.0f, 0.1f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structGeometry.fRadius * 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE_PHI);
	pCtrl->SetRange(0, 360000);	////360.0f, 0.0f,  0.001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structLight.fAnglePhi * 1000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_ANGLE_THETA);
	pCtrl->SetRange(0, 36000);	////360.0f, 0.0f,  0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structLight.fAngleTheta * 100);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHT_RATIO);
	pCtrl->SetRange(0, 10000);	//1.0f, 0.0f,  0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structLight.fLightRatio * 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FRONT_HIGH_LIGHT);
	pCtrl->SetRange(0, 20000);	//200.0f, 0.0f,  0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structLight.fFrontHighLight * 100);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FRONT_SMOOTHNESS);
	pCtrl->SetRange(0, 20000);	//20.0f, 0.0f,  0.001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structLight.fFrontSmoothness * 1000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_REAR_HIGH_LIGHT);
	pCtrl->SetRange(0, 20000);	//200.0f, 0.0f,  0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structLight.fRearHighLight * 100);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_REAR_SMOOTHNESS);
	pCtrl->SetRange(0, 20000);	//20.0f, 0.0f,  0.001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structLight.fRearSmoothness * 1000);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_USE_FORE_GROUND);
	if(m_pParam)	pBtn->SetCheck(m_pParam->structRear.bUseForeGround);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MATTE_RATIO);
	pCtrl->SetRange(0, 10000);	//1.0f, 0.0f,  0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structRear.fMatteRatio * 10000);

	char vColor[4];
	if(m_pParam)
		memcpy(vColor, &(m_pParam->structRear.crMaatte), sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MATTE_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MATTE_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MATTE_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);


	pBtn = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	if(m_pParam)	pBtn->SetCheck(m_pParam->structTrans.bReverse);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamQuadPageRollDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	char vColor[4];
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_TRANSITION:		//Progress
		m_pParam->structTrans.fTransition = nPos / 10000.f;
		break;
	case IDC_SLIDER_ANGLE_LT:
		m_pParam->fAngle[0] = nPos / 100.f;
		break;
	case IDC_SLIDER_ANGLE_RT:
		m_pParam->fAngle[1] = nPos / 100.f;
		break;
	case IDC_SLIDER_ANGLE_RB:
		m_pParam->fAngle[2] = nPos / 100.f;
		break;
	case IDC_SLIDER_ANGLE_LB:
		m_pParam->fAngle[3] = nPos / 100.f;
		break;
	case IDC_SLIDER_STEP_PATTERN:
		m_pParam->nStepPattern = nPos;
		break;
	case IDC_SLIDER_GROUP_PATTERN:
		m_pParam->nGroupPattern = nPos;
		break;
	case IDC_SLIDER_RADIUS:
		m_pParam->structGeometry.fRadius = nPos / 10000.f;
		break;
	case IDC_SLIDER_ANGLE_PHI:
		m_pParam->structLight.fAnglePhi = nPos / 1000.f;
		break; 
	case IDC_SLIDER_ANGLE_THETA:
		m_pParam->structLight.fAngleTheta = nPos / 100.0f;
		break;
	case IDC_SLIDER_LIGHT_RATIO:
		m_pParam->structLight.fLightRatio = nPos / 10000.f;
		break;
	case IDC_SLIDER_FRONT_HIGH_LIGHT:
		m_pParam->structLight.fFrontHighLight = nPos / 100.0f;
		break;
	case IDC_SLIDER_FRONT_SMOOTHNESS:
		m_pParam->structLight.fFrontSmoothness = nPos / 1000.f;
		break;
	case IDC_SLIDER_REAR_HIGH_LIGHT:
		m_pParam->structLight.fRearHighLight = nPos / 100.f;
		break;
	case IDC_SLIDER_REAR_SMOOTHNESS:
		m_pParam->structLight.fRearSmoothness = nPos / 1000.f;
		break;
	case IDC_SLIDER_MATTE_RATIO:
		m_pParam->structRear.fMatteRatio = nPos / 10000.f;
		break; 
	case IDC_SLIDER_MATTE_R:
		memcpy(vColor, &m_pParam->structRear.crMaatte, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->structRear.crMaatte, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_MATTE_G:
		memcpy(vColor, &m_pParam->structRear.crMaatte, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->structRear.crMaatte, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_MATTE_B:
		memcpy(vColor, &m_pParam->structRear.crMaatte, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->structRear.crMaatte, vColor, sizeof(vColor));
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_QUAD_PAGE_ROLL, (LPARAM)m_pParam);
}

void CParamQuadPageRollDlg::OnBnClickedCheckReverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_REVERSE);
	if(m_pParam)	m_pParam->structTrans.bReverse = (pBtn->GetCheck() == BST_CHECKED);
}

void CParamQuadPageRollDlg::OnBnClickedCheckUseForeGround()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_USE_FORE_GROUND);
	if(m_pParam)	m_pParam->structRear.bUseForeGround = (pBtn->GetCheck() == BST_CHECKED);
}
