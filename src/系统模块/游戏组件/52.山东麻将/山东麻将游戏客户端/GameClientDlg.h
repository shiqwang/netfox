#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "GameLogic.h"
#include "..\消息定义\CMD_Mj.h"
#include "TranscendLog.h"
#include "GameParrentDlg.h"


//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//日志
	TranscendLog log;
	//游戏变量
protected:
	BYTE							m_byOutMj;							//打出的牌
	BYTE							m_byHandMjCount;					//手上麻将数
	BYTE							m_byHandMjs[14];					//手上麻将
	BYTE							m_byMjCount[4];						//麻将数

	int								m_nChiMode;							//吃牌模式
	int								m_nGangMode;						//杠牌模式
//	MJ_HU							m_hu[4];
	
	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
public:
	CGameClientView					m_GameClientView;					//游戏视图

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序
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
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse,UINT nTimerID);
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize);
protected:
	void PlayPaiSound(BYTE nPai,BYTE sex);

	//消息处理
protected:
	//发送麻将
	bool OnSubSendMj( const void * pBuffer, WORD wDataSize );
	//游戏开始
	bool OnSubGameStart( const void * pBuffer, WORD wDataSize );
	//显示按扭
	bool OnSubGameButton( const void * pBuffer, WORD wDataSize );
	//用户出牌
	bool OnSubOutMj( const void * pBuffer, WORD wDataSize );
	//自动摸牌
	bool OnSubTouchMj( const void * pBuffer, WORD wDataSize );
	//用户碰牌
	bool OnSubPengMj( const void *pBuffer, WORD wDataSize );
	//用户吃牌
	bool OnSubChiMj( const void *pBuffer, WORD wDataSize );
	//用户杠牌
	bool OnSubGangMj( const void *pBuffer, WORD wDataSize );
	//游戏结束
	bool OnSubGameEnd( const void *pBuffer, WORD wDataSize );
	//杠牌选择模式
	bool OnSubSelectGang( const void *pBuffer, WORD wDataSize );
	//流局
	bool OnSubEndEmpty( const void *pBuffer, WORD wDataSize);
	
	//辅助函数
protected:
	//用户位置软换
	WORD TransViewID( WORD wChairID );
	//设置庄位
	void SetBanker( WORD wBanker );

	//消息映射
public:
	//开始消息
	LRESULT OnStart( WPARAM wParam, LPARAM lParam );
	//出牌消息
	LRESULT OnOutMj( WPARAM wParam, LPARAM lParam );
	//胡牌消息
	LRESULT OnHuMj( WPARAM wParam, LPARAM lParam );
	//听牌消息
	LRESULT OnTingMj( WPARAM wParam, LPARAM lParam );
	//杠牌消息
	LRESULT OnGangMj( WPARAM wParam, LPARAM lParam );
	//碰牌消息
	LRESULT OnPengMj( WPARAM wParam, LPARAM lParam );
	//吃牌消息
	LRESULT OnChiMj( WPARAM wParam, LPARAM lParam );
	//放弃消息
	LRESULT OnGiveUp( WPARAM wParam, LPARAM lParam );

	//关闭消息
	LRESULT OnGameSet( WPARAM wParam, LPARAM lParam );
	//最小化
	LRESULT OnShowMini( WPARAM wParam, LPARAM lParam );

	//选择吃牌
	LRESULT OnSelectChi( WPARAM wParam, LPARAM lParam );
	//选择杠牌
	LRESULT OnSelectGang( WPARAM wParam, LPARAM lParam );
	//右键消息
	LRESULT OnRightHitMj( WPARAM wParam, LPARAM lParam );
	//左键消息
	LRESULT OnLeftHitMj( WPARAM wParam, LPARAM lParam );

	//超时消
	LRESULT OnCmdTimerOut( WPARAM wParam, LPARAM lParam );
	//
	LRESULT OnPlaySound(WPARAM wParam, LPARAM lParam);

	//定时器消息
	//afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
