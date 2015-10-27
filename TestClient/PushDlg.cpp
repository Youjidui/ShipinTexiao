// PushDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "PushDlg.h"


// PushDlg 对话框

IMPLEMENT_DYNAMIC(CPushDlg, CDialog)

CPushDlg::CPushDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPushDlg::IDD, pParent)
{

}

CPushDlg::~CPushDlg()
{
}

void CPushDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPushDlg, CDialog)
END_MESSAGE_MAP()


// PushDlg 消息处理程序
