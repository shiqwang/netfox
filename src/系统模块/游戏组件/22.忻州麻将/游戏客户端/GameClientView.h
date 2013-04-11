#pragma once

#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "GameScoreWnd.h"

#include "TimerControl.h"
//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_SHOW_SZ				(WM_USER+102)						//show
#define IDM_TING_CARD	(WM_USER+103)						//听牌消息

#define IDM_RESET_UI			(WM_USER+113)                       //刷新UI
#define MYSLEF_VIDEO_W    80
#define MYSLEF_VIDEO_H    60
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;
public:
	CPoint m_ptNameNew[GAME_PLAYER];
	int xOffInt;
	int yOffInt;


public:
	int									m_tDice;						//骰子动画时间
	CTimerControl						m_Timer;						//时钟控件
	//画听标记
void ShowTingPic( CDC *pDC,int x, int y );
bool							m_bTingFlag[GAME_PLAYER];				//听牌标志
int		m_IntFirstTingFlag[GAME_PLAYER];//听后的第一张牌 
	//标志变量
protected:
	bool							m_bOutCard;							//出牌标志
	bool							m_bWaitOther;						//等待标志
	bool							m_bHuangZhuang;						//荒庄标志

	//游戏属性
protected:
	LONG							m_lCellScore;						//基础积分
	WORD							m_wBankerUser;						//庄家用户

	//用户状态
protected:
	BYTE							m_cbCardData;						//出牌麻将
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbUserAction[GAME_PLAYER];					//用户动作

	int									m_nDiceCount1;					///骰子数1
	int									m_nDiceCount2;					//骰子数2
	//位置变量
protected:
	CPoint							m_UserFlagPos[GAME_PLAYER];					//标志位置
	CSize								m_csActionDice1;				//骰子动画大小
	CSize								m_csActionDice2;				//骰子大小

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageWait;						//等待提示
	CSkinImage						m_ImageOutCard;						//出牌提示
	CSkinImage						m_ImageUserFlag;					//用户标志
	CSkinImage						m_ImageUserAction;					//用户动作
	CSkinImage						m_ImageActionBack;					//动作背景
	CSkinImage						m_ImageHuangZhuang;					//荒庄标志

	CSkinImage						m_ImageTingBack;
	CSkinImage							m_ActionDice1;					//骰子动画
	CSkinImage							m_ActionDice2;					//骰子动画定点
	//麻将控件
public:
	CHeapCard						m_HeapCard[GAME_PLAYER];						//堆立麻将
	CUserCard						m_UserCard[3];						//用户麻将
	CTableCard						m_TableCard[GAME_PLAYER];						//桌面麻将
	CWeaveCard						m_WeaveCard[GAME_PLAYER][GAME_PLAYER];					//组合麻将
	CDiscardCard					m_DiscardCard[GAME_PLAYER];					//丢弃麻将
	CCardControl					m_HandCardControl;					//手上麻将

	//控件变量
public:
	CSkinButton						m_btTingCard;						//听牌按钮
	CSkinButton						m_btStart;							//开始按钮
	CControlWnd						m_ControlWnd;						//控制窗口
	CGameScoreWnd					m_GameScoreWnd;						//结束窗口

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//骰子动画
	void  ActionDice1( CDC *pDC,int nFrame);
	//骰子动画2
	void ActionDice2( CDC *pDC,int pos1, int pos2 );
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
	//听牌按钮
	afx_msg void OnTingCard();
	//开始按钮
	afx_msg void OnStart();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);//鼠标移动
	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
