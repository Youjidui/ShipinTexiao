
// TestClientView.cpp : CTestClientView 类的实现
//

#include "stdafx.h"
#include "TestClient.h"

#include "TestClientDoc.h"
#include "TestClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestClientView

IMPLEMENT_DYNCREATE(CTestClientView, CView)

BEGIN_MESSAGE_MAP(CTestClientView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTestClientView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CTestClientView 构造/析构

CTestClientView::CTestClientView()
{
	// TODO: 在此处添加构造代码

}

CTestClientView::~CTestClientView()
{
}

BOOL CTestClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CTestClientView 绘制

void CTestClientView::OnDraw(CDC* /*pDC*/)
{
	CTestClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CTestClientView 打印


void CTestClientView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CTestClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CTestClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CTestClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CTestClientView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTestClientView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CTestClientView 诊断

#ifdef _DEBUG
void CTestClientView::AssertValid() const
{
	CView::AssertValid();
}

void CTestClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestClientDoc* CTestClientView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestClientDoc)));
	return (CTestClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CTestClientView 消息处理程序
