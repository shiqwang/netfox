#ifndef GAME_FRAME_HEAD_FILE
#define GAME_FRAME_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "DlgControlBar.h"
#include "PlazaViewItem.h"

/////////////////////////////////////////////////////////////////////////////

//主框架类
class CGameFrame : public CFrameWnd
{
	//控件变量
public:
	CSkinButton							m_btMin;						//最小按钮
	CSkinButton							m_btClose;						//关闭按钮
	CToolTipCtrl						m_ToolTipCtrl;					//提示控件
	CPlazaViewItem						m_DlgGamePlaza;					//游戏广场
	CDlgControlBar						m_DlgControlBar;				//控制条类
	CHtmlBrower							* m_pAdBrower;					//广告窗口

	//导航按钮
public:
	CSkinButton							m_btButton1;					//功能按钮
	CSkinButton							m_btButton2;					//功能按钮
	CSkinButton							m_btButton3;					//功能按钮
	CSkinButton							m_btButton4;					//功能按钮
	CSkinButton							m_btButton5;					//功能按钮
	CSkinButton							m_btButton6;					//功能按钮
	CSkinButton							m_btButton7;					//功能按钮

	//资源变量
protected:
	CSkinImage							m_ImageL;						//资源图片
	CSkinImage							m_ImageR;						//资源图片
	CSkinImage							m_ImageTL;						//资源图片
	CSkinImage							m_ImageTM;						//资源图片
	CSkinImage							m_ImageTR;						//资源图片

	//函数定义	
public:
	//构造函数
	CGameFrame();
	//析构函数
	virtual ~CGameFrame();

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//消息函数
protected:
	//绘画消息
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//大小消息
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR * lpMMI);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//最小按钮
	afx_msg void OnBnClickedMin();
	//关闭按钮
	afx_msg void OnBnClickedClose();
	//改变消息
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//关闭消息
	afx_msg void OnClose();

	//导航按钮
protected:
	//功能按钮
	afx_msg void OnBnClickedButton1();
	//功能按钮
	afx_msg void OnBnClickedButton2();
	//功能按钮
	afx_msg void OnBnClickedButton3();
	//功能按钮
	afx_msg void OnBnClickedButton4();
	//功能按钮
	afx_msg void OnBnClickedButton5();
	//功能按钮
	afx_msg void OnBnClickedButton6();
	//功能按钮
	afx_msg void OnBnClickedButton7();

	//自定义消息
protected:
	//安装完成
	LRESULT OnMessageSetupFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
