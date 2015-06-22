#pragma once

#include "BufferBar.h"

// CBufferSettingsPane

class CBufferSettingsPane : public CDockablePane
{
	DECLARE_DYNAMIC(CBufferSettingsPane)

public:
	CBufferSettingsPane();
	virtual ~CBufferSettingsPane();

private:
	CBufferBar m_wndBar;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


