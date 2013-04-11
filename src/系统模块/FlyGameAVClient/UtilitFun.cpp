// UtilitFun.cpp: implementation of the CUtilitFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlyGameAVClient.h"
#include "../Include/UtilitFun.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtilitFun::CUtilitFun()
{

}

CUtilitFun::~CUtilitFun()
{

}
CString CUtilitFun::PackListStr(CString blankStr)
{

	CString resultStr;
	/*
		//Ñ­»·
		char my[100];
		for (int i=0; i < m_listRoomPerson.GetCount(); i++)
		{
			CString itemStr;
			m_listRoomPerson.GetText(i, my);
			itemStr.Format("%s", my);
			resultStr += blankStr + itemStr;

		}
		//AfxMessageBox(resultStr);
		*/
	return resultStr;
}
//ÊäÈë£º*122*233232*43433*3232
//Êä³ö£º
int CUtilitFun::UnPackListStr(CString msg, CString findStr, CArray<CString,CString> *myArray )
{
	int resultInt=0;

	msg+=findStr;
	int firstInt = msg.Find(findStr);
	int nextInt = msg.Find( findStr, firstInt + 1);
	int msgLeng = nextInt - firstInt;
	
	while ( firstInt >= 0)
	{
		CString myStr = msg.Mid( firstInt + 1, msgLeng -1 );
		msgLeng = myStr.GetLength();

		if ( myStr.GetLength() > 0) 
		{
			myArray->Add(myStr);
			resultInt++;
		}

		firstInt = nextInt;
		nextInt = msg.Find(findStr, firstInt + 1);
		msgLeng = nextInt - firstInt;
	}	
	return resultInt;
}