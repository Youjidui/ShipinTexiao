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
		else if(FX_MATRIX_WIPE == str)
		{
			if(!m_matrixWipeDlg.GetSafeHwnd())
			{
				m_matrixWipeDlg.Create(CParamMatrixWipeDlg::IDD);
			}
			m_matrixWipeDlg.ShowWindow(SW_SHOW);
		}
		else if(FX_PUSH == str)
		{
			if(!m_pushDlg.GetSafeHwnd())
			{
				m_pushDlg.Create(CPushDlg::IDD);
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
	}
}

BOOL CEffectBar::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	i = m_ctrlEffects.AddString(FX_MATRIX_WIPE);
	{
		MatrixWipeFxParam* pParam = new MatrixWipeFxParam;
		pParam->cbSize = sizeof(MatrixWipeFxParam);
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_matrixWipeDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_SONY_SLIDE);
	{
		SonySlideFxParam* pParam = new SonySlideFxParam;
		ZeroMemory(pParam, sizeof(SonySlideFxParam));
		m_ctrlEffects.SetItemDataPtr(i, pParam);
		m_SonySlideDlg.SetParam(pParam);
	}
	i = m_ctrlEffects.AddString(FX_SONY_MASK);
	{
		SonyMaskFxParam* p = new SonyMaskFxParam;
		m_ctrlEffects.SetItemDataPtr(i, p);
		//m_blurDlg.SetParam(p);
	}
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
		if(FX_BARM_WIPE == str)
		{
			BarmWipeFxParam* pParam = (BarmWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->structPattern.fOffset = nPos / 10000.f;
			AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)(LPCTSTR)str, (LPARAM)pParam);
		}
		if(FX_MATRIX_WIPE == str)
		{
			MatrixWipeFxParam* pParam = (MatrixWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			pParam->structPattern.fOffset = nPos / 10000.f;
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
		else if(FX_BARM_WIPE == str)
		{
			BarmWipeFxParam* pParam = (BarmWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->structPattern.fOffset * 10000);
		}
		else if(FX_MATRIX_WIPE == str)
		{
			MatrixWipeFxParam* pParam = (MatrixWipeFxParam*)m_ctrlEffects.GetItemDataPtr(nSel);
			if(pParam)	pCtrl->SetPos(pParam->structPattern.fOffset * 10000);
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
		//else if(FX_SONY_DME_3D_TRANSFORM == str)
		//{
		//	SonyDME3DTransfromFxPrarm* pParam = (SonyDME3DTransfromFxPrarm*)m_ctrlEffects.GetItemDataPtr(nSel);
		//	if(pParam)	pCtrl->SetPos(pParam->fPerspective * 10000);
		//}
	}
}
