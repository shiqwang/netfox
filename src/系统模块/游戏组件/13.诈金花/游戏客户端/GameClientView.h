#pragma once

#include "Stdafx.h"
#include "GoldView.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "GoldControl.h"

//////////////////////////////////////////////////////////////////////////                          

//消息定义
#define IDM_START						WM_USER+100						//开始消息
#define IDM_ADD_GOLD					WM_USER+101						//加注消息
#define IDM_FOLLOW						WM_USER+102						//跟注消息
#define IDM_GIVE_UP						WM_USER+103						//放弃消息
#define IDM_SHOWHAND					WM_USER+104						//梭哈消息
#define IDM_KAIPAI  					WM_USER+105						//开牌消息
#define IDM_KANPAI                      WM_USER+107						//看牌消息  add

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;
public:
	bool                            mArrayKanPai[GAME_PLAYER+1];        //是否看牌状态 ADD
	bool                            myKanPaiBool[GAME_PLAYER+1];        //是否看牌状态 ADD

	//计算某玩家放入的数
	int GetOneUserInputAllMoney(int userInt);
	//视图变量
protected:
	LONG							m_lMaxGold;							//最高下注
	LONG							m_lBasicGold;						//最少分数

	//界面变量
protected:
	CSkinImage						m_ImageKanPaiLog;					//看牌标记
	CSkinImage						m_ImageTitle;						//标题位图
	CSkinImage						m_ImageBarMid;						//控制位图
	CSkinImage						m_ImageBarLeft;						//控制位图
	CSkinImage						m_ImageBarRight;					//控制位图
	CSkinImage						m_ImageGlodTable;					//金币位图
	CSkinImage						m_ImageBackGround;					//背景位图
	CSkinImage						m_ImageBack;                        //背景 ADD
	CPoint							m_GoldStation[2*GAME_PLAYER];		//筹码位置
	CSkinImage                      m_BankerView;                       //庄家视图 ADD
	WORD                            m_Banker;                           //ADD  当前庄家
	CPoint							m_UserFlagPos[GAME_PLAYER];         //ADD  庄家标志在4个方位的坐标
	
	//控件变量
public:
	TransButton						m_btStart;							//开始按钮
	TransButton						m_btFollow;							//跟注按钮
	TransButton						m_btGiveUp;							//放弃按钮
	TransButton						m_btAddGold;						//加注按钮
	CSkinButton						m_btShowHand;						//梭哈按钮
	CGoldView						m_GoldView[2*GAME_PLAYER];			//用户筹码
	CScoreView						m_ScoreView;						//积分视图
	CGoldControl					m_GoldControl;						//加注视图
	CCardControl					m_CardControl[GAME_PLAYER];			//扑克控件
	TransButton						m_btKanPai;							//看牌按钮 //add
	TransButton						m_btKaiPai;							//开牌按钮 //add
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

	//功能函数
public:
	//金币标题
	void SetGoldTitleInfo(DWORD lMaxGold, DWORD lBasicGold);
	//用户筹码
	void SetUserGoldInfo(WORD wViewChairID, bool bTableGold, DWORD dwGold);
	//从里CGameClientDlg类获得MyCurrentUser的值
	void GetCurrentUser(WORD MyCurrentUser);
	
	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//跟注按钮
	afx_msg void OnFollow();
	//放弃按钮
	afx_msg void OnGiveUp();
	//加注按钮
	afx_msg void OnAddGold();
	//梭哈按钮
	afx_msg void OnShowHand();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//看牌按钮
	afx_msg void KANPAI();
	//开牌按钮
	afx_msg void KaiPai();

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
