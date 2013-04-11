#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//梭哈扑克类型
#define TeShu                               0
#define SHAN								1
#define DUI_ZI								2
#define SHUN_ZI								3
#define	TONG_HUA							4
#define TONG_HUA_SHUN						5
#define SAN_TIAO							6


//扑克花色类型
#define FANG_KUAI							0
#define MEI_HUA								1
#define HONG_TAO							2
#define HEI_TAO								3

//结束原因
#define GER_NO_PLAYER						0x10						//没有玩家


//游戏逻辑类
class CGameLogic
{
	//变量定义

	//变量定义
private:
	static const BYTE				m_bCardArray[52];					//扑克定义

	//基础函数
private:
	//获取扑克花色
	BYTE GetHuaKind(BYTE bCard);
	//获取扑克数值
	BYTE GetCardValoe(BYTE bCard);
	//获取扑克类型
	BYTE GetCardKind(BYTE bCardList[], BYTE bCardCount);
	//查找对牌
	BYTE FindDoubleCard(BYTE bCardList[], BYTE bCardCount, BYTE * bOutDoubleCard);

	//类型函数
public:
	//是否同花顺
	bool IsTongHuaShun(BYTE bCardList[], BYTE bCardCount);
	//是否铁支
	bool IsTieZhi(BYTE bCardList[], BYTE bCardCount);
	//是否葫芦
	bool IsHuLu(BYTE bCardList[], BYTE bCardCount);
	//是否同花
	bool IsTongHua(BYTE bCardList[], BYTE bCardCount);
	//是否顺子
	bool IsShunZhi(BYTE bCardList[], BYTE bCardCount);
	//是否三条
	bool IsSanTiao(BYTE bCardList[], BYTE bCardCount);
	//是否两对
	bool IsLiangDui(BYTE bCardList[], BYTE bCardCount);
	//是否对子
	bool IsDuiZhi(BYTE bCardList[], BYTE bCardCount);
	//特殊牌
	bool IsTeShu(BYTE bCardList[], BYTE bCardCount);
	

	//对比函数
public:
	//对比单只扑克
	bool CompareOnlyOne(BYTE bFirstCard, BYTE bLastCard);
	//对比同花顺
	bool CompareTongHuaShun(BYTE bFirstList[], BYTE bLastList[]);
	//对比铁支
	bool CompareTeiZhi(BYTE bFirstList[], BYTE bLastList[]);
	//对比葫芦
	bool CompareHuLu(BYTE bFirstList[], BYTE bLastList[]);
	//对比同花
	bool CompareTongHua(BYTE bFirstList[], BYTE bLastList[]);
	//对比顺子
	bool CompareShunZhi(BYTE bFirstList[], BYTE bLastList[]);
	//对比三条
	bool CompareSanTiao(BYTE bFirstList[], BYTE bLastList[]);
	//对比两对
	bool CompareLiangDui(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount);
	//对比对子
	bool CompareDuiZhi(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount);
	//对比散牌
	bool CompareSanCard(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount);
	

	//功能函数
public:
	//混乱扑克
	void RandCard(BYTE bCardBuffer[], BYTE bBufferCount);
	//排列扑克
	void SortCard(BYTE bCardList[], BYTE bCardCount);
	//对比扑克
	bool CompareCard(BYTE bFirstList[], BYTE bLastList[],BYTE bCardCount);
};

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink
{
	//游戏变量
protected:
	
	WORD							m_wOperaCount;							//操作次数
	WORD							m_wCurrentUser;							//当前用户
	BYTE							m_bSendCardCount;						//发牌数目
	BYTE							m_bTableCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_bTableCardArray[GAME_PLAYER][3];		//桌面扑克    //been 5
	bool                            start;                                  //游戏开始或下一轮状态  //add
	BYTE                            Winer;                                  //赢家椅子号
	//金币信息
protected:
	LONG							m_lMaxGold;								//最大下注
	LONG							m_lTurnBasicGold;						//最少下注
	LONG							m_lTableGold[2*GAME_PLAYER];			//下注金币

	//辅助变量
protected:
	BYTE							m_bPlayStatus[GAME_PLAYER];				//游戏状态
	bool                            m_bKanPai[GAME_PLAYER+1];               //看牌状态   
	
	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const enStartMode		m_GameStartMode;						//开始模式

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
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

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
	//放弃事件
	bool OnUserGiveUp(WORD wChairID);
	//加注事件
	bool OnUserAddGold(WORD wChairID, LONG lGold, bool bCancel);
	//看牌事件
	bool OnUserKanPai();       //add
	bool SetBeginOver();       //add
	

	//游戏函数
protected:
	//推断胜者
	WORD DeduceWiner(BYTE bBeginPos, BYTE bEndPos);												   //delete
	
	WORD DeduceWiner();																			   //add
};

//////////////////////////////////////////////////////////////////////////


#endif