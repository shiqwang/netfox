#ifndef GameRank_HEAD_FILE
#define GameRank_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "..\..\模板库\Template.h"

//全局头文件
#include "..\..\公共文件\GlobalDef.h"

//组件头文件
#include "..\..\组件接口\IUnknownEx.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef GAMERANK_CLASS
	#ifdef  GAMERANK_DLL
		#define GAMERANK_CLASS _declspec(dllexport)
	#else
		#define GAMERANK_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define GAMERANK_DLL_NAME	TEXT("GameRankD.dll")					//组件 DLL 名字
#else
	#define GAMERANK_DLL_NAME	TEXT("GameRank.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#define VER_IGameRankManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameRankManager={0x850e5617,0x4eab,0x4f2e,0xae,0x11,0xef,0xa3,0x22,0x65,0xe0,0xc2};

//游戏等级接口
interface IGameRankManager : public IUnknownEx
{
	//获取等级
	virtual LPCTSTR __cdecl GetGameRank(WORD wKindID, LONG lScore, WORD wGameGenre)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//关系管理辅助类
class CGameRankManagerHelper : public CTempldateHelper<IGameRankManager>
{
	//函数定义
public:
	//构造函数
	CGameRankManagerHelper(void) : CTempldateHelper<IGameRankManager>(IID_IGameRankManager,
		VER_IGameRankManager,GAMERANK_DLL_NAME,TEXT("CreateGameRankManager")) { }
};

//////////////////////////////////////////////////////////////////////////

#endif