#ifndef GLOBAL_UNITS_HEAD_FILE
#define GLOBAL_UNITS_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgStatus.h"
#include "RoomViewItem.h"
#include "PlazaViewItem.h"
#include "ServerItemView.h"

//////////////////////////////////////////////////////////////////////////

//全局用户资料
struct tagGlobalUserData
{
	WORD								wFaceID;						//头像索引
	BYTE								cbGender;						//用户性别
	BYTE								cbMember;						//会员等级
	DWORD								dwUserID;						//用户 I D
	DWORD								dwGroupID;						//社团索引
	DWORD								dwExperience;					//用户经验
	DWORD								dwUserRight;					//用户等级
	DWORD								dwMasterRight;					//管理权限
	TCHAR								szAccounts[NAME_LEN];			//登录帐号
	TCHAR								szPassWord[PASS_LEN];			//登录密码
	TCHAR								szGroupName[GROUP_LEN];			//社团信息
};

//全局信息类
class CGlobalUnits
{
	//信息变量
protected:
	TCHAR								m_szStationPage[32];			//站点页面
	tagGlobalUserData					m_GloblaUserData;				//用户信息

	//系统变量
protected:
	DWORD								m_dwPlazaVersion;				//大厅版本
	TCHAR								m_szDirSkin[MAX_PATH];			//皮肤目录
	TCHAR								m_szDirWork[MAX_PATH];			//工作目录

	//组件变量
public:
	CServerListManager					m_ServerListManager;			//列表管理

	//组件定义
public:
	CUserFaceResHelper					m_UserFaceRes;					//头像组件
	CGameRankManagerHelper				m_GameRankManager;				//游戏等级
	CDownLoadServiceHelper				m_DownLoadService;				//下载服务
	CCompanionManagerHelper				m_CompanionManager;				//关闭管理
	
	//函数定义
public:
	//构造函数
	CGlobalUnits(void);
	//析构函数
	virtual ~CGlobalUnits(void);

	//功能函数
public:
	//初始化函数
	bool InitGlobalUnits();
	//拷贝字符
	bool CopyToClipboard(LPCTSTR pszString);
	//获取序列
	void GetClientSerial(tagClientSerial & ClientSerial);

	//查询函数
public:
	//大厅版本
	DWORD GetPlazaVersion() { return m_dwPlazaVersion; }
	//工作目录
	LPCTSTR GetWorkDirectory() { return m_szDirWork; }
	//界面目录
	LPCTSTR GetSkinDirectory() { return m_szDirSkin; }

	//状态函数
public:
	//设置主站地址
	LPCTSTR SetStationPage(LPCTSTR pszStationPage);
	//获取主站地址
	LPCTSTR GetStationPage() { return m_szStationPage; }
	//获取用户信息
	tagGlobalUserData & GetGolbalUserData() { return m_GloblaUserData; }

	//组件函数
public:
	//头像接口
	IUserFaceRes * GetUserFaceRes() { return m_UserFaceRes.GetInterface(); }
};

//////////////////////////////////////////////////////////////////////////

//全局调度类
class CGlobalAttemper
{
	//控件变量
protected:
	CDlgStatus							m_DlgStatus;					//状态窗口
   
	//函数定义
public:
	//构造函数
	CGlobalAttemper(void);
	//析构函数
	virtual ~CGlobalAttemper(void);

	//功能函数
public:
	//显示提示
	bool ShowStatusMessage(LPCTSTR pszMessage, CWnd * pNotifyWnd);
	//关闭提示
	bool DestroyStatusWnd(CWnd * pNotifyWnd);
	//下载游戏
	bool DownLoadClient(LPCTSTR pszKindName, WORD wKindID, bool bDisplay);
};

//////////////////////////////////////////////////////////////////////////

//全局信息 
extern	CGlobalUnits					g_GlobalUnits;					//信息组件			
extern	CGlobalAttemper					g_GlobalAttemper;				//全局调度

//////////////////////////////////////////////////////////////////////////

#endif