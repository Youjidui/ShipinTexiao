
// TestClient.h : TestClient Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������

#include "../Logger/Logging.h"

// CTestClientApp:
// �йش����ʵ�֣������ TestClient.cpp
//

class CTestClientApp : public CWinAppEx
{
public:
	CTestClientApp();
	~CTestClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	BOOL  m_bHiColorIcons;
	Logging* m_pLogging;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTestClientApp theApp;
