// ParamBarmWipeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamBarmWipeDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)

// CParamBarmWipeDlg 对话框

IMPLEMENT_DYNAMIC(CParamBarmWipeDlg, CDialog)

CParamBarmWipeDlg::CParamBarmWipeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamBarmWipeDlg::IDD, pParent)
	, m_pParam(NULL)
{

}

CParamBarmWipeDlg::~CParamBarmWipeDlg()
{
}

void CParamBarmWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamBarmWipeDlg, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CParamBarmWipeDlg 消息处理程序

BOOL CParamBarmWipeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_OFFSET1);		//Progress
	pCtrl->SetRange(0, 10000);	//1.f, 0, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fOffset * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DIVIDE_WIDTH);
	pCtrl->SetRange(2000, 10000);	//1.0f, 0.20f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.fDivideWidth * 10);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DIVIDE_TYPE);
	pCtrl->SetRange(1, 5);	//1800.0f, 0.0f,  0.1f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nDivideType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CENTER_X);
	pCtrl->SetRange(-100000, 100000);	//1.0f, -1.0f, 0.00001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fCenterX * 100000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CENTER_Y);
	pCtrl->SetRange(0, 100000);	//1.0f, 0.0f, 0.00001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fCenterY * 100000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_INVERT);
	pCtrl->SetRange(0, 10000);	//1.0f, 0.0f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.bInvert * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TRANSPARENCY);
	pCtrl->SetRange(0, 100);	//100, 0, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structGeneral.fTransparency);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MULTIPLE_TYPE);
	pCtrl->SetRange(0, 100);	//100, 0, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nMultipleType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MULTIPLE_NUMBER_X);
	pCtrl->SetRange(0, 10000);	//1.0f, 0.0f,  0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nMultipleNumberX * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MULTIPLE_NUMBER_Y);
	pCtrl->SetRange(-36000, 72000);	//720.0f, -360.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nMultipleNumberY * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PATTERN);
	pCtrl->SetRange(-36000, 72000);	//720.0f, -360.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.nPattern * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FLIP);
	pCtrl->SetRange(-36000, 72000);	//720.0f, -360.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.bFlip * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_OVERLAP);
	pCtrl->SetRange(-36000, 72000);	//720.0f, -360.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.bOverlap * 100);
	char vColor[4];
	memcpy(&vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_COLOR_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_COLOR_g);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_COLOR_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	//pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HIGH_COLOR_A);
	//pCtrl->SetRange(0, 255);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_WIDTH);
	pCtrl->SetRange(-36000, 72000);	//720.0f, -360.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fBorderWidth * 100);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SOFT_WIDTH);
	pCtrl->SetRange(-36000, 72000);	//720.0f, -360.0f, 0.01f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fSoftWidth * 100);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamBarmWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	char vColor[4];
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_OFFSET1:		//Progress
		m_pParam->structPattern.fOffset = nPos / 10000.f;
		break;
	case IDC_SLIDER_DIVIDE_TYPE:
		m_pParam->structModify.nDivideType = nPos;
		break;
	case IDC_SLIDER_DIVIDE_WIDTH:
		m_pParam->structModify.fDivideWidth = nPos / 10000.f;
		break;
	case IDC_SLIDER_CENTER_X:
		m_pParam->structPattern.fCenterX = nPos / 100000.f;
		break; 
	case IDC_SLIDER_CENTER_Y:
		m_pParam->structPattern.fCenterY = nPos / 100000.0f;
		break;
	case IDC_SLIDER_INVERT:
		m_pParam->structPattern.bInvert = nPos / 10000.f;
		break;
	case IDC_SLIDER_TRANSPARENCY:
		m_pParam->structGeneral.fTransparency = nPos;
		break;
	case IDC_SLIDER_MULTIPLE_TYPE:
		m_pParam->structModify.nMultipleType = nPos / 10000.f;
		break;
	case IDC_SLIDER_MULTIPLE_NUMBER_X:
		m_pParam->structModify.nMultipleNumberX = nPos / 100.f;
		break;
	case IDC_SLIDER_PATTERN:
		m_pParam->structPattern.nPattern = nPos / 10000.f;
		break;
	case IDC_SLIDER_MULTIPLE_NUMBER_Y:
		m_pParam->structModify.nMultipleNumberY = nPos / 100.f;
		break;
	case IDC_SLIDER_BORDER_COLOR_R:
		memcpy(vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->structPattern.crBorderColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_BORDER_COLOR_g:
		memcpy(vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->structPattern.crBorderColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_BORDER_COLOR_B:
		memcpy(vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->structPattern.crBorderColor, vColor, sizeof(vColor));
		break;
	//case IDC_SLIDER_BORDER_COLOR_A:
	//	memcpy(vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
	//	vColor[3] = nPos;
	//	memcpy(&m_pParam->structPattern.crBorderColor, vColor, sizeof(vColor));
	//	break;
	case IDC_SLIDER_BORDER_WIDTH:
		m_pParam->structPattern.fBorderWidth = nPos / 10000.f;
		break;
	case IDC_SLIDER_SOFT_WIDTH:
		m_pParam->structPattern.fSoftWidth = nPos / 100.f;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_BARM_WIPE, (LPARAM)m_pParam);
}
