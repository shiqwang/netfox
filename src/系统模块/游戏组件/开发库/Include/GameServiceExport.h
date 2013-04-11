#ifndef GAME_SERVICE_EXPORT_HEAD_FILE
#define GAME_SERVICE_EXPORT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "Template.h"
#include "GlobalDef.h"
#include "GlobalField.h"
#include "GlobalFrame.h"
#include "GlobalRight.h"
#include "GlobalService.h"

//命令码文件
#include "CMD_Game.h"
#include "CMD_Plaza.h"
#include "CMD_Center.h"

//组件头文件
#include "IUnknownEx.h"
#include "SocketModule.h"
#include "ServerKernel.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef GAME_SERVICE_CLASS
	#ifdef  GAME_SERVICE_DLL
		#define GAME_SERVICE_CLASS _declspec(dllexport)
	#else
		#define GAME_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define GAME_SERVICE_DLL_NAME	TEXT("GameServiceD.dll")				//组件 DLL 名字
#else
	#define GAME_SERVICE_DLL_NAME	TEXT("GameService.dll")					//组件 DLL 名字
#endif

//宏定义
#define IDI_MAX_TIME_ID					30									//极限定时器

//结束原因
#define GER_NORMAL						0x00								//常规结束
#define GER_DISMISS						0x01								//游戏解散
#define GER_USER_LEFT					0x02								//用户离开

//////////////////////////////////////////////////////////////////////////
//枚举定义

//开始模式
enum enStartMode
{
	enStartMode_FullReady,				//满人开始
	enStartMode_AllReady,				//所有准备
	enStartMode_Symmetry,				//对称开始
	enStartMode_TimeControl,			//时间控制
};

//分数类型
enum enScoreKind
{
	enScoreKind_Win,					//胜
	enScoreKind_Lost,					//输
	enScoreKind_Draw,					//和
	enScoreKind_Flee,					//逃
};

//////////////////////////////////////////////////////////////////////////
//结构定义

//积分信息
struct tagScoreInfo
{
	LONG								lScore;								//游戏积分
	enScoreKind							ScoreKind;							//分数类型
};

//用户规则
struct tagUserRule
{
	bool								bPassword;							//设置密码
	bool								bLimitWin;							//限制胜率
	bool								bLimitFlee;							//限制逃跑
	bool								bLimitScore;						//限制分数
	bool								bLimitDelay;						//限制延时
	bool								bCheckSameIP;						//效验地址
	WORD								wWinRate;							//胜率限制
	WORD								wFleeRate;							//限制逃跑
	WORD								wNetDelay;							//网络延时
	LONG								lMaxScore;							//最高分数 
	LONG								lLessScore;							//最低分数
	TCHAR								szPassword[PASS_LEN];				//桌子密码
};

//用户信息结构
struct tagServerUserData
{
	//属性信息
	WORD								wFaceID;							//头像索引
	BYTE								cbMember;							//会员等级
	BYTE								cbGender;							//用户性别
	DWORD								dwUserID;							//用户 I D
	DWORD								dwGroupID;							//社团索引
	DWORD								dwUserRight;						//用户等级
	DWORD								dwMasterRight;						//管理权限
	TCHAR								szAccounts[NAME_LEN];				//用户帐号
	TCHAR								szGroupName[GROUP_LEN];				//社团名字

	//状态信息
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	WORD								wNetDelay;							//网络延时
	BYTE								cbUserStatus;						//用户状态

	//分数信息
	tagUserScore						UserScoreInfo;						//用户分数
};

//桌子参数结构
struct tagTableFrameParameter
{
	IUnknownEx							* pIGameServiceFrame;				//服务框架
	IUnknownEx							* pIGameServiceManager;				//服务管理
	const tagGameServiceAttrib			* pGameServiceAttrib;				//桌子属性
	const tagGameServiceOption			* pGameServiceOption;				//配置参数
};

//服务参数结构
struct tagGameServiceParameter
{
	TCHAR								szModule[32];						//模块名字
	DWORD								dwCenterAddr;						//中心地址
	tagDataBaseInfo						GameUserDBInfo;						//数据配置
	tagDataBaseInfo						GameScoreDBInfo;					//数据配置
	tagGameServiceOption				GameServiceOption;					//服务配置
};

