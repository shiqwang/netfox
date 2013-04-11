// FlySoundText.cpp: implementation of the CFlySoundText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "FlyScenePlayer.h"
#include "FlySoundText.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlySoundText::CFlySoundText()
{
	useSceneInt=0;

	memset( (char *)mySceneItemList , 0, sizeof(mySceneItemList));
}

CFlySoundText::~CFlySoundText()
{

}
/*
[SceneSound]
TypeCount=6
*/
//从INI文件读取得数据，并保存到数据结构中
void CFlySoundText::ReadDataFromIni(CString iniFileNameStr)
{
	char TypeCountStr[50];
	myCUtilitFun.RedDataFromIni("SceneSound","TypeCount","", TypeCountStr,16, iniFileNameStr );
 
	//1.获得场景数
	useSceneInt = atoi(TypeCountStr);//TypeCount
	//2。循环读取场景信息
	for (int i=0; i< useSceneInt && i<MAX_SCENE_NUM; i++)
	{
		ReadOneSceneDataFromIni( iniFileNameStr,   i+1);
	}
}
/*
[Type1]
TypeName=催促
count=3

itemText1=出牌，快出牌呀!
itemText2=快出快出，一会老板来了就惨了！
itemText3=别想了，快把牌拆了随便出一张呗!




file1=boy\催促\boy (9).mp3
file2=boy\催促\boy (10).mp3
file3=boy\催促\boy.mp3
*/
//从INI文件读取一个场景数据，并保存到数据结构中
void CFlySoundText::ReadOneSceneDataFromIni(CString iniFileNameStr,  int sceneInt)
{
	CString TypeText;
	TypeText.Format("Type%d", sceneInt);
	CString TypeName;

	int count;
	//1。获得概要情况
	char countStr[50];
	myCUtilitFun.RedDataFromIni( TypeText,"count","", countStr,16, iniFileNameStr );

	count = atoi(countStr);
	mySceneItemList[sceneInt-1].useTextInt = count;

	myCUtilitFun.RedDataFromIni( TypeText,"TypeName","", mySceneItemList[sceneInt-1].TypeName,50, iniFileNameStr );

	// ="";
	//2。循环读取
		CString itemText;
		CString fileText;
	for (int i=0; i< count && i<ONE_SCENE_MAX_TEXT_NUM; i++)
	{
		itemText.Format("itemText%d", i+1);
		fileText.Format("file%d", i+1);

	myCUtilitFun.RedDataFromIni( TypeText, itemText,"", mySceneItemList[sceneInt-1].TextList[i].showInfo,50, iniFileNameStr );
	myCUtilitFun.RedDataFromIni( TypeText, fileText,"", mySceneItemList[sceneInt-1].TextList[i].playSound,50, iniFileNameStr );

	}//End for (int i=
}


	//获得场景名称
CString CFlySoundText::GetOneSceneName(int sceneInt)
{
	CString	resultStr;
	if ( sceneInt < MAX_SCENE_NUM && sceneInt<useSceneInt)
	{
		resultStr.Format("%s",
		mySceneItemList[sceneInt].TypeName );
	}
	return resultStr;	
}

	//获得场景数据
bool CFlySoundText::GetOneSceneDataItem(int sceneInt,SCENE_ITEM *myItemPtr )
{
	bool resultBool = false;
	if ( sceneInt < MAX_SCENE_NUM && sceneInt<useSceneInt)
	{
		memcpy((char *)myItemPtr , (char *)&mySceneItemList[sceneInt] ,sizeof(SCENE_ITEM));
		resultBool=true;
	}
	else
	{
		return resultBool;	
	}
	return resultBool;	
}
	//获得one sound数据
bool CFlySoundText::GetOneSoundDataItem(int sceneInt,int soundInt,SOUND_TEXT_ITEM * mySoundItemPtr )
{
	bool resultBool = false;
	if ( sceneInt < MAX_SCENE_NUM && sceneInt<useSceneInt)
	{
		//memcpy((char *)myItemPtr , (char *)&mySceneItemList[sceneInt] ,sizeof(SCENE_ITEM));
		SCENE_ITEM myItemPtr;
		if ( GetOneSceneDataItem( sceneInt, &myItemPtr ))
		{
			
			memcpy((char *)mySoundItemPtr , (char *)&myItemPtr.TextList[soundInt] ,sizeof(SOUND_TEXT_ITEM));
			resultBool=true;
		}
	}
	else
	{
		return resultBool;	
	}
	return resultBool;	
}
//加载系统配置
void CFlySoundText::OnInitSysConfig()
{
/*
    CString strLocalIP,strUserNum,strUpdateTime,strVodPath;
	char chLocalIP[17],chUserNum[3],chUpdateTime[6],chVodPath[260];

	//Add by doctor 20070722
	CString videoBlank;
	CString stepInt;
	CString sleepTime;

	char chvideoBlank[20], chstepInt[20],chsleepTime[20];
	//End add

	
	myCUtilitFun->RedDataFromIni("VodConfig","LocalIPStr","",chLocalIP,16,".\\VodServerConfig.ini");
    strLocalIP.Format("%s",chLocalIP);
	m_LocalIP=strLocalIP;	//最大用户数
	LocalIPStr=strLocalIP;
	
	myCUtilitFun->RedDataFromIni("VodConfig","MaxVodUserNum","",chUserNum,3,".\\VodServerConfig.ini");
    strUserNum.Format("%s",chUserNum);
	m_UserNum=strUserNum;	//最大用户数
	UserNumStr=strUserNum;
	
	myCUtilitFun->RedDataFromIni("VodConfig","FileSearchTime","",chUpdateTime,6,".\\VodServerConfig.ini");
    strUpdateTime.Format("%s",chUpdateTime);
	m_UpdateTime=strUpdateTime;   //间隔时间
	
	myCUtilitFun->RedDataFromIni("VodConfig","VodRootFilePath","",chVodPath,60,".\\VodServerConfig.ini");
    strVodPath.Format("%s",chVodPath);
	m_VodPath=strVodPath;  //VOD路径
	VodPathStr=strVodPath;
	


	myCUtilitFun->RedDataFromIni("Record","videoBlank","",chvideoBlank, 20,".\\VodServerConfig.ini");
videoBlank.Format("%s", chvideoBlank);
	myCUtilitFun->RedDataFromIni("Record","stepInt","",chstepInt, 20,".\\VodServerConfig.ini");
stepInt.Format("%s", chstepInt);
	myCUtilitFun->RedDataFromIni("Record","sleepTime","",chsleepTime, 20,".\\VodServerConfig.ini");
sleepTime.Format("%s", chsleepTime);

videoBlankInt = atoi(videoBlank);
markStepInt = atoi(stepInt);//和视频比较
sleepTimeInt = atoi(sleepTime); //睡眠时间
	*/

}