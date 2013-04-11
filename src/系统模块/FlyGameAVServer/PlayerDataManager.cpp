// PlayerDataManager.cpp: implementation of the PlayerDataManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlyGameAVServer.h"
#include "PlayerDataManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PlayerDataManager::PlayerDataManager()
{
	
}

PlayerDataManager::~PlayerDataManager()
{
	
}
/*
//删除一条记录
功能：删除一用户信息
输入：
返回：
*/
void PlayerDataManager::DelOnePlayerInfo(CString strKey)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sPlayerDataManager.log",strTime);
	
	strTemp.Format("DelOnePlayerInfo(strKey=%s)", strKey);
	theApp.WriteLog(strFile, strTemp);
	
//向同房间的发送删除命令
	
	CMapUserList.RemoveKey(strKey);
	
	strTemp.Format("成功DEL一条记录!");
	theApp.WriteLog(strFile, strTemp);



	
}
//功能:增加一玩家资料
//输入:
//输出:
void PlayerDataManager::AddOnePlayerInfo(USERLISTITEM* myOneItem)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sPlayerDataManager.log",strTime);
	
	strTemp.Format("AddOnePlayerInfo()");
	theApp.WriteLog(strFile, strTemp);
	
	
	CString strKey;
	strKey.Format("%s", myOneItem->User_num);
	
	strTemp.Format("myOneItem->User_num=%s", myOneItem->User_num);
	theApp.WriteLog(strFile, strTemp);
	
	//查询记录
	USERLISTITEM* pItem = NULL;
	if(!CMapUserList.Lookup(strKey, (LPVOID&)pItem)) 
	{
		USERLISTITEM *oneItem = new USERLISTITEM;
		oneItem = myOneItem;
		
		CMapUserList.SetAtEx( strKey, oneItem);	
		strTemp.Format("成功增加一条记录!");
		theApp.WriteLog(strFile, strTemp);
		
	}
	//修改记录
	else
	{
		
		//pItem->ipAdd = myOneItem->ipAdd ;//IP 地址
		strTemp.Format("pItem->ipAdd=%s, 新记录myOneItem->ipAdd=%s ", pItem->ipAdd,myOneItem->ipAdd );
		theApp.WriteLog(strFile, strTemp);
		
		//	CopyMemory( 	pItem->ipAdd, myOneItem->ipAdd, 50);
		strcpy( pItem->ipAdd , myOneItem->ipAdd);
		pItem->recCmdPort = myOneItem->recCmdPort;//portInt
		
		pItem->room_id=	myOneItem->room_id;
		pItem->table_id = myOneItem->table_id;
		pItem->desk_id = myOneItem->desk_id;
		
		strTemp.Format("已经有记录,修改记录!,pItem->ipAdd=%s, pItem->recCmdPort=%d ", pItem->ipAdd,pItem->recCmdPort );
		theApp.WriteLog(strFile, strTemp);
		
		
	}
	
}
/*
//功能:修改某玩家的端口信息
//输入:用户ID，类型，位置，端口
//输出:
*/
void PlayerDataManager::SetOnePlayerPortInfo(CString  UserNO , int avType, int poInt,int portInt)
{
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sPlayerDataManager.log",strTime);
	
	strTemp.Format("SetOnePlayerPortInfo(  UserNO=%s ,  avType=%d,  poInt=%d, portInt=%d)", UserNO ,   avType,   poInt, portInt);
	theApp.WriteLog(strFile, strTemp);
	
	CString strKey;
	strKey.Format("%s", UserNO);
	
	//查询记录
	USERLISTITEM* pItem = NULL;
	if(CMapUserList.Lookup(strKey, (LPVOID&)pItem)) 
	{
		
		switch (avType)
		{
			//语音
		case DATA_TYPE_AUDIO:
			{
				if (poInt < REC_AUDIO_PORT_NUM )
				{
					pItem->recAudioPort[poInt] = portInt;	
					strTemp.Format("修改语音端口OK!");
					theApp.WriteLog(strFile, strTemp);
					
				}
			}
			break;
			//视频
		case DATA_TYPE_VIDEO:
			{
				if ( poInt <REC_VIDEO_PORT_NUM)
				{
					strTemp.Format("修改视频端口OK!");
					theApp.WriteLog(strFile, strTemp);
					pItem->recVideoPort[poInt] = portInt;
				}
			}
			break;
		}
		
	}
}
/*
功能：获得一玩家信息
输入：关键字
返回：用户信息
*/
USERLISTITEM PlayerDataManager::GetOnePlayerInfo(CString strKey)
{
	USERLISTITEM resultItem;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	CString strFile,strTemp;
	strFile.Format("log\\%sPlayerDataManager.log",strTime);
	
	strTemp.Format("GetOnePlayerInfo(strKey=%s)", strKey);
	theApp.WriteLog(strFile, strTemp);
	
	
	
	//查询记录
	USERLISTITEM* pItem = NULL;
	if(CMapUserList.Lookup(strKey, (LPVOID&)pItem)) 
	{
		
		resultItem = * pItem;
		strTemp.Format("查询到一条记录!");
		theApp.WriteLog(strFile, strTemp);
		
	}
	return resultItem;
}