// FlyGameAVServerView.h : interface of the CFlyGameAVServerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLYGAMEAVSERVERVIEW_H__11C9F6AF_15FD_4191_81C7_EA206C4A37A4__INCLUDED_)
#define AFX_FLYGAMEAVSERVERVIEW_H__11C9F6AF_15FD_4191_81C7_EA206C4A37A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFlyGameAVServerView : public CEditView
{
public:
	void Message(LPCTSTR lpszMessage);
protected: // create from serialization only
	CFlyGameAVServerView();
	DECLARE_DYNCREATE(CFlyGameAVServerView)

// Attributes
public:
	CFlyGameAVServerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlyGameAVServerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFlyGameAVServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFlyGameAVServerView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FlyGameAVServerView.cpp
inline CFlyGameAVServerDoc* CFlyGameAVServerView::GetDocument()
   { return (CFlyGameAVServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLYGAMEAVSERVERVIEW_H__11C9F6AF_15FD_4191_81C7_EA206C4A37A4__INCLUDED_)
