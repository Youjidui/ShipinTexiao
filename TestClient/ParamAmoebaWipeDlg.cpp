// ParamAmoebaWipeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamAmoebaWipeDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"


const float MaxValue = 256.0f;


// CParamAmoebaWipeDlg 对话框

IMPLEMENT_DYNAMIC(CParamAmoebaWipeDlg, CDialog)

CParamAmoebaWipeDlg::CParamAmoebaWipeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamAmoebaWipeDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CParamAmoebaWipeDlg::~CParamAmoebaWipeDlg()
{
}

void CParamAmoebaWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamAmoebaWipeDlg, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CParamAmoebaWipeDlg 消息处理程序

BOOL CParamAmoebaWipeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PROGRESS);
	pCtrl->SetRange(0, 100);	//1.0f, 0.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fProgress * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PHASE);
	pCtrl->SetRange(0, 18000);	//1800.0f, 0.0f,  0.1f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fPhase * 10);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BUMP_DENSITY);
	pCtrl->SetRange(2000, 10000);	//1.0f, 0.20f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBumpDensity * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SLANT);
	pCtrl->SetRange(-100000, 100000);	//1.0f, -1.0f, 0.00001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fSlant * 100000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);
	pCtrl->SetRange(0, 100000);	//1.0f, 0.0f, 0.00001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fHeight * 100000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SOFT_EDGE);
	pCtrl->SetRange(0, 10000);	//1.0f, 0.0f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fSoftEdge * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_RANDOM_SEED);
	pCtrl->SetRange(0, 100);	//100, 0, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->nRandomSeed);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
	pCtrl->SetRange(0, 10000);	//1.0f, 0.0f,  0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBrightness * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHT_ANGLE);
	pCtrl->SetRange(-36000, 72000);	//720.0f, -360.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fLightAngle * 100);
	char vColor[4];
	memcpy(&vColor, &m_pParam->crHColor, sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HIGH_COLOR_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HIGH_COLOR_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HIGH_COLOR_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HIGH_COLOR_A);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[3]);
	memcpy(&vColor, &m_pParam->crLColor, sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LOW_COLOR_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LOW_COLOR_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LOW_COLOR_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LOW_COLOR_A);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[3]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_OFFSET);
	pCtrl->SetRange(0, 10000);	//1.f, 0, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fOffset * 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamAmoebaWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	char vColor[4];
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_PROGRESS:
		m_pParam->fProgress = nPos / 100.f;
		break;
	case IDC_SLIDER_PHASE:
		m_pParam->fPhase = nPos / 10.f;
		break;
	case IDC_SLIDER_BUMP_DENSITY:
		m_pParam->fBumpDensity = nPos / 10000.f;
		break;
	case IDC_SLIDER_SLANT:
		m_pParam->fSlant = nPos / 100000.f;
		break; 
	case IDC_SLIDER_HEIGHT:
		m_pParam->fHeight = nPos / 100000.0f;
		break;
	case IDC_SLIDER_SOFT_EDGE:
		m_pParam->fSoftEdge = nPos / 10000.f;
		break;
	case IDC_SLIDER_RANDOM_SEED:
		m_pParam->nRandomSeed = nPos;
		break;
	case IDC_SLIDER_BRIGHTNESS:
		m_pParam->fBrightness = nPos / 10000.f;
		break;
	case IDC_SLIDER_LIGHT_ANGLE:
		m_pParam->fLightAngle = nPos / 100.f;
		break;
	case IDC_SLIDER_HIGH_COLOR_R:
		memcpy(vColor, &m_pParam->crHColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->crHColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_HIGH_COLOR_G:
		memcpy(vColor, &m_pParam->crHColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->crHColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_HIGH_COLOR_B:
		memcpy(vColor, &m_pParam->crHColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->crHColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_HIGH_COLOR_A:
		memcpy(vColor, &m_pParam->crHColor, sizeof(vColor));
		vColor[3] = nPos;
		memcpy(&m_pParam->crHColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_LOW_COLOR_R:
		memcpy(vColor, &m_pParam->crLColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->crLColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_LOW_COLOR_G:
		memcpy(vColor, &m_pParam->crLColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->crLColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_LOW_COLOR_B:
		memcpy(vColor, &m_pParam->crLColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->crLColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_LOW_COLOR_A:
		memcpy(vColor, &m_pParam->crLColor, sizeof(vColor));
		vColor[3] = nPos;
		memcpy(&m_pParam->crLColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_OFFSET:
		m_pParam->fOffset = nPos / 10000.f;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_AMOEBA_WIPE, (LPARAM)m_pParam);
}
