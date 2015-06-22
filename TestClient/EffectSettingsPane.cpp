// EffectSettingsPane.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "EffectSettingsPane.h"


// CEffectSettingsPane

IMPLEMENT_DYNAMIC(CEffectSettingsPane, CDockablePane)

CEffectSettingsPane::CEffectSettingsPane()
{

}

CEffectSettingsPane::~CEffectSettingsPane()
{
}


BEGIN_MESSAGE_MAP(CEffectSettingsPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CEffectSettingsPane 消息处理程序



int CEffectSettingsPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_wndEffectBar.Create(IDD_DIALOGBAR, this);
	CRect rc;
	m_wndEffectBar.GetClientRect(rc);
	SetMinSize(CSize(rc.Width(), rc.Height()));
	m_wndEffectBar.ShowWindow(SW_SHOW);

	return 0;
}

void CEffectSettingsPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rc;
	GetClientRect(rc);
	m_wndEffectBar.MoveWindow(rc);
}
