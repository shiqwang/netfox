// FlyGameAVClient.h : main header file for the FLYGAMEAVCLIENT DLL
//

#if !defined(AFX_FLYGAMEAVCLIENT_H__A5D080A4_C56A_46AC_8CAD_3AC676AA1DCD__INCLUDED_)
#define AFX_FLYGAMEAVCLIENT_H__A5D080A4_C56A_46AC_8CAD_3AC676AA1DCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "FlyMsgManager.h"

/////////////////////////////////////////////////////////////////////////////
// CFlyGameAVClientApp
// See FlyGameAVClient.cpp for the implementation of this class
//

class CFlyGameAVClientApp : public CWinApp
{
public:
//CFlyMsgManager  myFlyMsgManager;

	CFlyGameAVClientApp();
	//功能:
	//输入:
	//输出:
	void WriteLog(CString strFileName, CString strText);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlyGameAVClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFlyGameAVClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern CFlyGameAVClientApp theApp;
extern CMapMsgItem<USERLISTITEM>  CMapUserList;
extern BOOL bOneShot;
extern 	CWnd * remoteWnd ;
extern CRect remoteRect;
extern CSampleGrabberCB *mpCB ;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLYGAMEAVCLIENT_H__A5D080A4_C56A_46AC_8CAD_3AC676AA1DCD__INCLUDED_)
