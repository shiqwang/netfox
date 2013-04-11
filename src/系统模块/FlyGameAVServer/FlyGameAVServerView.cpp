// FlyGameAVServerView.cpp : implementation of the CFlyGameAVServerView class
//

#include "stdafx.h"
#include "FlyGameAVServer.h"

#include "FlyGameAVServerDoc.h"
#include "FlyGameAVServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVServerView

IMPLEMENT_DYNCREATE(CFlyGameAVServerView, CEditView)

BEGIN_MESSAGE_MAP(CFlyGameAVServerView, CEditView)
	//{{AFX_MSG_MAP(CFlyGameAVServerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVServerView construction/destruction

CFlyGameAVServerView::CFlyGameAVServerView()
{
	// TODO: add construction code here

}

CFlyGameAVServerView::~CFlyGameAVServerView()
{
}

BOOL CFlyGameAVServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVServerView drawing

void CFlyGameAVServerView::OnDraw(CDC* pDC)
{
	CFlyGameAVServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVServerView printing

BOOL CFlyGameAVServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default CEditView preparation
	return CEditView::OnPreparePrinting(pInfo);
}

void CFlyGameAVServerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView begin printing.
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void CFlyGameAVServerView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView end printing
	CEditView::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVServerView diagnostics

#ifdef _DEBUG
void CFlyGameAVServerView::AssertValid() const
{
	CEditView::AssertValid();
}

void CFlyGameAVServerView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CFlyGameAVServerDoc* CFlyGameAVServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFlyGameAVServerDoc)));
	return (CFlyGameAVServerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVServerView message handlers

	//功能:
	//输入:
	//输出:
void CFlyGameAVServerView::Message(LPCTSTR lpszMessage)
{
	CString strTemp = lpszMessage;
	strTemp += _T("\r\n");
	int len = GetWindowTextLength();
	GetEditCtrl().SetSel(len,len);
	GetEditCtrl().ReplaceSel(strTemp);

}