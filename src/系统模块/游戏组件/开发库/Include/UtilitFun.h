// UtilitFun.h: interface for the CUtilitFun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILITFUN_H__78599FDE_567A_4D0D_AF2D_A476553F4D43__INCLUDED_)
#define AFX_UTILITFUN_H__78599FDE_567A_4D0D_AF2D_A476553F4D43__INCLUDED_

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
	
	void WriteLogFile(CString logFileStr, CString inMsg);
	void WriteMsgToFile(CString fileName, char * myP, int msgLength);
	//0=缩小,1=放大
	CRect ZoomInOutRect(CRect *inRect, int opType, int wInt, int hInt);
	
	//Ini file
	void WriteDataToIni(CString fileNameStr, CString strSection, CString strSectionKey, CString strValue );
	CString  RedDataFromIni(CString fileNameStr, CString strSection, CString strSectionKey);
	
	CString GetLogFileStr();
	//读写配置文件
	int	RedDataFromIni(CString SectionName,CString KeyName,CString Default,char * nBuffer,DWORD nSize,CString ConfigFileName);
   // void WriteDataToIni(CString SectionName,CString KeyName,CString StringVariable,CString ConfigFileName);
	
};

#endif // !defined(AFX_UTILITFUN_H__78599FDE_567A_4D0D_AF2D_A476553F4D43__INCLUDED_)
