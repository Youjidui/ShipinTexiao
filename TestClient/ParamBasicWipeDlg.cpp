#include "StdAfx.h"
#include "TestClient.h"
#include "ParamBasicWipeDlg.h"

#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)

// CParamBasicWipeDlg 对话框

IMPLEMENT_DYNAMIC(CParamBasicWipeDlg, CDialog)

CParamBasicWipeDlg::CParamBasicWipeDlg(LPCTSTR pEffectName, CWnd* pParent /*=NULL*/)
: CDialog(CParamBasicWipeDlg::IDD, pParent)
, m_pParam(NULL)
, m_EffectName(pEffectName)
{
	ASSERT(m_EffectName);
}

CParamBasicWipeDlg::~CParamBasicWipeDlg()
{
	//deleted in CEffectBar::OnDestroy
	//delete m_pParam;
}

void CParamBasicWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamBasicWipeDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_bOverlap, &CParamBasicWipeDlg::OnBnClickedCheckboverlap)
	ON_BN_CLICKED(IDC_CHECK_bInvert, &CParamBasicWipeDlg::OnBnClickedCheckbinvert)
	ON_BN_CLICKED(IDC_CHECK_bFlip, &CParamBasicWipeDlg::OnBnClickedCheckbflip)
END_MESSAGE_MAP()


// CParamBasicWipeDlg 消息处理程序

BOOL CParamBasicWipeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT(m_EffectName);

	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_OFFSET1);		//Progress
	pCtrl->SetRange(0, 10000, TRUE);	//1.f, 0, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fOffset * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DIVIDE_WIDTH);
	pCtrl->SetRange(0, 10000, TRUE);	//1.f, 0, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.fDivideWidth * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_DIVIDE_TYPE);
	pCtrl->SetRange(1, 5, TRUE);	//5, 1,  1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nDivideType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CENTER_X);
	pCtrl->SetRange(-20000, 20000, TRUE);	//2.0f, -2.0f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fCenterX * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CENTER_Y);
	pCtrl->SetRange(-20000, 20000, TRUE);	//2.0f, -2.0f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fCenterY * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_TRANSPARENCY);
	pCtrl->SetRange(0, 10000);	//1.0f, 0.0f,  0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structGeneral.fTransparency * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MULTIPLE_TYPE);
	pCtrl->SetRange(0, 3);	//3, 0, 1	??
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nMultipleType);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MULTIPLE_NUMBER_X);
	pCtrl->SetRange(1, 20, TRUE);	//20, 1, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nMultipleNumberX);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MULTIPLE_NUMBER_Y);
	pCtrl->SetRange(1, 20, TRUE);	//20, 1, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structModify.nMultipleNumberY);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PATTERN);
	pCtrl->SetRange(0, 5);	//5, 0, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.nPattern);

	char vColor[4];	//D3DCOLOR_XRGB
	memcpy(&vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_COLOR_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_COLOR_g);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_COLOR_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BORDER_WIDTH);
	pCtrl->SetRange(0, 50000);	//5.0f, 0.0f,  0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fBorderWidth * 10000);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SOFT_WIDTH);
	pCtrl->SetRange(0, 50000);	//5.0f, 0.0f,  0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.fSoftWidth * 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamBasicWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
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
		m_pParam->structPattern.fCenterX = nPos / 10000.f;
		break; 
	case IDC_SLIDER_CENTER_Y:
		m_pParam->structPattern.fCenterY = nPos / 10000.0f;
		break;
	case IDC_SLIDER_TRANSPARENCY:
		m_pParam->structGeneral.fTransparency = nPos / 10000.0f;
		break;
	case IDC_SLIDER_MULTIPLE_TYPE:
		m_pParam->structModify.nMultipleType = nPos;
		break;
	case IDC_SLIDER_MULTIPLE_NUMBER_X:
		m_pParam->structModify.nMultipleNumberX = nPos;
		break;
	case IDC_SLIDER_PATTERN:
		m_pParam->structPattern.nPattern = nPos;
		break;
	case IDC_SLIDER_MULTIPLE_NUMBER_Y:
		m_pParam->structModify.nMultipleNumberY = nPos;
		break;
	case IDC_SLIDER_BORDER_COLOR_R:
		memcpy(vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->structPattern.crBorderColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_BORDER_COLOR_g:
		memcpy(vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->structPattern.crBorderColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_BORDER_COLOR_B:
		memcpy(vColor, &m_pParam->structPattern.crBorderColor, sizeof(vColor));
		vColor[0] = nPos;
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
		m_pParam->structPattern.fSoftWidth = nPos / 10000.f;
		break;
	}

	ASSERT(m_EffectName);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)m_EffectName, (LPARAM)m_pParam);
}

void CParamBasicWipeDlg::OnBnClickedCheckboverlap()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bOverlap);
	m_pParam->structModify.bOverlap = pBtn->GetCheck();
	ASSERT(m_EffectName);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)m_EffectName, (LPARAM)m_pParam);
}

void CParamBasicWipeDlg::OnBnClickedCheckbinvert()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bInvert);
	m_pParam->structPattern.bInvert = pBtn->GetCheck();
	ASSERT(m_EffectName);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)m_EffectName, (LPARAM)m_pParam);
}

void CParamBasicWipeDlg::OnBnClickedCheckbflip()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bFlip);
	m_pParam->structModify.bFlip = pBtn->GetCheck();
	ASSERT(m_EffectName);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)m_EffectName, (LPARAM)m_pParam);
}
