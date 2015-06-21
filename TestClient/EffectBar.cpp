// EffectBar.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "EffectBar.h"


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
END_MESSAGE_MAP()


// CEffectBar 消息处理程序

void CEffectBar::OnBnClickedParameters()
{
	// TODO: 在此添加控件通知处理程序代码
}

BOOL CEffectBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlEffects.AddString("Negative");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
