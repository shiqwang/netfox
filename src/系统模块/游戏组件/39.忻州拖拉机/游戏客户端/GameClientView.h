#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CallCardWnd.h"
#include "CardControl.h"
#include "ConcealCardView.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

//按钮消息
#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_OUT_PROMPT				(WM_USER+102)						//提示消息
#define IDM_SEND_CONCEAL			(WM_USER+103)						//留底消息

//功能按钮
#define IDM_CONCEAL_CARD			(WM_USER+110)						//查看底牌
#define IDM_REQUEST_LEAVE			(WM_USER+111)						//请求离开
#define IDM_LAST_TURN_CARD			(WM_USER+112)						//上轮扑克
#define IDM_TRUSTEE_CONTROL			(WM_USER+113)						//托管控制

#define IDM_RESET_UI			(WM_USER+114)                       //刷新UI
#define MYSLEF_VIDEO_W    80
#define MYSLEF_VIDEO_H    60
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
public:
	CPoint m_ptNameNew[GAME_PLAYER];

	//显示系统抽牌信息
	int isSysChouInt;//1=系统抽牌。。。
	CString strChouStr;//显示信息

	int xOffInt;
	int yOffInt;
	//显示变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序
	bool							m_bWaitConceal;						//等待标志
	bool							m_bLastTurnCard;					//上轮扑克

	//游戏变量
protected:
	WORD							m_wCardScore;						//玩家得分
	WORD							m_wBankerUser;						//庄家用户

	//叫牌信息
protected:
	BYTE							m_cbCallColor;						//叫牌花色   本次打的花色
	BYTE							m_cbCallCount;						//叫牌数目
	BYTE							m_cbMainValue;						//主牌数值
	WORD							m_wCallCardUser;					//叫牌用户
	BYTE							m_cbValueOrder[2];					//主牌数值

	//位置信息
protected:
	CSize							m_SizeBanker;						//庄家大小
	CSize							m_SizeStation;						//状态大小
	CPoint							m_PointBanker[GAME_PLAYER];			//庄家位置

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageBackGround;					//背景资源
	CSkinImage						m_ImageBanker;						//庄家资源
	CSkinImage						m_ImageLastTurn;					//上轮标志
	CSkinImage						m_ImageColorFlag;					//花色标志
	CSkinImage						m_ImageWaitConceal;					//等待资源
	CSkinImage						m_ImageStationBack;					//状态资源
	CSkinImage						m_ImageStationTitle;				//状态资源

	//按钮控件
public:
	TransButton						m_btStart;							//开始按钮
	TransButton						m_btOutCard;						//出牌按钮
	TransButton						m_btOutPrompt;						//提示按钮
	TransButton						m_btSendConceal;					//留底按钮

	//功能按钮
public:
	TransButton						m_btConcealCard;					//查看底牌
	TransButton						m_btRuquestLeave;					//请求离开
	TransButton						m_btLastTurnCard;					//上轮扑克
	TransButton						m_btStusteeControl;					//拖管控制

	//扑克控件
public:
	CCardControl					m_CardScore;						//得分扑克
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_UserCardControl[4];				//扑克视图

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CCallCardWnd					m_CallCardWnd;						//叫牌窗口
	CConcealCardView				m_ConcealCardView;					//底牌窗口

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

	//配置函数
public:
	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//游戏信息
public:
	//设置得分
	void SetCardScore(WORD wCardScore);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//设置等待
	void SetWaitConceal(bool bWaitConceal);
	//设置上轮
	void SetLastTurnCard(bool bLastTurnCard);
	//设置主牌
	void SetValueOrder(BYTE cbMainValue, BYTE cbValueMySelf, BYTE cbValueOther);
	//设置亮主
	void SetCallCardInfo(WORD wCallCardUser, BYTE cbCallColor, BYTE cbCallCount);

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//提示按钮
	afx_msg void OnOutPrompt();
	//留底按钮
	afx_msg void OnSendConceal();
	//查看底牌
	afx_msg void OnConcealCard();
	//请求离开
	afx_msg void OnRuquestLeave();
	//上轮扑克
	afx_msg void OnLastTurnCard();
	//拖管控制
	afx_msg void OnStusteeControl();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
