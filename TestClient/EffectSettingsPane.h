#pragma once

#include "EffectBar.h"

// CEffectSettingsPane

class CEffectSettingsPane : public CDockablePane
{
	DECLARE_DYNAMIC(CEffectSettingsPane)

public:
	CEffectSettingsPane();
	virtual ~CEffectSettingsPane();

private:
	CEffectBar m_wndEffectBar;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