//////////////////////////////////////////////////////////////////////////

#define VER_IServerUserItem INTERFACE_VERSION(2,1)
static const GUID IID_IServerUserItem={0x8fbde5db,0x99c5,0x4908,0x91,0xed,0x74,0xea,0xe4,0xc0,0xa7,0x8d};

//服务器用户接口
interface IServerUserItem : public IUnknownEx
{
	//信息接口
public:
	//是否激活
	virtual bool __cdecl IsActive()=NULL;
	//用户索引
	virtual WORD __cdecl GetUserIndex()=NULL;
	//获取地址
	virtual DWORD __cdecl GetClientIP()=NULL;
	//获取密码
	virtual LPCTSTR __cdecl GetPassword()=NULL;
	//获取规则
	virtual const tagUserRule * __cdecl GetUserRule()=NULL;
	//获取积分
	virtual const tagUserScore * __cdecl GetUserScore()=NULL;
	//获取信息
	virtual tagServerUserData * __cdecl GetUserData()=NULL;

	//属性接口
public:
	//用户名字
	virtual LPCTSTR __cdecl GetAccounts()=NULL;
	//用户 I D
	virtual DWORD __cdecl GetUserID()=NULL;
	//桌子号码
	virtual WORD __cdecl GetTableID()=NULL;
	//椅子号码
	virtual WORD __cdecl GetChairID()=NULL;
	//用户状态
	virtual BYTE __cdecl GetUserStatus()=NULL;
	//用户胜率
	virtual WORD __cdecl GetUserWinRate()=NULL;
	//用户输率
	virtual WORD __cdecl GetUserLostRate()=NULL;
	//用户和率
	virtual WORD __cdecl GetUserDrawRate()=NULL;
	//用户逃率
	virtual WORD __cdecl GetUserFleeRate()=NULL;

	//统计接口
public:
	//游戏时间
	virtual DWORD __cdecl GetPlayTimeCount()=NULL;
	//在线时间
	virtual DWORD __cdecl GetOnlineTimeCount()=NULL;
	//积分信息
	virtual bool __cdecl GetUserScoreInfo(tagUserScore & ScoreResultInfo, tagUserScore & ScoreModifyInfo)=NULL;

	//功能接口
public:
	//设置规则
	virtual bool __cdecl SetUserRule(tagUserRule & UserRule)=NULL;
	//修改积分
    virtual bool __cdecl WriteScore(tagScoreInfo & ScoreInfo, DWORD dwPlayTimeCount)=NULL;
	//设置状态
	virtual bool __cdecl SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IServerUserManager INTERFACE_VERSION(2,1)
static const GUID IID_IServerUserManager={0xa27d1d5d,0x5999,0x488e,0xaa,0x4a,0x30,0x8a,0x1,0x11,0xf4,0x9e};

//服务器用户管理接口
interface IServerUserManager : public IUnknownEx
{
	//管理接口
public:
	//重置用户
	virtual bool __cdecl ResetUserManager()=NULL;
	//切换用户
	virtual bool __cdecl SwitchOnLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)=NULL;
	//激活用户
	virtual bool __cdecl ActiveOffLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)=NULL;
	//激活用户
	virtual IServerUserItem * __cdecl ActiveUserItem(tagServerUserData & ServerUserData, DWORD dwClientIP, WORD wUserIndex, TCHAR szPassword[PASS_LEN])=NULL;
	//删除用户
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//注册断线
	virtual bool __cdecl RegOffLineUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//删除断线
	virtual bool __cdecl UnRegOffLineUserItem(IServerUserItem * pIServerUserItem)=NULL;

	//查找接口
public:
	//枚举用户
	virtual IServerUserItem * __cdecl EnumOnLineUser(WORD wEnumIndex)=NULL;
	//枚举用户
	virtual IServerUserItem * __cdecl EnumOffLineUser(WORD wEnumIndex)=NULL;
	//查找用户
	virtual IServerUserItem * __cdecl SearchOnLineUser(DWORD dwUserID)=NULL;
	//查找用户
	virtual IServerUserItem * __cdecl SearchOffLineUser(DWORD dwUserID)=NULL;

	//信息接口
public:
	//在线人数
	virtual DWORD __cdecl GetOnLineCount()=NULL;
	//断线人数
	virtual DWORD __cdecl GetOffLineCount()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrame INTERFACE_VERSION(5,1)
