// EffectBar.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "EffectBar.h"
#include "EffectName.h"
#include "CommonMessage.h"
#include "../FxParam.h"

// CEffectBar 对话框

IMPLEMENT_DYNAMIC(CEffectBar, CDialog)

CEffectBar::CEffectBar(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectBar::IDD, pParent)
{

}

CEffectBar::~CEffectBar()
{
}

void CEffectBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EFFECTS, m_ctrlEffects);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(CEffectBar, CDialog)
	ON_BN_CLICKED(IDC_PARAMETERS, &CEffectBar::OnBnClickedParameters)
	ON_CBN_SELCHANGE(IDC_EFFECTS, &CEffectBar::OnCbnSelchangeEffects)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CEffectBar 消息处理程序

void CEffectBar::OnBnClickedParameters()
{
	// TODO: 在此添加控件通知处理程序代码
	//根据选择的特效，弹出其参数对话框
	int nSel = m_ctrlEffects.GetCurSel();
	if(LB_ERR != nSel)
	{
		CString str;
		m_ctrlEffects.GetLBText(nSel, str);
		if(FX_NEGATIVE == str)
		{

		}
		else if(FX_COLOR_KEY == str)
		{
			if(!m_colorKeyDlg.GetSafeHwnd())
			{
				m_colorKeyDlg.Create(CParamColorKeyDlg::IDD);
			}
			m_colorKeyDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_SONY_BLUR == str)
		{
			if(!m_blurDlg.GetSafeHwnd())
			{
				m_blurDlg.Create(CParamBlurDlg::IDD);
			}
			m_blurDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_AMOEBA_WIPE == str)
		{
			if(!m_amoebaWipeDlg.GetSafeHwnd())
			{
				m_amoebaWipeDlg.Create(CParamAmoebaWipeDlg::IDD);
			}
			m_amoebaWipeDlg.ShowWindow(SW_SHOW);
		}
	}
}

BOOL CEffectBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//m_ctrlEffects.AddString(_T("none"));
	int i = m_ctrlEffects.AddString(FX_NEGATIVE);
	//m_ctrlEffects.SetItemData(i, FX_NEGATIVE);
	NegativeFxParam* pN = new NegativeFxParam;
	m_ctrlEffects.SetItemDataPtr(i, pN);
	i = m_ctrlEffects.AddString(FX_COLOR_KEY);
	{
		ColorKeyParam* pCK = new ColorKeyParam;
		pCK->fKeyColor[0] = 0.8f;
		pCK->fKeyColor[1] = 0.8f;
		//param.fKeyColor[2] = 0.8f;
		pCK->fAngle1 = 1.0f;
		pCK->fAngle2 = 2.0f;
		pCK->fLength1 = 0.2f;
		pCK->fLength2 = 0.45f;
		m_ctrlEffects.SetItemDataPtr(i, pCK);
		m_colorKeyDlg.SetParam(pCK);
	}
	i = m_ctrlEffects.AddString(FX_SONY_BLUR);
	{
		SonyBlurFxParam* pSB = new SonyBlurFxParam;
		pSB->blurX = 41.0f;	pSB->blurY = 29.9f;
		m_ctrlEffects.SetItemDataPtr(i, pSB);
		m_blurDlg.SetParam(pSB);
	}
	i = m_ctrlEffects.AddString(FX_AMOEBA_WIPE);
	{
		AmoebaWipeFxParam* pAW = new AmoebaWipeFxParam;
		ZeroMemory(pAW, sizeof(AmoebaWipeFxParam));
		pAW->cbSize = sizeof(AmoebaWipeFxParam);
		pAW->fBumpDensity = 0.25f;
		pAW->fHeight = 0.5f;
		pAW->fSoftEdge = 0.05f;
		pAW->fBrightness = 0.2f;
		pAW->fLightAngle = 135.f;
		pAW->fOffset = 0.5f;
		m_ctrlEffects.SetItemDataPtr(i, pAW);
		m_amoebaWipeDlg.SetParam(pAW);
	}

	i = m_ctrlEffects.AddString(FX_PUSH);
	{
		PushFxParam* pPushFxParam = new PushFxParam;
		m_ctrlEffects.SetItemDataPtr(i, pPushFxParam);
	}


	m_ctrlEffects.SetCurSel(0);

	//ON_CONTROL(CBN_SELCHANGE, IDC_EFFECTS
	PostMessage(WM_COMMAND, MAKELONG(IDC_EFFECTS, CBN_SELCHANGE), (LPARAM)m_ctrlEffects.GetSafeHwnd());

	m_ctrlProgress.SetRange(0, 10000);	//1.f, 0, 0.0001f
	SetProgress(&m_ctrlProgress);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEffectBar::OnCbnSelchangeEffects()
{
	int nSel = m_ctrlEffects.GetCurSel();
	if(LB_ERR != nSel)
	{
		CString str;
		m_ctrlEffects.GetLBText(nSel, str);
		void* pParam = m_ctrlEffects.GetItemDataPtr(nSel);
		AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
	}
}

void CEffectBar::OnDestroy()
{
	int n = m_ctrlEffects.GetCount();
	for(int i = 0; i < n; ++i)
	{
		void* pParam = m_ctrlEffects.GetItemDataPtr(i);
		delete pParam;
	}

	CDialog::OnDestroy();
}

void CEffectBar::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	OnProgressChange(nPos);
}

void CEffectBar::OnProgressChange( int nPos )
{
	int nSel = m_ctrlEffects.GetCurSel();
	if(LB_ERR != nSel)
	{
		CString str;
		m_ctrlEffects.GetLBText(nSel, str);
		if(FX_AMOEBA_WIPE == str)
		{
			AmoebaWipeFxParam* pParam = (AmoebaWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			//pParam->fProgress = nPos / 100.f;
			pParam->fOffset = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
	}
}

void CEffectBar::SetProgress(CSliderCtrl* pCtrl)
{
	int nSel = m_ctrlEffects.GetCurSel();
	if(LB_ERR != nSel)
	{
		CString str;
		m_ctrlEffects.GetLBText(nSel, str);
		if(FX_AMOEBA_WIPE == str)
		{
			AmoebaWipeFxParam* pParam = (AmoebaWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fOffset * 10000);
		}
	}
}
