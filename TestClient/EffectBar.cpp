// EffectBar.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "EffectBar.h"
#include "EffectName.h"
#include "CommonMessage.h"
#include "../FxParam.h"

#pragma warning(disable:4244)

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
		else if(FX_BARM_WIPE == str)
		{
			if(!m_barmWipeDlg.GetSafeHwnd())
			{
				m_barmWipeDlg.Create(CParamBarmWipeDlg::IDD);
			}
			m_barmWipeDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_FAN_WIPE == str)
		{
			if(!m_fanWipeDlg.GetSafeHwnd())
			{
				m_fanWipeDlg.Create(CParamFanWipeDlg::IDD);
			}
			m_fanWipeDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_MULTI_AXIS_ROTARY_WIPE == str)
		{
			if(!m_marWipeDlg.GetSafeHwnd())
			{
				m_marWipeDlg.Create(CParamMARWipeDlg::IDD);
			}
			m_marWipeDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_MATRIX_WIPE == str)
		{
			if(!m_matrixWipeDlg.GetSafeHwnd())
			{
				m_matrixWipeDlg.Create(CParamMatrixWipeDlg::IDD);
			}
			m_matrixWipeDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_REVOLVING_WIPE == str)
		{
			if(!m_revolvingWipeDlg.GetSafeHwnd())
			{
				m_revolvingWipeDlg.Create(CParamRevolvingWipeDlg::IDD);
			}
			m_revolvingWipeDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_RIPPLE_WIPE == str)
		{
			if(!m_rippleWipeDlg.GetSafeHwnd())
			{
				m_rippleWipeDlg.Create(m_rippleWipeDlg.IDD);
			}
			m_rippleWipeDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_SONY_BARN_SLIDE == str)
		{
			if(!m_barnSlideDlg.GetSafeHwnd())
			{
				m_barnSlideDlg.Create(m_barnSlideDlg.IDD);
			}
			m_barnSlideDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_BROKEN_GLASS == str)
		{
			if(!m_brokenGlassDlg.GetSafeHwnd())
			{
				m_brokenGlassDlg.Create(m_brokenGlassDlg.IDD);
			}
			m_brokenGlassDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_RINGS == str)
		{
			if(!m_ringsDlg.GetSafeHwnd())
			{
				m_ringsDlg.Create(m_ringsDlg.IDD);
			}
			m_ringsDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_DISSOLVE == str)
		{
			if(!m_dissolveDlg.GetSafeHwnd())
			{
				m_dissolveDlg.Create(m_dissolveDlg.IDD);
			}
			m_dissolveDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_FADE_FROM_TO == str)
		{
			if(!m_fadeFromToDlg.GetSafeHwnd())
			{
				m_fadeFromToDlg.Create(m_fadeFromToDlg.IDD);
			}
			m_fadeFromToDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_PAGE_ROTATION == str)
		{
			if(!m_pageRotationDlg.GetSafeHwnd())
			{
				m_pageRotationDlg.Create(m_pageRotationDlg.IDD);
			}
			m_pageRotationDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_PUSH == str)
		{
			if(!m_pushDlg.GetSafeHwnd())
			{
				m_pushDlg.Create(CParamPushDlg::IDD);
			}
			m_pushDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_SONY_SLIDE == str)
		{
			if (!m_SonySlideDlg.GetSafeHwnd())
			{
				m_SonySlideDlg.Create(m_SonySlideDlg.IDD);
			}
			m_SonySlideDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_CHROMA_KEY == str)
		{
			if(!m_chromaKeyDlg.GetSafeHwnd())
			{
				m_chromaKeyDlg.Create(m_chromaKeyDlg.IDD);
			}
			m_chromaKeyDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_PAGE_ROLL == str)
		{
			if(!m_PageRollDlg.GetSafeHwnd())
			{
				m_PageRollDlg.Create(m_PageRollDlg.IDD);
			}
			m_PageRollDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_DUO_PAGE_ROLL == str)
		{
			if(!m_duoPageRollDlg.GetSafeHwnd())
			{
				m_duoPageRollDlg.Create(m_duoPageRollDlg.IDD);
			}
			m_duoPageRollDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_QUAD_PAGE_ROLL == str)
		{
			if(!m_QuadPageRollDlg.GetSafeHwnd())
			{
				m_QuadPageRollDlg.Create(m_QuadPageRollDlg.IDD);
			}
			m_QuadPageRollDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_CUBE_TRANS == str)
		{
			if(!m_cubeTransDlg.GetSafeHwnd())
			{
				m_cubeTransDlg.Create(m_cubeTransDlg.IDD);
			}
			m_cubeTransDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_SONY_DME_3D_TRANSFORM == str)
		{
			if(!m_sonyDME3DDlg.GetSafeHwnd())
			{
				m_sonyDME3DDlg.Create(m_sonyDME3DDlg.IDD);
			}
			m_sonyDME3DDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_SONY_PINP == str)
		{
			if(!m_sonyPinpDlg.GetSafeHwnd())
			{
				m_sonyPinpDlg.Create(m_sonyPinpDlg.IDD);
			}
			m_sonyPinpDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_SONY_3D_BROKEN_GLASS == str)
		{
			if(!m_sony3DBrokenGlassDlg.GetSafeHwnd())
			{
				m_sony3DBrokenGlassDlg.Create(m_sony3DBrokenGlassDlg.IDD);
			}
			m_sony3DBrokenGlassDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_DIP_TO_COLOR == str)
		{
			if(!m_dipToColorDlg.GetSafeHwnd())
			{
				m_dipToColorDlg.Create(m_dipToColorDlg.IDD);
			}
			m_dipToColorDlg.ShowWindow(SW_SHOW);
		}
	}
}

