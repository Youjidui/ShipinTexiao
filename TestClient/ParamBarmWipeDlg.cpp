// ParamBarmWipeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamBarmWipeDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)

// CParamBarmWipeDlg 对话框

IMPLEMENT_DYNAMIC(CParamBarmWipeDlg, CParamBasicWipeDlg)

CParamBarmWipeDlg::CParamBarmWipeDlg(CWnd* pParent /*=NULL*/)
	: _baseClass(FX_BARM_WIPE, pParent)
{
}

CParamBarmWipeDlg::~CParamBarmWipeDlg()
{
}

void CParamBarmWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	_baseClass::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamBarmWipeDlg, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CParamBarmWipeDlg 消息处理程序

BOOL CParamBarmWipeDlg::OnInitDialog()
{
	_baseClass::OnInitDialog();

	BarmWipeFxParam* pParam = (BarmWipeFxParam*)m_pParam;
	CSliderCtrl* pCtrl = NULL;

	CWnd* pWnd = GetDlgItem(IDC_STATIC_fSlant);
	pWnd->ShowWindow(SW_SHOW);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SLANT2);
	pCtrl->ShowWindow(SW_SHOW);
	pCtrl->SetRange(-45*1000, 45*1000);
	if(m_pParam)	pCtrl->SetPos(pParam->fSlant * 1000);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamBarmWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	BarmWipeFxParam* pParam = (BarmWipeFxParam*)m_pParam;
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_SLANT2:
		pParam->fSlant = nPos / 1000.f;
		break; 
	}

	_baseClass::OnHScroll(nSBCode, uPos, pScrollBar);
}
