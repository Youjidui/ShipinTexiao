
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

#include "EffectSettingsPane.h"
#include "BufferSettingsPane.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:
	bool InitDoc();
	void UninitDoc();

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

	CBufferSettingsPane m_wndBufferBar;
	CEffectSettingsPane m_wndEffectBar;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSetImage(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSetTargetBufferSize(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSelectEffect(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
};


