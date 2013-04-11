#pragma once

#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "GameScoreWnd.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_AUTO					(WM_USER+102)						//代打按钮
#define IDM_OPEN_DOOR				(WM_USER+103)						//开门消息
#define IDM_OPEN_GOLD				(WM_USER+104)						//开金消息


//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//标志变量
protected:
	bool							m_bOutCard;							//出牌标志
	bool							m_bWaitOther;						//等待标志
	bool							m_bHuangZhuang;						//荒庄标志
	POINT							m_ptGold;
	CSize							m_csGlod;							//金币图大小	
	bool							m_bAutoEnable;						//代打标志
	bool							m_bGoldCardShow;					//金牌显示	
	bool							m_bGoldOutEnable;					//金牌使能

	//游戏属性
protected:
	LONG							m_lCellScore;						//基础积分
	WORD							m_wBankerUser;						//庄家用户

	//用户状态
protected:
	BYTE							m_cbCardData;						//出牌扑克
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbUserAction[5];					//用户动作

	//骰子变量
protected:
	WORD							m_wDoorDice;						//开门点数
	WORD							m_wGoldDice;						//开金点数


	//位置变量
protected:
	CPoint							m_UserFlagPos[4];					//标志位置

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageWait;						//等待提示
	CSkinImage						m_ImageOutCard;						//出牌提示
	CSkinImage						m_ImageUserFlag;					//用户标志
	CSkinImage						m_ImageUserAction;					//用户动作
	CSkinImage						m_ImageActionBack;					//动作背景
	CSkinImage						m_ImageBirdBack;					//鸟图背景
	CSkinImage						m_ImageHuangZhuang;					//荒庄标志
	CSkinImage						m_ImageGold;						//金B位置
	CSkinImage						m_BackGround;						//底色
	CSkinImage						m_ImageChuiFlag;					//捶牌图案
	CSkinImage						m_ImageGoldCard;					//金牌背景

	//扑克控件
public:
	CHeapCard						m_InsideHeapCard[4];				//内排扑克
	CHeapCard						m_OutSideHeapCard[4];				//外排扑克
	CUserCard						m_UserCard[3];						//用户扑克
	CTableCard						m_TableCard[4];						//桌面扑克
	CWeaveCard						m_WeaveCard[4][5];					//组合扑克
	CDiscardCard					m_DiscardCard[4];					//丢弃扑克
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_GoldCardControl;					//金牌扑克
	

	//控件变量
public:
	TransButton						m_btStart;							//开始按钮
	//CSkinButton						m_btAutoOn;							//代打ON按钮
	//CSkinButton						m_btAutoOff;						//代打OFF按钮
	CControlWnd						m_ControlWnd;						//控制窗口
	CGameScoreWnd					m_GameScoreWnd;						//结束窗口
	TransButton						m_btOpenDoor;						//开门按钮
	TransButton						m_btOpenGold;						//开金按钮

	//提示控件
public:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件


	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);
	void  SetImageGold( CDC *pDC, BYTE byPlayer, const tagUserData* pUserData );
	void SetArtText( CDC *pDC, int x, int y, COLORREF nColor, COLORREF nBkColor, LPCTSTR lpszString );

	//功能函数
public:
	//基础积分
	void SetCellScore(LONG lCellScore);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//荒庄设置
	void SetHuangZhuang(bool bHuangZhuang);
	//状态标志
	void SetStatusFlag(bool bOutCard, bool bWaitOther);
	//出牌信息
	void SetOutCardInfo(WORD wViewChairID, BYTE cbCardData);
	//动作信息
	void SetUserAction(WORD wViewChairID, BYTE bUserAction);

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//代打ON按钮
	afx_msg void OnAutoOn();
	//代打OFF按钮
	afx_msg void OnAutoOff();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//开门消息
	afx_msg void OnOpenDoor();
	//开金消息
	afx_msg void OnOpenGold();

	DECLARE_MESSAGE_MAP()

public:
	
	// 听牌显示
	void SetImageTing( CDC *pDC, BYTE byPlayer);
	//设置金牌显示
	void SetGoldCardShow(bool bGoldCardShow);
	//金牌使能
	void SetGoldOutEnable(bool bGoldOutEnable){m_bGoldOutEnable=bGoldOutEnable;}
	//开门点数
	void SetDoorDice(WORD wDoorDice){m_wDoorDice=wDoorDice;}
	//开金点数
	void SetGoldDice(WORD wGoldDice){m_wGoldDice=wGoldDice;}
	
	
};

//////////////////////////////////////////////////////////////////////////