BOOL CEffectBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_ctrlEffects.AddString(_T("none"));
	int i = m_ctrlEffects.AddString(FX_NEGATIVE);
	//m_ctrlEffects.SetItemData(i, FX_NEGATIVE);
	FxParamBase* pN = new FxParamBase;
	m_ctrlEffects.SetItemDataPtr(i, pN);
	//i = m_ctrlEffects.AddString(FX_COLOR_KEY);
	//{
	//	ColorKeyParam* pCK = new ColorKeyParam;
	//	pCK->fKeyColor[0] = 0.8f;
	//	pCK->fKeyColor[1] = 0.8f;
	//	//param.fKeyColor[2] = 0.8f;
	//	pCK->fAngle1 = 1.0f;
	//	pCK->fAngle2 = 2.0f;
	//	pCK->fLength1 = 0.2f;
	//	pCK->fLength2 = 0.45f;
	//	m_ctrlEffects.SetItemDataPtr(i, pCK);
	//	m_colorKeyDlg.SetParam(pCK);
	//}
	//i = m_ctrlEffects.AddString(FX_SONY_BLUR);
	//{
	//	SonyBlurFxParam* pSB = new SonyBlurFxParam;
	//	pSB->blurX = 41.0f;	pSB->blurY = 29.9f;
	//	m_ctrlEffects.SetItemDataPtr(i, pSB);
	//	m_blurDlg.SetParam(pSB);
	//}
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
		pPushFxParam->fTransition = 0.5f;
		pPushFxParam->dwType = 0;
		pPushFxParam->bReverse = false;
		pPushFxParam->fFirstSoftness = 0.f;
		pPushFxParam->fSecondSoftness = 0.f;
		pPushFxParam->dwFirstColor = 0xFFFFFFFF;
		pPushFxParam->dwSecondColor = 0xFFFFFFFF;
		m_ctrlEffects.SetItemDataPtr(i, pPushFxParam);
		m_pushDlg.SetParam(pPushFxParam);
	}
	i = m_ctrlEffects.AddString(FX_BARM_WIPE);
	{
		BarmWipeFxParam* pParam = new BarmWipeFxParam;
		pParam->cbSize = sizeof(BarmWipeFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_barmWipeDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_FAN_WIPE);
	{
		BasicWipeFxParam* pParam = new BasicWipeFxParam;
		pParam->cbSize = sizeof(BasicWipeFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_fanWipeDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_MULTI_AXIS_ROTARY_WIPE);
	{
		BasicWipeFxParam* pParam = new BasicWipeFxParam;
		pParam->cbSize = sizeof(BasicWipeFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_marWipeDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_MATRIX_WIPE);
	{
		MatrixWipeFxParam* pParam = new MatrixWipeFxParam;
		pParam->cbSize = sizeof(MatrixWipeFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_matrixWipeDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_REVOLVING_WIPE);
	{
		MatrixWipeFxParam* pParam = new MatrixWipeFxParam;
		pParam->cbSize = sizeof(MatrixWipeFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_revolvingWipeDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_RIPPLE_WIPE);
	{
		RippleWipeFxParam* pParam = new RippleWipeFxParam;
		pParam->cbSize = sizeof(RippleWipeFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_rippleWipeDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_BROKEN_GLASS);
	{
		BrokenGlassFxParam* pParam = new BrokenGlassFxParam;
		pParam->cbSize = sizeof(BrokenGlassFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_brokenGlassDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_RINGS);
	{
		RingsFxParam* pParam = new RingsFxParam;
		pParam->cbSize = sizeof(RingsFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_ringsDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_DISSOLVE);
	{
		DissolveFxParam* pParam = new DissolveFxParam;
		pParam->cbSize = sizeof(DissolveFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_dissolveDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_PAGE_ROTATION);
	{
		PageRotationFxParam* pParam = new PageRotationFxParam;
		pParam->cbSize = sizeof(PageRotationFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_pageRotationDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_FADE_FROM_TO);
	{
		SonyFadeFromToFxParam* pParam = new SonyFadeFromToFxParam;
		pParam->cbSize = sizeof(SonyFadeFromToFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_fadeFromToDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_SONY_BARN_SLIDE);
	{
		SonyBarnSlideFxParam* pParam = new SonyBarnSlideFxParam;
		pParam->cbSize = sizeof(SonyBarnSlideFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_barnSlideDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_SONY_SLIDE);
	{
		SonySlideFxParam* pParam = new SonySlideFxParam;
		ZeroMemory(pParam, sizeof(SonySlideFxParam));
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_SonySlideDlg.SetParam(pParam);
	}
	//i = m_ctrlEffects.AddString(FX_SONY_MASK);
	//{
	//	SonyMaskFxParam* p = new SonyMaskFxParam;
	//	m_ctrlEffects.SetItemDataPtr(i, p);
	//	//m_blurDlg.SetParam(p);
	//}
	i = m_ctrlEffects.AddString(FX_CHROMA_KEY);
	{
		ChromaKeyFxParam * p = new ChromaKeyFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_chromaKeyDlg.SetParam(p);
	}
	i = m_ctrlEffects.AddString(FX_PAGE_ROLL);
	{
		PageRollFxParam* p = new PageRollFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_PageRollDlg.SetParam(p);
	}
	i = m_ctrlEffects.AddString(FX_SONY_DME_3D_TRANSFORM);
	{
		SonyDME3DTransfromFxPrarm* p = new SonyDME3DTransfromFxPrarm;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_sonyDME3DDlg.SetParam(p);

	}
	i = m_ctrlEffects.AddString(FX_DUO_PAGE_ROLL);
	{
		DuoPageRollFxParam* p = new DuoPageRollFxParam;
		p->cbSize = sizeof(DuoPageRollFxParam);
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_duoPageRollDlg.SetParam(p);
	}
	i = m_ctrlEffects.AddString(FX_QUAD_PAGE_ROLL);
	{
		QuadPageRollFxParam* p = new QuadPageRollFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_QuadPageRollDlg.SetParam(p);
	}
	i = m_ctrlEffects.AddString(FX_CUBE_TRANS);
	{
		CubeFxParam* p = new CubeFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_cubeTransDlg.SetParam(p);
	}
	i = m_ctrlEffects.AddString(FX_SONY_PINP);
	{
		SonyPinPFxParam* p = new SonyPinPFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_sonyPinpDlg.SetParam(p);
	}
	i = m_ctrlEffects.AddString(FX_SONY_3D_BROKEN_GLASS);
	{
		Sony3DBrokenGlassFxParam* p = new Sony3DBrokenGlassFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_sony3DBrokenGlassDlg.SetParam(p);

	}
	i = m_ctrlEffects.AddString(FX_DIP_TO_COLOR);
	{
		DipToColorTransFxParam* p = new DipToColorTransFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		m_dipToColorDlg.SetParam(p);

	}

	m_ctrlEffects.SetCurSel(i);

	m_ctrlProgress.SetRange(0, 10000);	//1.f, 0, 0.0001f

	//ON_CONTROL(CBN_SELCHANGE, IDC_EFFECTS
	PostMessage(WM_COMMAND, MAKELONG(IDC_EFFECTS, CBN_SELCHANGE), (LPARAM)m_ctrlEffects.GetSafeHwnd());

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
		SetProgress(&m_ctrlProgress);
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
		if(FX_BARM_WIPE == str || FX_FAN_WIPE == str || FX_MULTI_AXIS_ROTARY_WIPE == str || FX_MATRIX_WIPE == str || FX_REVOLVING_WIPE == str)
		{
			BasicWipeFxParam* pParam = (BasicWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->structPattern.fOffset = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_RIPPLE_WIPE== str)
		{
			RippleWipeFxParam* pParam = (RippleWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->transParam.progress = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_BROKEN_GLASS == str)
		{
			BrokenGlassFxParam* pParam = (BrokenGlassFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->prm_movement = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_RINGS == str)
		{
			RingsFxParam* pParam = (RingsFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->fTranslate = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_DISSOLVE == str)
		{
			DissolveFxParam* pParam = (DissolveFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->prm_process = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_PAGE_ROTATION == str)
		{
			PageRotationFxParam* pParam = (PageRotationFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->structTrans.fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_FADE_FROM_TO == str)
		{
			SonyFadeFromToFxParam* pParam = (SonyFadeFromToFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_PUSH == str)
		{
			PushFxParam* pParam = (PushFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_SONY_SLIDE == str)
		{
			SonySlideFxParam* pParam = (SonySlideFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_PAGE_ROLL == str)
		{
			PageRollFxParam* pParam = (PageRollFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->structTrans.fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}

		else if (FX_SONY_DME_3D_TRANSFORM == str)
		{
			SonyDME3DTransfromFxPrarm* pParam = (SonyDME3DTransfromFxPrarm*)m_ctrlEffects.GetItemDataPtr(nSel);
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_DUO_PAGE_ROLL == str)
		{
			DuoPageRollFxParam* pParam = (DuoPageRollFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->structTrans.fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_QUAD_PAGE_ROLL == str)
		{
			QuadPageRollFxParam* pParam = (QuadPageRollFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->structTrans.fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_CUBE_TRANS == str)
		{
			CubeFxParam* pParam = (CubeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->fTransition = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_SONY_BARN_SLIDE == str)
		{
			SonyBarnSlideFxParam* pParam = (SonyBarnSlideFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->fTrans = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_SONY_3D_BROKEN_GLASS == str)
		{
			Sony3DBrokenGlassFxParam* pParam = (Sony3DBrokenGlassFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->progress = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		else if (FX_DIP_TO_COLOR == str)
		{
			DipToColorTransFxParam* pParam = (DipToColorTransFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->fProcess = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
	}
}

void CEffectBar::SetProgress(CSliderCtrl* pCtrl)
{
	pCtrl->EnableWindow(TRUE);
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
		else if(FX_BARM_WIPE == str || FX_FAN_WIPE == str || FX_MULTI_AXIS_ROTARY_WIPE == str || FX_MATRIX_WIPE == str || FX_REVOLVING_WIPE == str)
		{
			BarmWipeFxParam* pParam = (BarmWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->structPattern.fOffset * 10000);
		}
		else if (FX_RIPPLE_WIPE == str)
		{
			RippleWipeFxParam* pParam = (RippleWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->transParam.progress * 10000);
		}
		else if (FX_BROKEN_GLASS == str)
		{
			BrokenGlassFxParam* pParam = (BrokenGlassFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->prm_movement * 10000);
		}
		else if (FX_RINGS == str)
		{
			RingsFxParam* pParam = (RingsFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fTranslate * 10000);
		}
		else if (FX_DISSOLVE == str)
		{
			DissolveFxParam* pParam = (DissolveFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->prm_process * 10000);
		}
		else if (FX_PAGE_ROTATION == str)
		{
			PageRotationFxParam* pParam = (PageRotationFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->structTrans.fTransition * 10000);
		}
		else if (FX_FADE_FROM_TO == str)
		{
			SonyFadeFromToFxParam* pParam = (SonyFadeFromToFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fTransition * 10000);
		}
		else if (FX_PUSH == str)
		{
			PushFxParam* pParam = (PushFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fTransition * 10000);
		}
		else if (FX_SONY_SLIDE == str)
		{
			SonySlideFxParam* pParam = (SonySlideFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fTransition * 10000);
		}
		else if (FX_PAGE_ROLL == str)
		{
			PageRollFxParam* pParam = (PageRollFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->structTrans.fTransition * 10000);
		}
		else if (FX_DUO_PAGE_ROLL == str)
		{
			DuoPageRollFxParam* pParam = (DuoPageRollFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->structTrans.fTransition * 10000);
		}
		else if (FX_QUAD_PAGE_ROLL == str)
		{
			QuadPageRollFxParam* pParam = (QuadPageRollFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->structTrans.fTransition * 10000);
		}
		else if (FX_CUBE_TRANS == str)
		{
			CubeFxParam* pParam = (CubeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fTransition * 10000);
		}
		else if(FX_SONY_BARN_SLIDE == str)
		{
			SonyBarnSlideFxParam* pParam = (SonyBarnSlideFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fTrans * 10000);
		}
		else if(FX_SONY_3D_BROKEN_GLASS == str)
		{
			Sony3DBrokenGlassFxParam* pParam = (Sony3DBrokenGlassFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->progress * 10000);
		}
		else if(FX_DIP_TO_COLOR == str)
		{
			DipToColorTransFxParam* pParam = (DipToColorTransFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->fProcess * 10000);
		}
		else	//不是过渡特技，没有进度条
		{
			pCtrl->SetPos(0);
			pCtrl->EnableWindow(FALSE);
		}
	}
	else
	{
		AfxMessageBox(_T("当前没有选择特技"), MB_ICONWARNING|MB_OK);
	}
}
