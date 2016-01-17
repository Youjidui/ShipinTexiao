
// TestClientView.cpp : CTestClientView ���ʵ��
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
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTestClientView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CTestClientView ����/����

CTestClientView::CTestClientView()
{
	// TODO: �ڴ˴���ӹ������

}

CTestClientView::~CTestClientView()
{
}

BOOL CTestClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CTestClientView ����

void CTestClientView::OnDraw(CDC* /*pDC*/)
{
	CTestClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	pDoc->Render();
	pDoc->Draw(GetSafeHwnd());
}


// CTestClientView ��ӡ


void CTestClientView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CTestClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CTestClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CTestClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
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


// CTestClientView ���

#ifdef _DEBUG
void CTestClientView::AssertValid() const
{
	CView::AssertValid();
}

void CTestClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestClientDoc* CTestClientView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestClientDoc)));
	return (CTestClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CTestClientView ��Ϣ�������

BOOL CTestClientView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}
