// FlyMsgManager.h: interface for the CFlyMsgManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLYMSGMANAGER_H__2BAF42F5_8109_48D9_A3D8_532454991498__INCLUDED_)
#define AFX_FLYMSGMANAGER_H__2BAF42F5_8109_48D9_A3D8_532454991498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Include/FlyNetProtocol.h"
#include "../Include/UtilitFun.h"

#include "PlayerDataManager.h"
class CFlyMsgManager  
{
	
private:
	PlayerDataManager  myPlayerDataManager;
	void AddOnePlayerInfo(CArray<CString,CString> *myArray,  CString ipAdd, CString portStr);
	
	//功能:
	//输入:
	//输出:
	void SendOnePlayerInfo(char * myUserItem, char *ipStr, int portInt, int opType=0);
	
public:
	CFlyMsgManager();
	virtual ~CFlyMsgManager();
public:

	//功能:向同桌的其他玩家发送新增玩家信息
	//输入:
	//输出:
	void SendAddOrDelOnePlayerInfoToOther(int userID, int roomID, int tableID, int opTypeInt =0);
	

	//功能:处理获取同桌玩家信息
	//输入:0=默认,1=转发AV数据
	//输出:
	void DealwithGetOneTablePlayerList(int userID, int roomID, int tableID, int opTypeInt =0);
	
	//功能:
	//输入:
	//输出:
	void SendCmdMsgToClient(char * buf, int lengInt, char *ipStr, int portInt);
	
	//功能:停止一个线程
	//输入:
	//输出:
	void StopOneThread(HANDLE myH);
	
	//线程函数
	HANDLE m_hMsgRecvSockThread;
	DWORD m_dwMsgRecvSockThreadId;
	//功能:接收控制命令
	//输入:
	//输出:
	static DWORD WINAPI MsgRecvSockThreadProc(LPVOID lpParameter);
	
	HANDLE m_hTransSockThread;
	DWORD m_dwTransSockThreadId;
	//转发语音视频数据
	static DWORD WINAPI TransSockThreadProc(LPVOID lpParameter);
	
	CUtilitFun myCUtilitFun;
	
	int UdpSocketArray[UDP_NUM];
	//初始化，创建SOCKET，并监听
	BOOL Init();
	void InitSocketList();
};

#endif // !defined(AFX_FLYMSGMANAGER_H__2BAF42F5_8109_48D9_A3D8_532454991498__INCLUDED_)
