// Logger.h : Logger DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLoggerApp
// �йش���ʵ�ֵ���Ϣ������� Logger.cpp
//

class CLoggerApp : public CWinApp
{
public:
	CLoggerApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
