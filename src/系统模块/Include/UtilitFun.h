// UtilitFun.h: interface for the CUtilitFun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITFUN_H__6A47BAFC_8FF2_45C2_872B_21A549A34B48__INCLUDED_)
#define AFX_UTILITFUN_H__6A47BAFC_8FF2_45C2_872B_21A549A34B48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxtempl.h"
//定义了常用的工具函数
class CUtilitFun  
{
public:
	CUtilitFun();
	virtual ~CUtilitFun();
public:
	int UnPackListStr(CString msg, CString findStr, CArray<CString,CString> *myArray );
	CString PackListStr(CString blankStr);
};

#endif // !defined(AFX_UTILITFUN_H__6A47BAFC_8FF2_45C2_872B_21A549A34B48__INCLUDED_)
