// EffectBar.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "EffectBar.h"
#include "EffectName.h"
#include "CommonMessage.h"

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
END_MESSAGE_MAP()


// CEffectBar 消息处理程序

void CEffectBar::OnBnClickedParameters()
{
	// TODO: 在此添加控件通知处理程序代码
	//根据选择的特效，弹出其参数对话框
}

BOOL CEffectBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//m_ctrlEffects.AddString(_T("none"));
	m_ctrlEffects.AddString(FX_NEGATIVE);
	m_ctrlEffects.AddString(FX_COLOR_KEY);

	m_ctrlEffects.SetCurSel(0);

	//ON_CONTROL(CBN_SELCHANGE, IDC_EFFECTS
	PostMessage(WM_COMMAND, MAKELONG(IDC_EFFECTS, CBN_SELCHANGE), (LPARAM)m_ctrlEffects.GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEffectBar::OnCbnSelchangeEffects()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel = m_ctrlEffects.GetCurSel();
	if(LB_ERR != nSel)
	{
		CString str;
		m_ctrlEffects.GetLBText(nSel, str);
		AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, nSel, (LPARAM)(LPCTSTR)str);
	}
}
