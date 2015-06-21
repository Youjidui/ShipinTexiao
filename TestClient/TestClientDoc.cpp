
// TestClientDoc.cpp : CTestClientDoc 类的实现
//

#include "stdafx.h"
#include "TestClient.h"

#include "TestClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestClientDoc

IMPLEMENT_DYNCREATE(CTestClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestClientDoc, CDocument)
END_MESSAGE_MAP()


// CTestClientDoc 构造/析构

CTestClientDoc::CTestClientDoc()
{
	// TODO: 在此添加一次性构造代码

}

CTestClientDoc::~CTestClientDoc()
{
}

BOOL CTestClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CTestClientDoc 序列化

void CTestClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CTestClientDoc 诊断

#ifdef _DEBUG
void CTestClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTestClientDoc 命令
