#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//用户变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户

	//状态变量
protected:
	bool							m_bAuto;
	WORD							m_wDoorDiceCount;					//开门骰子大小
	WORD							m_wGoldDiceCount;					//开金骰子大小
	BYTE							m_cbSubtractTailCount;				//尾部取麻将
	BYTE							m_cbSubtractHeadCount;				//头部取麻将
	
	//堆立变量
protected:						
	WORD							m_wHeapHand;						//堆立头部
	WORD							m_wHeapTail;						//堆立尾部
	BYTE							m_cbHeapCardInfo[4][2];				//堆牌信息


	//扑克变量
protected:
	BYTE							m_cbLeftCardCount;					//剩余数目
	BYTE							m_cbCardIndex[MAX_INDEX];			//手中扑克
	BYTE							m_cbFlowerCard[8];					//花牌麻将

	//组合扑克
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][5];	//组合扑克

	//出牌信息
protected:
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbOutCardData;					//出牌扑克
	BYTE							m_cbDiscardCount[GAME_PLAYER];		//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];	//丢弃记录
	BYTE							m_cbGoldCard[4];					//金牌扑克

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图

//test
protected:
	//char							*m_cMsg;						//杠信息
	
	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();
	//游戏设置
	virtual void OnGameOptionSet();
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID);
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	// 开门状态
	bool OnSubOpenDoor(const void * pBuffer, WORD wDataSize);
	// 开金状态
	bool OnSubOpenGold(const void * pBuffer, WORD wDataSize);
	// 补花结束
	bool OnSubEndFlower(const void * pBuffer, WORD wDataSize);
	// 游牌状态
	bool OnSubGoldSwim(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//播放声音
	void PlayCardSound(BYTE cbCardData);
	//播放声音
	void PlayActionSound(BYTE cbAction);
	//扣除扑克
	void DeductionTableCard(bool bHeadCard);
	//显示控制
	bool ShowOperateControl(BYTE cbUserAction, BYTE cbActionCard);

	//消息映射
protected:
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌操作
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//扑克操作
	LRESULT OnCardOperate(WPARAM wParam, LPARAM lParam);
	//代打操作
	LRESULT OnAuto(WPARAM wParam, LPARAM lParam);
	//开门操作
	LRESULT OnOpenDoor(WPARAM wParam,LPARAM lParam);
	//开金操作
	LRESULT OnOpenGold(WPARAM wParam,LPARAM lParam);	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);

	
	
	
};

//////////////////////////////////////////////////////////////////////////
