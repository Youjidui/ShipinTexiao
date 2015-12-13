#include "StdAfx.h"
#include "TestClient.h"
#include "ParamMatrixWipeDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)

IMPLEMENT_DYNAMIC(CParamMatrixWipeDlg, CParamBasicWipeDlg)

CParamMatrixWipeDlg::CParamMatrixWipeDlg(CWnd* pParent /*=NULL*/)
: _baseClass(FX_MATRIX_WIPE, pParent)
{
	//SetEffectName(FX_MATRIX_WIPE);
}

CParamMatrixWipeDlg::~CParamMatrixWipeDlg(void)
{
}

void CParamMatrixWipeDlg::DoDataExchange(CDataExchange* pDX)
{
	_baseClass::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CParamMatrixWipeDlg, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CParamMatrixWipeDlg::OnInitDialog()
{
	_baseClass::OnInitDialog();

	MatrixWipeFxParam* pParam = (MatrixWipeFxParam*)m_pParam;
	CSliderCtrl* pCtrl = NULL;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_fSlant);
	pWnd->SetWindowText(_T("fTitleSize"));
	pWnd->ShowWindow(SW_SHOW);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SLANT2);
	pCtrl->ShowWindow(SW_SHOW);
	pCtrl->SetRange(0, 10000);	//1.0f, 0f, 0.0001f
	if(m_pParam)	pCtrl->SetPos(pParam->fTitleSize* 10000);

	pWnd = GetDlgItem(IDC_STATIC_nSequence);
	pWnd->ShowWindow(SW_SHOW);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nSequence);
	pCtrl->ShowWindow(SW_SHOW);
	pCtrl->SetRange(0,5);	//0-5
	if(m_pParam)	pCtrl->SetPos(pParam->nSequence);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamMatrixWipeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	MatrixWipeFxParam* pParam = (MatrixWipeFxParam*)m_pParam;
	//char vColor[4];
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_SLANT2:
		pParam->fTitleSize = nPos / 10000.f;
		break; 
	case IDC_SLIDER_nSequence:
		pParam->nSequence = nPos;
	}

	_baseClass::OnHScroll(nSBCode, uPos, pScrollBar);
}
