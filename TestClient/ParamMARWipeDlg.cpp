#include "StdAfx.h"
#include "TestClient.h"
#include "EffectName.h"
#include "CommonMessage.h"
#include "ParamMARWipeDlg.h"

#pragma warning(disable:4244)

// CParamMARWipeDlg 对话框

IMPLEMENT_DYNAMIC(CParamMARWipeDlg, CParamBasicWipeDlg)

CParamMARWipeDlg::CParamMARWipeDlg(CWnd* pParent /*=NULL*/)
: _baseClass(FX_MULTI_AXIS_ROTARY_WIPE, pParent)
{
}

CParamMARWipeDlg::~CParamMARWipeDlg(void)
{
}

void CParamMARWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	_baseClass::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamMARWipeDlg, CParamBasicWipeDlg)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CParamMARWipeDlg 消息处理程序

BOOL CParamMARWipeDlg::OnInitDialog()
{
	_baseClass::OnInitDialog();

	BarmWipeFxParam* pParam = (BarmWipeFxParam*)m_pParam;
	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PATTERN);
	pCtrl->SetRange(0, 14);	//14, 0, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.nPattern);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamMARWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	//CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	//int nPos = uPos;
	//if(pCtrl)
	//	nPos = pCtrl->GetPos();

	//switch(pScrollBar->GetDlgCtrlID())
	//{
	//case IDC_SLIDER_SLANT2:
	//	pParam->fSlant = nPos / 1000.f;
	//	break; 
	//}

	_baseClass::OnHScroll(nSBCode, uPos, pScrollBar);
}
