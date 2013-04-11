#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

/*
问题：
1.         没有停牌。
//.听牌。。。。。

2.         没有黄牌，当牌只剩下10层（即共20张牌）时就应出现黄牌。
//剩余张数


3.         色子只打一遍，必须打两遍


4.         自己打出去的牌，只能自摸，别人打出来的时候，不能再胡牌。

5.         界面显示无牌后还能继续打牌。

1。色子。。。。 撒2次。。。
庄家。。。。
6+7。。。。

17*
14*。。。。

1.庄家甩色子-----》从哪家起拿    12
2.再甩色子------>保留多少。。。  12+12

2。听牌。。。。。

3。怎么当庄。。。。
轮流当庄。。。

1。计算分数不一样。。。。
2。打过的不能胡。。。。。
自己打过的不能胡。。。。

3.碰牌。。。

4.剩10对牌，就黄牌。。。。

5.连黄3盘就赔。。。
*/
//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
private:
	//是否第一次移动
	int uiShowInt;
	//显示消息
    int ShowMessageBox(LPCTSTR pszMessage, UINT nType);
	
	CMD_S_GameStart  myGameStart;
	BYTE cbSiceFirst;//第一个
	BYTE cbSiceSecond;//第二个

	BYTE cbSice2First;//第一个
	BYTE cbSice2Second;//第二个

public:
//显示用户MJ
	void ShowUserMJ();
	//调整视频窗口
	void ResetVideoWindowPostion(int wInt, int hInt);
	//用户变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户

	//状态变量
protected:
	bool							m_bHearStatus;						//听牌状态

	//堆立变量
protected:
	WORD							m_wHeapHand;						//堆立头部
	WORD							m_wHeapTail;						//堆立尾部
	BYTE							m_cbHeapCardInfo[4][2];				//堆牌信息

	//麻将变量
protected:
	BYTE							m_cbLeftCardCount;					//剩余数目
	BYTE							m_cbCardIndex[MAX_INDEX];			//手中麻将

	//组合麻将
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];	//组合麻将

	//出牌信息
protected:
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbOutCardData;					//出牌麻将
	BYTE							m_cbDiscardCount[GAME_PLAYER];		//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];	//丢弃记录

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
public:
	CGameClientView					m_GameClientView;					//游戏视图
	
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
	//听牌
	bool OnSubTingCard(const void * pBuffer, WORD wDataSize);
	//辅助函数
protected:
	//播放声音
	//默认为男声
	void PlayCardSound(BYTE cbCardData, int sexInt = 1);
	//播放声音
	void PlayActionSound(BYTE cbAction , int sexInt = 1);
	//扣除麻将
	void DeductionTableCard(bool bHeadCard);
	//显示控制
	bool ShowOperateControl(BYTE cbUserAction, BYTE cbActionCard);

	//消息映射
protected:
	//
	LRESULT OnShowSZ(WPARAM wParam, LPARAM lParam);
	//听牌按钮
	LRESULT OnTingCard(WPARAM wParam, LPARAM lParam);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌操作
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//麻将操作
	LRESULT OnCardOperate(WPARAM wParam, LPARAM lParam);

	//RESET UI
	LRESULT OnResetUI(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
