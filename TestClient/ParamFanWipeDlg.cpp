#include "StdAfx.h"
#include "TestClient.h"
#include "EffectName.h"
#include "CommonMessage.h"
#include "ParamFanWipeDlg.h"

#pragma warning(disable:4244)

// CParamFanWipeDlg 对话框

IMPLEMENT_DYNAMIC(CParamFanWipeDlg, CParamBasicWipeDlg)

CParamFanWipeDlg::CParamFanWipeDlg(CWnd* pParent /*=NULL*/)
: _baseClass(FX_FAN_WIPE, pParent)
{
}

CParamFanWipeDlg::~CParamFanWipeDlg(void)
{
}

void CParamFanWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	_baseClass::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamFanWipeDlg, CParamBasicWipeDlg)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CParamFanWipeDlg 消息处理程序

BOOL CParamFanWipeDlg::OnInitDialog()
{
	_baseClass::OnInitDialog();

	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PATTERN);
	pCtrl->SetRange(0, 13);	//13, 0, 1
	if(m_pParam)	pCtrl->SetPos(m_pParam->structPattern.nPattern);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamFanWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
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

