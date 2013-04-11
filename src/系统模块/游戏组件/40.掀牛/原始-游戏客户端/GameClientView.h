#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "..\游戏服务器\GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_COUPAI					(WM_USER+103)						//扣牌消息
#define IDM_AUTO_OUTCARD			(WM_USER+104)                       //提示消息
#define IDM_XIANPAI					(WM_USER+105)						//掀牌消息
#define IDM_GIVEUP_QIANGCI			(WM_USER+106)						//放弃抢刺
#define IDM_QIANGCI					(WM_USER+107)						//抢刺消息
#define IDM_FORETURN_CARD			(WM_USER+108)						//上一轮提示

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:
	WORD							m_wBanker;						//地主用户
	LONG							m_lBaseScore;						//基础分数
	BYTE							m_bZhanPaiCount[GAME_PLAYER];		//占牌张数
protected:
	long							m_lForeScore[GAME_PLAYER];			//上一局成绩
	long							m_lAllScore[GAME_PLAYER];			//总成绩
protected:
	static long							m_lTimeCount;					//计时器

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序

	//状态变量
public:
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bScore[GAME_PLAYER];				//用户叫分
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	bool							m_bCouPaiTitle[GAME_PLAYER];		//扣牌标志
	bool							m_bXianPaiTitle[GAME_PLAYER];		//掀牌标志
	bool							m_bQiangCiTitle[GAME_PLAYER];		//抢刺标志
protected:
	BYTE							m_bXianCount[GAME_PLAYER];			//掀人数组
	BYTE							m_bBeiXianCount[GAME_PLAYER];		//被掀数组

	//位置信息
public:
	CSize							m_LandSize;							//挖坑标志
	CPoint							m_ptScore[GAME_PLAYER];				//叫分位置
	CPoint							m_ptLand[GAME_PLAYER];				//反牌位置
	CPoint							m_pPass[GAME_PLAYER];
	CPoint							m_pZhanPaiName[GAME_PLAYER];		//占牌用户名
	CPoint							m_pTimeFlag;						//时间标志

	//位图变量
protected:
	CSkinImage						m_ImageBomb;						//炸弹图片
	CSkinImage						m_ImageLand;						//庄家图片
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageScore;						//分数图片
	CSkinImage						m_ImageCenter;						//背景资源
	CSkinImage						m_ImageLc;							//左上角标志
	CSkinImage						m_ImageHeadBg;						//头像背景
	CSkinImage						m_ImageUserPass;					//放弃标志
	CSkinImage						m_ImageTime;						//时间标志
	CSkinImage						m_ImageTime2;						//时间段
	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btKouPai;
	CSkinButton						m_btXianPai;
	CSkinButton						m_btQiangCi;					
	CSkinButton						m_btGiveUpCiangCi;					//放弃按钮
	CSkinButton						m_btAutoOutCard;					//提示按钮
	CSkinButton						m_btForeTurn;						//上轮提示

	//扑克控件
public:
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_ZhanCardControl[GAME_PLAYER];		//占牌扑克
	CCardControl					m_BackCardControl;					//底牌扑克
	CCardControl					m_UserCardControl[3];				//扑克视图
	CCardControl					m_LeaveCardControl[2];				//结束扑克
	CCardControl					m_ForeTurn[GAME_PLAYER];			//上一轮扑克

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);
	//绘画时间标志
	virtual void DrawTime(CDC * pDC,int nWidth,int nHeight);

	//配置函数
public:
	//用户玩一局要清理的
	void ClearPlayerOneGame();
	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//功能函数
public:

	//基础分数
	void SetBaseScore(LONG lBaseScore);
	//显示提示
	void ShowKouPaiTitle(bool bCouPaiTitle,WORD wChairID);
	//显示提示
	void ShowXianPaiTitle(bool bXianPaiTitle,WORD wChairID);
	//显示提示
	void ShowQiangCiTitle(bool bQiangCiTitle,WORD wChairID);
	//清理显示
	void HideAllTitle(bool bHide,WORD wChairID);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);
	//设置占牌数目
	void IncreaseZhanPaiCount(WORD wChairID,BYTE bCardCount);
	//设置上一局成绩
	void SetForeScore(WORD wChairID,LONG lScore);
	//设置总成绩
	void SetAllScore(WORD wChairID,LONG lScore);
	//设置时间
	void SetTimeFlag(LONG lTimeCount,DWORD dCurrentTime);
	//设置掀牌增加
	void SetIncreaseXian(WORD wChairID);
	//设置被掀增加
	void SetIncreaseBeiXian(WORD wChairID);

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();
	//叫分按钮
	afx_msg void OnKouPai();
	//叫分按钮
	afx_msg void OnXianPai();
	//叫分按钮
	afx_msg void OnQiangCi();
	//放弃按钮
	afx_msg void OnGiveUpQingCi();
	//上一轮提示
	afx_msg void OnForTurn();
	//出牌提示
	afx_msg void OnAutoOutCard();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