static const GUID IID_ITableFrame={0x1aee9bf4,0x8629,0x45dc,0xb0,0xfa,0x2c,0xa7,0x4f,0xbe,0x18,0xc1};

//游戏桌子接口
interface ITableFrame : public IUnknownEx
{
	//信息接口
public:
	//桌子号码
	virtual WORD __cdecl GetTableID()=NULL;
	//游戏人数
	virtual WORD __cdecl GetChairCount()=NULL;
	//游戏时间
	virtual DWORD __cdecl GetPlayTimeCount()=NULL;

	//用户接口
public:
	//枚举用户
	virtual IServerUserItem * __cdecl EnumLookonUserItem(WORD wIndex)=NULL;
	//获取用户
	virtual IServerUserItem * __cdecl GetServerUserItem(WORD wChairID)=NULL;

	//状态接口
public:
	//获取状态
	virtual BYTE __cdecl GetGameStatus()=NULL;
	//设置状态
	virtual void __cdecl SetGameStatus(BYTE bGameStatus)=NULL;
	//获取配置
	virtual const tagGameServiceOption * __cdecl GetGameServiceOption()=NULL;

	//定时器接口
public:
	//设置定时器
	virtual bool __cdecl SetGameTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)=NULL;
	//删除定时器
	virtual bool __cdecl KillGameTimer(WORD wTimerID)=NULL;

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;

	//消息接口
public:
	//发送房间消息
	virtual bool __cdecl SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//发送游戏消息
	virtual bool __cdecl SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;

	//游戏接口
public:
	//解散游戏
	virtual bool __cdecl DismissGame()=NULL;
	//结束游戏
	virtual bool __cdecl ConcludeGame()=NULL;
	//写入积分
	virtual bool __cdecl WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo)=NULL;
	//写入积分
	virtual bool __cdecl WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, LONG lTaxScore)=NULL;
	//发送场景
	virtual bool __cdecl SendGameScene(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameManager INTERFACE_VERSION(2,1)
static const GUID IID_ITableFrameManager={0xc0994158,0xf4b4,0x462f,0xa8,0x84,0x85,0x13,0xe2,0xac,0xfe,0x31};

//框架管理接口
interface ITableFrameManager : public ITableFrame
{
	//配置接口
public:
	//初始化
	virtual bool __cdecl InitTableFrame(WORD wTableID, tagTableFrameParameter * pTableFrameParameter)=NULL;

	//信息接口
public:
	//锁定状态
	virtual bool __cdecl IsTableLocked()=NULL;
	//游戏状态
	virtual bool __cdecl IsGameStarted()=NULL;
	//游戏状态
	virtual bool __cdecl IsUserPlaying(IServerUserItem * pIServerUserItem)=NULL;

	//执行接口
public:
	//坐下动作
	virtual bool __cdecl PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//旁观动作
	virtual bool __cdecl PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//离开动作
	virtual bool __cdecl PerformStandUpAction(IServerUserItem * pIServerUserItem)=NULL;

	//用户事件
public:
	//断线事件
	virtual bool __cdecl OnUserOffLine(WORD wChairID)=NULL;
	//重进事件
	virtual bool __cdecl OnUserReConnect(WORD wChairID)=NULL;
	//请求断线
	virtual bool __cdecl OnUserReqOffLine(WORD wChairID)=NULL;

	//系统事件
public:
	//定时器事件
	virtual bool __cdecl OnEventTimer(WORD wTimerID, WPARAM wBindParam)=NULL;
	//游戏事件
	virtual bool __cdecl OnEventSocketGame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//框架事件
	virtual bool __cdecl OnEventSocketFrame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameSink INTERFACE_VERSION(3,1)
static const GUID IID_ITableFrameSink={0x53d985c5,0x623a,0x41df,0x89,0x76,0x27,0xb1,0x8d,0xc1,0x97,0x37};

//游戏桌子回调接口
interface ITableFrameSink : public IUnknownEx
{
	//管理接口
public:
	//初始化
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx)=NULL;
	//复位桌子
	virtual void __cdecl RepositTableFrameSink()=NULL;

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode()=NULL;
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID)=NULL;

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart()=NULL;
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret)=NULL;

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam)=NULL;
	//游戏消息
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//框架消息
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameEvent INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameEvent={0xb16717de,0xc243,0x4686,0xbb,0x8a,0xd0,0x37,0xb8,0xc2,0xc,0xf6};

