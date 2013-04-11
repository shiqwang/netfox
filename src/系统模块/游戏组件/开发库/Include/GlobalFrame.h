#ifndef GLOBAL_FRAME_HEAD_FILE
#define GLOBAL_FRAME_HEAD_FILE

#pragma once

#include "GlobalDef.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏状态
#define GS_FREE							0								//空闲状态
#define GS_PLAYING						100								//游戏状态

//////////////////////////////////////////////////////////////////////////
//IPC 网络事件

#define IPC_MAIN_SOCKET					1								//网络消息

#define IPC_SUB_SOCKET_SEND				1								//网络发送
#define IPC_SUB_SOCKET_RECV				2								//网络接收

//IPC 网络包结构
struct IPC_SocketPackage
{
	CMD_Command							Command;						//命令信息
	BYTE								cbBuffer[SOCKET_PACKAGE];		//数据缓冲
};

//////////////////////////////////////////////////////////////////////////
//IPC 配置信息

#define IPC_MAIN_CONFIG					2								//配置信息

#define IPC_SUB_SERVER_INFO				1								//房间信息
#define IPC_SUB_COLUMN_INFO				2								//列表信息

//游戏信息
struct IPC_GF_ServerInfo
{
	DWORD								dwUserID;						//用户 I D
	WORD								wTableID;						//桌子号码
	WORD								wChairID;						//椅子号码
	WORD								wKindID;						//类型标识
	WORD								wServerID;						//房间标识
	WORD								wGameGenre;						//游戏类型
	WORD								wChairCount;					//椅子数目
	TCHAR								szKindName[KIND_LEN];			//类型名字
	TCHAR								szServerName[SERVER_LEN];		//房间名称
	tagOptionBuffer						OptionBuffer;					//配置信息
};

//////////////////////////////////////////////////////////////////////////
//IPC 用户信息

#define IPC_MAIN_USER					3								//用户信息

#define IPC_SUB_USER_COME				1								//用户信息
#define IPC_SUB_USER_STATUS				2								//用户状态
#define IPC_SUB_USER_SCORE				3								//用户积分
#define IPC_SUB_GAME_START				4								//游戏开始
#define IPC_SUB_GAME_FINISH				5								//游戏结束

//用户状态
struct IPC_UserStatus
{
	DWORD								dwUserID;						//用户 I D
	WORD								wNetDelay;						//网络延时
	BYTE								cbUserStatus;					//用户状态
};

//用户分数
struct IPC_UserScore
{
	DWORD								dwUserID;						//用户 I D
	tagUserScore						UserScore;						//用户积分
};

//////////////////////////////////////////////////////////////////////////
//IPC 控制信息

#define IPC_MAIN_CONCTROL				4								//控制信息

#define IPC_SUB_START_FINISH			1								//启动完成
#define IPC_SUB_CLOSE_FRAME				2								//关闭框架

//////////////////////////////////////////////////////////////////////////
//网络命令码

#define MDM_GF_GAME						100								//游戏消息
#define MDM_GF_FRAME					101								//框架消息

#define SUB_GF_INFO						1								//游戏信息
#define SUB_GF_USER_READY				2								//用户同意

#define SUB_GF_OPTION					100								//游戏配置
#define SUB_GF_SCENE					101								//场景信息

#define SUB_GF_USER_CHAT				200								//用户聊天

#define SUB_GF_MESSAGE					300								//系统消息

//版本信息
struct CMD_GF_Info
{
	BYTE								bAllowLookon;					//旁观标志
};

//游戏配置
struct CMD_GF_Option
{
	BYTE								bGameStatus;					//游戏状态
	BYTE								bAllowLookon;					//允许旁观
};

//聊天结构
struct CMD_GF_UserChat
{
	WORD								wChatLength;					//信息长度
	COLORREF							crFontColor;					//信息颜色
	DWORD								dwSendUserID;					//发送用户
	DWORD								dwTargetUserID;					//目标用户
	TCHAR								szChatMessage[MAX_CHAT_LEN];	//聊天信息
};

//消息类型
#define SMT_INFO						0x0001							//信息消息
#define SMT_EJECT						0x0002							//弹出消息
#define SMT_GLOBAL						0x0004							//全局消息
#define SMT_CLOSE_GAME					0x1000							//关闭游戏

//消息数据包
struct CMD_GF_Message
{
	WORD							wMessageType;						//消息类型
	WORD							wMessageLength;						//消息长度
	TCHAR							szContent[1024];					//消息内容
};

//////////////////////////////////////////////////////////////////////////

//共享内存定义
struct tagShareMemory
{
	WORD								wDataSize;						//数据大小
	HWND								hWndGameFrame;					//框架句柄
	HWND								hWndGamePlaza;					//广场句柄
	HWND								hWndGameServer;					//房间句柄
};

//////////////////////////////////////////////////////////////////////////

#endif