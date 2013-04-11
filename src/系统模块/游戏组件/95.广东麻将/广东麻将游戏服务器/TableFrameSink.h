#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\消息定义\CMD_Mj.h"
#include "GameLogic.h"
#include "CLog.h"
//#include <vector>

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink
{
	//日志
	CLog							log;

	//变量定义
private:
	bool							m_bZiM;											//是否自摸
	WORD							m_wBankerUser;									//庄家
	WORD							m_wCurrentUser;									//当前玩家
	WORD							m_wLastUser;									//上轮出牌玩家
	WORD							m_wLostUser;									//放炮玩家
	long							m_nFang[GAME_PLAYER];							//胡牌番数

	char							*m_strFang;										//算番符
	BYTE							m_wQFeng;										//圈风

	bool							m_bTianHu;										//天胡可能
	bool							m_bDiHu[GAME_PLAYER];							//地胡可能
	bool							m_bRenHu[GAME_PLAYER];							//人胡可能
	//运行信息
private:
	WORD							m_wTurnWiner;									//胜利玩家
	BYTE							m_byOutMj;										//所出牌
	int								m_ActionRight[GAME_PLAYER];						//操作权限,6-胡,4-杠,3-碰,																						  2-吃,1-等待, 0-取消

	//麻将信息
private:
	static  BYTE					m_byMjs[ORG_MJ_MAX];							//麻将牌数据
	BYTE							m_byTouchMjCount;								//摸牌区麻将个数
	int								m_nTouchMjPos;									//摸牌位置
	BYTE							m_byHandMjCount[GAME_PLAYER];					//玩家手上牌数
	BYTE							m_byHandMjs[GAME_PLAYER][HAND_MJ_MAX];			//手上麻将
	HU								m_hu[GAME_PLAYER];								//胡牌面
	HU								m_tempHu[GAME_PLAYER];							//临时胡牌面

	//组件变量
protected:
	CGameLogic						m_GameLogic;									//游戏逻辑
	ITableFrame						* m_pITableFrame;								//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;							//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;									//游戏人数
	static const enStartMode		m_GameStartMode;								//开始模式

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void __cdecl RepositTableFrameSink();

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode();
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart();
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//游戏事件
protected:
	//用户出牌
	bool OnUserOutMj( WORD wChairID, BYTE *byOutMj );
	//用户摸牌
	bool OnUserTouchMj( WORD wChairID );
	//用户放弃
	bool OnUserGiveUp( WORD wChairID, const void *pDataBuffer);
	//用户碰牌
	bool OnUserPeng( WORD wChairID, const void *pDataBuffer );
	//用户吃牌
	bool OnUserChi( WORD wChairID, const void *pDataBuffer );
	//用户杠牌
	bool OnUserGang( WORD wChairID, const void *pDataBuffer );
	//用户吃牌
	bool OnUserHu( WORD wChairID, const void *pDataBuffer );
	//用户吃牌选择
	bool OnUserChiSelect( WORD wChairID, int nChiType);
	//用户选择杠牌
	bool OnUserGangSelect( WORD wChairID, const void *pDataBuffer );

	//流局
	bool OnGameEndEmpty();
};

//////////////////////////////////////////////////////////////////////////

#endif