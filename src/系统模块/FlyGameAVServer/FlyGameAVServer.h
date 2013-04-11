// FlyGameAVServer.h : main header file for the FLYGAMEAVSERVER application
//

#if !defined(AFX_FLYGAMEAVSERVER_H__DAD4CAED_DBD7_407D_B7C4_048C2D70A4A4__INCLUDED_)
#define AFX_FLYGAMEAVSERVER_H__DAD4CAED_DBD7_407D_B7C4_048C2D70A4A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVServerApp:
// See FlyGameAVServer.cpp for the implementation of this class
//

class CFlyGameAVServerApp : public CWinApp
{
public:
	//功能:
	//输入:
	//输出:
	void WriteLog(CString strFileName, CString strText);

public:
	CFlyGameAVServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlyGameAVServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFlyGameAVServerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CFlyGameAVServerApp theApp;
extern CMapMsgItem<USERLISTITEM>  CMapUserList;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLYGAMEAVSERVER_H__DAD4CAED_DBD7_407D_B7C4_048C2D70A4A4__INCLUDED_)
