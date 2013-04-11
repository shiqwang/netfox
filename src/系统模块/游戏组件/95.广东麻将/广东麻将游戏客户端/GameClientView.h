#pragma once

#include "Stdafx.h"
#include <vector>
#include "GameLogic.h"
#include "TimerControl.h"
#include "ScoreView.h"
#include "GameParrentView.h"

//////////////////////////////////////////////////////////////////////////
//消息定义
#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_MJ					(WM_USER+101)						//出牌消息
#define IDM_ADDFLOWER_MJ			(WM_USER+102)						//补花消息
#define IDM_HU_MJ					(WM_USER+103)						//胡牌消息
#define IDM_TING_MJ					(WM_USER+104)						//听牌消息
#define IDM_GANG_MJ					(WM_USER+105)						//杠牌消息
#define IDM_PENG_MJ					(WM_USER+106)						//碰牌消息
#define IDM_CHI_MJ					(WM_USER+107)						//吃牌消息
#define IDM_GIVEUP					(WM_USER+108)						//放弃消息
#define IDM_AUTO_OUTMJ				(WM_USER+109)                       //自动出牌

#define IDM_LEFTHIT_MJ				(WM_USER+110)						//左键出牌
#define IDM_RIGHTHIT_MJ				(WM_USER+111)						//右键出牌

#define IDM_SELECT_CHI				(WM_USER+112)						//选择吃牌
#define IDM_SELECT_GANG				(WM_USER+113)						//选择杠牌

#define IDM_CANCEL2					(WM_USER+114)						//关闭消息
#define IDM_SHOWMINI				(WM_USER+115)						//最小化消息
#define IDM_GAMESET					(WM_USER+116)						//游戏设置
/////////////////////////////////////////////////////////////////////////

#define MYSLEF_VIDEO_W    80
#define MYSLEF_VIDEO_H    60


//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;

public:
	int									m_iUpMj;						//麻将弹起标志
	int									m_tOutStartTime;				//出牌动画开始时间
	int									m_tOut;							//出牌动画时间
	bool								m_bActionOut;					//出牌动画
	bool								m_bPlay;						//打牌标志
	int									m_tDice;						//骰子动画时间
	CTimerControl						m_Timer;						//时钟控件

	//配置变量
protected:
	bool								m_bEnd;							//结束
	bool								m_bEndEmpty;					//流局
	bool								m_bAction;						//操作


	int									m_nChiMode;						//吃牌模式
	int 								m_nGangMode;					//杠牌模式
	int									m_nChiType;						//吃牌类型
	int									m_nGangType;					//杠牌类型

	BYTE								m_byGangMj[(HAND_MJ_MAX-2)/3-1];//杠牌可选牌

	BYTE								m_byOutMj;						//打出的牌
	bool								m_bDeasilOrder;					//出牌顺序
	bool								m_bLookOn;						//旁观

	int									m_iOldUpMj;						//旧标志
	WORD								m_wLastUser;					//上次出牌者
	WORD								m_wCurrentUser;					//当前玩家
	WORD								m_wBankerUser;					//庄家


	//数据变量
protected:
	BYTE								m_byTouchMjCount[GAME_PLAYER];	//摸牌区牌数
	BYTE								m_byHandMjCount[GAME_PLAYER];	//牌面牌数
	std::vector< BYTE >					m_vcOutMjs[GAME_PLAYER];		//出牌区数据
	MJ_HU								m_hu[GAME_PLAYER];				//胡牌区数据

	BYTE								m_byFengWei[GAME_PLAYER];		//风位数据
	WORD								m_wQuanWei;						//圈风
	WORD								m_wQuanWeiBanker;				//圈风庄位


	int									m_nActionOutFlag;				//出牌标志
	int									m_nDiceCount1;					///骰子数1
	int									m_nDiceCount2;					//骰子数2


	int									m_tOutFlag;						//出牌标志帧


	//位置变量
protected:
	POINT								m_ptTouchMjs[GAME_PLAYER];		//摸牌区起始位
	POINT								m_ptHuMjs[GAME_PLAYER];			//胡牌面起始位
	POINT								m_ptOutMjs[GAME_PLAYER];		//出牌区起始位
	POINT								m_ptFaceMj[GAME_PLAYER];		//牌面起始位
	POINT								m_ptGetMj[GAME_PLAYER];			//摸牌位
	POINT								m_ptFengWei[GAME_PLAYER];		//风位
	POINT								m_ptHua[GAME_PLAYER];			//补花

	POINT								m_ptQuanFengBk;					//圈风背景图
	POINT								m_ptQuanFeng;					//圈风
	POINT								m_ptActionOut;					//出牌动画起始位
	POINT								m_ptActionOutFlag;				//出牌标志起始位
	POINT								m_ptActionDice;					//骰子动画起始位
	POINT								m_ptActionDice2;				//骰子动画

	//界面变量
protected:
	CSize								m_csTouchMj[GAME_PLAYER];		//摸牌麻将大小
	CSize								m_csFaceMj[GAME_PLAYER];		//牌面麻将大小
	CSize								m_csOutMj[GAME_PLAYER];			//出牌麻将大小
	CSize								m_csHuMj[GAME_PLAYER];			//胡牌面麻将大小

	CSize								m_csFengWei;					//风位图大小
	CSize								m_csQuanFeng;					//圈风图大小
	CSize								m_csActionOut;					//出牌动画大小
	CSize								m_csActionOutFlag;				//出牌标志大小
	CSize								m_csActionDice1;				//骰子动画大小
	CSize								m_csActionDice2;				//骰子大小
	CSize								m_csActionOption;				//操作图大小

	//控件变量
