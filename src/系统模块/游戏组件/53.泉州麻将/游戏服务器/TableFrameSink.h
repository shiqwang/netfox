#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"


//////////////////////////////////////////////////////////////////////////
//枚举定义

//效验类型
enum enEstimatKind
{
	EstimatKind_OutCard,			//出牌效验
	EstimatKind_GangCard,			//杠牌效验
};



//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableFrameEvent
{
	//游戏变量
protected:
	WORD							m_wDoorDiceCount;						//开门骰子点数
	WORD							m_wGoldDiceCount;						//开金骰子点数
	WORD							m_wBankerUser;							//庄家用户
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//用户麻将
	//BYTE							m_cbDisPengIndex[GAME_PLAYER][MAX_INDEX];//碰牌中的臭牌
	//BYTE							m_cbDisHuIndex[GAME_PLAYER][MAX_INDEX];	//胡牌中的臭牌
	enGoldSwim						m_GoldSwim;								//游牌状态
	bool							m_bGameStart;							//开始标志
	enGoldSwim						m_UserGoldSwim[GAME_PLAYER];			//游牌状态
	BYTE							m_cbCurCardIndex[GAME_PLAYER][MAX_INDEX];//玩家扑克 



	//出牌信息
protected:
	WORD							m_wOutCardUser;							//出牌用户
	BYTE							m_cbOutCardData;						//出牌麻将
	BYTE							m_cbOutCardCount;						//出牌数目
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];		//丢弃记录
	BYTE							m_cbMaxLeftCount;						//留牌张数

	

	//发牌信息
protected:
	BYTE							m_cbSendCardData;						//发牌麻将
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbLeftCardCount;						//剩余麻将
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存麻将
	BYTE							m_cbGoldCard[4];						//金牌麻将
	BYTE							m_cbRepairFlowerPos;					//补花位置
	BYTE							m_cbFlowerCard[8];						//花牌麻将
	BYTE							m_cbFlowerCount[GAME_PLAYER];			//花朵个数
	BYTE							m_cbGangCount[GAME_PLAYER];				//杠牌翻数
	BYTE							m_cbGoldCount[GAME_PLAYER];				//金牌个数
	BYTE							m_cbDoorCard;							//门牌麻将


	//运行变量
protected:
	WORD							m_wResumeUser;							//还原用户
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wProvideUser;							//供应用户
	BYTE							m_cbProvideCard;						//供应麻将
	bool							m_bHuUser[4];							//胡牌用户

	

	//状态变量
protected:
	bool							m_bSendStatus;							//发牌状态
	bool							m_bGangStatus;							//杆上胡状态
	bool							m_bEnjoinChiHu[GAME_PLAYER];			//禁止吃胡
	bool							m_bEnjoinChiPeng[GAME_PLAYER];			//禁止吃碰
	BYTE							m_bGoOnBankerTime;						//连庄次数

	
	
	

	//用户状态
public:
	bool							m_bResponse[GAME_PLAYER];				//响应标志
	BYTE							m_cbUserAction[GAME_PLAYER];			//用户动作
	BYTE							m_cbOperateCard[GAME_PLAYER];			//操作麻将
	BYTE							m_cbPerformAction[GAME_PLAYER];			//执行动作
	LONG							m_lUserCapital[GAME_PLAYER];			//玩家资本
	

	//组合麻将
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][5];		//组合麻将

	//结束信息
protected:
	BYTE							m_cbChiHuCard;							//吃胡麻将
	tagChiHuResult					m_ChiHuResult[GAME_PLAYER];				//吃胡结果

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const enStartMode		m_GameStartMode;						//开始模式
	//test
	char							*msg;									//和牌消息


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
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//请求事件
public:
	//请求同意
	virtual bool __cdecl OnEventUserReqReady(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求断线
	virtual bool __cdecl OnEventUserReqOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求重入
	virtual bool __cdecl OnEventUserReqReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求坐下
	virtual bool __cdecl OnEventUserReqSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }
	//请求起来
	virtual bool __cdecl OnEventUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }

	//动作事件
public:
	//用户同意
	virtual bool __cdecl OnEventUserReady(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户断线
	virtual bool __cdecl OnEventUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool __cdecl OnEventUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) {return true;}
	//用户坐下
	virtual bool __cdecl OnEventUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool __cdecl OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);

	//游戏事件
protected:
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData);
	//用户操作
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard);

	//辅助函数
protected:
	//发送操作
	bool SendOperateNotify();
	//派发麻将
	bool DispatchCardData(WORD wCurrentUser,bool bRepairFlower);
	//响应判断
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);	
	//游牌状态
	enGoldSwim GetCurrentGoldSwim();
	//游牌状态
	enGoldSwim GetUserGoldSwim(WORD wChairID);
	
	
	
public:
	// 开门消息
	bool OnUserOpenDoor();
	// 开金消息
	bool OnUserOpenGold();	
	// 补花消息
	bool OnUserRepairFlower(const void *pBuffer);
};

//////////////////////////////////////////////////////////////////////////

#endif