//框架事件接口
interface ITableFrameEvent : public IUnknownEx
{
	//请求事件
public:
	//请求同意
	virtual bool __cdecl OnEventUserReqReady(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//请求断线
	virtual bool __cdecl OnEventUserReqOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//请求重入
	virtual bool __cdecl OnEventUserReqReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//请求坐下
	virtual bool __cdecl OnEventUserReqSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon)=NULL;
	//请求起来
	virtual bool __cdecl OnEventUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon)=NULL;

	//动作事件
public:
	//用户同意
	virtual bool __cdecl OnEventUserReady(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//用户断线
	virtual bool __cdecl OnEventUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//用户重入
	virtual bool __cdecl OnEventUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//用户坐下
	virtual bool __cdecl OnEventUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户起来
	virtual bool __cdecl OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IGameServiceFrame INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceFrame={0xafccfa53,0x6431,0x4b2a,0x98,0x29,0xf5,0xcf,0x15,0xdf,0xd5,0x3b};

//游戏服务框架
interface IGameServiceFrame : public IUnknownEx
{
	//状态接口
public:
	//关闭查询
	virtual bool __cdecl IsShallClose()=NULL;
	//私聊查询
	virtual bool __cdecl IsAllowWisper()=NULL;
	//聊天查询
	virtual bool __cdecl IsAllowRoomChat()=NULL;
	//聊天查询
	virtual bool __cdecl IsAllowGameChat()=NULL;
	//进入查询
	virtual bool __cdecl IsAllowEnterRoom()=NULL;
	//进入查询
	virtual bool __cdecl IsAllowEnterGame()=NULL;

	//状态通知
public:
	//发送状态
	virtual bool __cdecl SendTableStatus(WORD wTableID)=NULL;
	//发送分数
	virtual bool __cdecl SendUserScore(IServerUserItem * pIServerUserItem)=NULL;
	//发送状态
	virtual bool __cdecl SendUserStatus(IServerUserItem * pIServerUserItem)=NULL;

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;

	//定时器接口
public:
	//设置定时器
	virtual bool __cdecl SetTableTimer(WORD wTableID, WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)=NULL; 
	//删除定时器
	virtual bool __cdecl KillTableTimer(WORD wTableID, WORD wTimerID)=NULL;

	//管理接口
public:
	//删除用户
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//输出信息
	virtual void __cdecl ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IGameServiceManager INTERFACE_VERSION(2,1)
static const GUID IID_IGameServiceManager={0xda6b9efa,0x3c56,0x4ee1,0x96,0xce,0x90,0xe1,0x20,0x48,0xe5,0xe7};

//游戏服务器管理接口
interface IGameServiceManager : public IUnknownEx
{
	//创建桌子
	virtual void * __cdecl CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取属性
	virtual void __cdecl GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//修改参数
	virtual bool __cdecl RectifyServiceOption(tagGameServiceOption * pGameServiceOption)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IGameService INTERFACE_VERSION(2,1)
static const GUID IID_IGameService={0x1239db23,0x4d2e,0x4f35,0xbb,0x4e,0x8c,0x4c,0x5d,0x39,0xe1,0xa0};

//游戏服务接口
interface IGameService : public IUnknownEx
{
	//服务状态
	virtual bool __cdecl IsService()=NULL;
	//设置事件
	virtual bool __cdecl SetEventService(IUnknownEx * pIEventService)=NULL;
	//开始服务
	virtual bool __cdecl StartService(tagGameServiceParameter * pGameServiceParameter)=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
};

//////////////////////////////////////////////////////////////////////////

//游戏服务组件辅助类
class CGameServiceHelper : public CTempldateHelper<IGameService>
{
	//函数定义
public:
	//构造函数
	CGameServiceHelper(void) : CTempldateHelper<IGameService>(IID_IGameService,
		VER_IGameService,GAME_SERVICE_DLL_NAME,TEXT("CreateGameService")) { }
};

//////////////////////////////////////////////////////////////////////////

#endif