public:
	CSkinButton							m_btStart;						//开始按扭
	CSkinButton							m_btHu;							//胡牌按扭
	CSkinButton							m_btTing;						//听牌按扭
	CSkinButton							m_btGang;						//杠牌按扭
	CSkinButton							m_btPeng;						//碰牌按扭
	CSkinButton							m_btChi;						//吃牌按扭
	CSkinButton							m_btGiveUp;						//放弃按扭

	CSkinButton							m_btShowMini;					//最小化按扭
	CSkinButton							m_btCancel;						//关闭按扭
	CSkinButton							m_btGameSet;					//设置按扭

	CSkinButton							btTest;							//调试用按扭
	CSkinButton							btTest2;						//调试2
	CSkinButton							btTest3;						//测试3
	//CSkinButton						btTest4;						//测试4

	//位图变量
protected:
	CSkinImage							m_ImageBack;					//背景资源
	CSkinImage							m_ImageFengWei;					//风位资源
	CSkinImage							m_ImageBack1;
	CSkinImage							m_ImageButtonBk;				//按扭背景图

	CSkinImage							m_ImageFaceMj[GAME_PLAYER];		//玩家手中牌
	CSkinImage							m_ImageBackMj;					//麻将背面图
	CSkinImage							m_ImageTouchMj[GAME_PLAYER];	//摸牌区麻将图
	CSkinImage							m_ImageOutMj[GAME_PLAYER];		//出牌区麻将图
	CSkinImage							m_ImageHuMj[GAME_PLAYER];		//胡牌区麻将图

	CSkinImage							m_ImageMaskMj;					//选牌模式-遮罩图

	CSkinImage							m_ImageQuanFeng;				//圈风图
	CSkinImage							m_ImageQuanFengBk;				//圈风图背景

	CSkinImage							m_ActionOut;					//出牌动画
	CSkinImage							m_ActionOutFlag;				//出牌标志动画
	CSkinImage							m_ActionDice1;					//骰子动画
	CSkinImage							m_ActionDice2;					//骰子动画定点
	CSkinImage							m_ActionOption;					//操作动画]

	CSkinImage							m_ImageEndEmpty;				//流局

	//麻将信息
protected:
	BYTE								m_byHandMjs[4][14];				//手上麻将


	CScoreView							m_scoreView;					//积分视图

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//绘图函数
	//流局
	void SetImageEmpty( CDC *pDC );
	//结束显牌
	void SetImageEnd( CDC *pDC, BYTE byPlayer );
	//减少牌面
	void SubTouchMJ();
	//摸牌区
	void SetImageTMj( CDC *pDC, BYTE byPlayer );
	//出牌区
	void SetImageOMj( CDC *pDC, BYTE byPlayer );
	//其它玩家牌面
	void SetImageFMj( CDC *pDC, BYTE byPlayer );
	//自已牌面
	void SetImageSelfMj( CDC *pDC, BYTE byData[], BYTE byMjCount );
	//胡牌区
	void SetImageHMj( CDC *pDC, BYTE byPlayer );
	//风位
	void SetImageFengWei( CDC *pDC, BYTE byPlayer );
	//圈风区
	void SetImageQuanFeng( CDC *pDC );
	//选择吃牌
	void SetImageSelectChi( CDC *pDC, BYTE byData[], BYTE byMjCount, int nChiMode, BYTE byMjOut );
	//选择杠牌
	void SetImageSeletGang( CDC *pDC, BYTE byData[], BYTE byMjCount, int nGangMode, BYTE byGangMj[]);

	//骰子动画
	void  ActionDice1( CDC *pDC,int nFrame);
	//骰子动画2
	void ActionDice2( CDC *pDC,int pos1, int pos2 );
	//出牌动画
	void ActionOut( CDC *pDC, int x, int y, int nFrame );
	//操作动画
	void ActionOption( CDC *pDC, BYTE byPlayer, int OptionType );
	//标志出牌动画
	void ActionOutFlag( CDC *pDC, int x, int y, int nFrame );
	//获取麻将
	void GetHandMj( BYTE byMj[], BYTE byMjCount );
	//加入出牌区
	void AddOutMj( WORD wChairID,BYTE data );
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
	void SetUserOrder( bool bDeasilOrder );
	//获取顺序
	bool IsDeasilOrder()	{ return m_bDeasilOrder; }

	//辅助函数
public:
	//禁用按扭
	void HideButton();
	//清空视图
	void CleanUpView();
	//设置描边字体
	void SetArtText( CDC *pDC, int x, int y, COLORREF nColor, COLORREF nBkColor,LPCTSTR lpszString  );

	//消息映射
protected:
	afx_msg void OnTest();
	afx_msg void OnTest2();
	afx_msg void OnTest3();
	//关闭按扭
	afx_msg void OnCancel();
	//最小化窗口
	afx_msg void OnShowMini();
	//游戏设置
	afx_msg void OnGameSet();
	//开始按扭
	afx_msg void OnStart();
	//胡牌按扭
	afx_msg void OnHuMj();
	//听牌按扭
	afx_msg void OnTingMj();
	//杠牌按扭
	afx_msg void OnGangMj();
	//碰牌按扭
	afx_msg void OnPengMj();
	//吃牌按扭
	afx_msg void OnChiMj();
	//放弃按扭
	afx_msg void OnGiveUp();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//鼠标右击
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);	
	//鼠标左击
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//移动鼠标
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
