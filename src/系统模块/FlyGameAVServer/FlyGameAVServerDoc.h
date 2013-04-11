// FlyGameAVServerDoc.h : interface of the CFlyGameAVServerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLYGAMEAVSERVERDOC_H__938280D6_7CD9_4E27_B03B_9FC38679BC40__INCLUDED_)
#define AFX_FLYGAMEAVSERVERDOC_H__938280D6_7CD9_4E27_B03B_9FC38679BC40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FlyMsgManager.h"

class CFlyGameAVServerDoc : public CDocument
{
protected:
	//功能:
	//输入:
	//输出:
	void CreateServer();
	CFlyMsgManager myFlyMsgManager;

public:
	//打印消息
	//功能:
	//输入:
	//输出:
	void PrintTextMsg(LPCTSTR lpszMessage) ;

protected: // create from serialization only
	CFlyGameAVServerDoc();
	DECLARE_DYNCREATE(CFlyGameAVServerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlyGameAVServerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFlyGameAVServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFlyGameAVServerDoc)
	afx_msg void OnCreateServer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLYGAMEAVSERVERDOC_H__938280D6_7CD9_4E27_B03B_9FC38679BC40__INCLUDED_)
