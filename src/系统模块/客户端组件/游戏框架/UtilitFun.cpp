// UtilitFun.cpp: implementation of the CUtilitFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "PlayFlash.h"
#include "UtilitFun.h"

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

//读取INI文件函数
int	CUtilitFun::RedDataFromIni(CString SectionName,CString KeyName,CString Default,char * nBuffer,DWORD nSize,CString ConfigFileName)
{
	int resultInt;
	resultInt=::GetPrivateProfileString(SectionName,KeyName,Default,nBuffer,nSize,ConfigFileName);//从INI文件中获得数据
	return resultInt;
}
/*
//写入INI文件函数
void CUtilitFun::WriteDataToIni(CString SectionName,CString KeyName,CString StringVariable,CString ConfigFileName)
{
	::WritePrivateProfileString(SectionName,KeyName,StringVariable,ConfigFileName);
	
}
*/
CString CUtilitFun::PackListStr(CString blankStr)
{

	CString resultStr;
	/*
		//循环
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
//-1=缩小,1=放大
CRect CUtilitFun::ZoomInOutRect(CRect *inRect, int opType, int wInt, int hInt)
{

	CRect resultRect;

	resultRect = *inRect;

	resultRect.left =resultRect.left - wInt*opType;
	resultRect.right=resultRect.right + wInt*opType;
	resultRect.top= resultRect.top - hInt*opType;
	resultRect.bottom= resultRect.bottom + hInt* opType;

	inRect->left = resultRect.left;
	inRect->right= 	resultRect.right;
	inRect->top= resultRect.top;
	inRect->bottom=resultRect.bottom;

	return resultRect;

}
//输入：*122*233232*43433*3232
//输出：
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

		//	if ( myStr.GetLength() > 0) 
		//解决了密码为空的异常2005-10-27;会导致注册端口异常 
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

void CUtilitFun::WriteMsgToFile(CString fileName, char * myP, int msgLength)
{
	//写入文件
	CFile *destFileW;
	destFileW = new CFile( fileName,  CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	destFileW->Write( myP,  msgLength);
	destFileW->Close();	

}
void CUtilitFun::WriteLogFile(CString logFileStr, CString inMsg)
{
	CString fileName;
	char * myP;
	int msgLength;
	fileName ="log//"+logFileStr+".log";

	msgLength = inMsg.GetLength();
	myP = new char[msgLength]  ;
	
	myP = (LPSTR)(PCSTR)inMsg;

	WriteMsgToFile(  fileName,  myP,  msgLength);

}

//write
void CUtilitFun::WriteDataToIni(CString fileNameStr, CString strSection, CString strSectionKey, CString strValue )
{
	char appPath[256];
	GetCurrentDirectory(256,appPath); //取得应用程序当前路径
	CString  FilePath;
	FilePath.Format("%s\\",appPath);
	FilePath+=fileNameStr;//"FLY_INI_FILE";
	

		WritePrivateProfileString (strSection,strSectionKey,  
			strValue, FilePath); 

}

CString  CUtilitFun::RedDataFromIni(CString fileNameStr, CString strSection, CString strSectionKey)
{
	CString resultStr;
	
	char appPath[256];
	GetCurrentDirectory(256,appPath); //取得应用程序当前路径
	CString  FilePath;
	FilePath.Format("%s\\",appPath);
	FilePath+=fileNameStr;//"FLY_INI_FILE";

	CString strValue       = _T("");
	char inBuf[80];
	GetPrivateProfileString (strSection,strSectionKey, NULL, inBuf, 80, FilePath); 
	resultStr=inBuf;

	return resultStr;
}

CString CUtilitFun::GetLogFileStr()
{
	CString resultStr;
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sMsgFlyGameXML.log",strTime);
	resultStr = strFile;
	return resultStr;
}