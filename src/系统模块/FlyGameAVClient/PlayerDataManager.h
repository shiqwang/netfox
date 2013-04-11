// PlayerDataManager.h: interface for the PlayerDataManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYERDATAMANAGER_H__3ECCEF9C_083D_42CD_9E4F_AEA84CC3CFE1__INCLUDED_)
#define AFX_PLAYERDATAMANAGER_H__3ECCEF9C_083D_42CD_9E4F_AEA84CC3CFE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Include/AMMsgType.h"

class PlayerDataManager  
{
public:
	//功能:设置某用户网络状态
	//输入:
	//输出:
int SetOnePlayerNetLinkStatus(CString userStr, int netStatus);

//功能:获取一玩家信息
//输入:
//输出:
	USERLISTITEM GetOnePlayerInfo(CString keyStr);

//功能:增加一玩家资料
//输入:
//输出:
	void AddOnePlayerInfo(USERLISTITEM* myOneItem);

//功能:DEL一玩家资料
//输入:
//输出:
	void DelOnePlayerInfo(CString keyStr);

//功能:修改某玩家的端口信息
//输入:
//输出:
	void SetOnePlayerPortInfo(CString  UserNO , int avType, int poInt , int portInt);


public:
	PlayerDataManager();
	virtual ~PlayerDataManager();

};

#endif // !defined(AFX_PLAYERDATAMANAGER_H__3ECCEF9C_083D_42CD_9E4F_AEA84CC3CFE1__INCLUDED_)
