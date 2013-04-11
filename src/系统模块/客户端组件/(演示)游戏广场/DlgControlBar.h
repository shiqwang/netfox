#ifndef DLG_CONTROL_BAR_HEAD_FILE
#define DLG_CONTROL_BAR_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "RoomViewItem.h"
#include "PlazaViewItem.h"

//宏定义
#define MAX_SERVER						2								//最大房间
#define CONTROL_BAR_HEIGHT				32								//控制条高

//////////////////////////////////////////////////////////////////////////

//控制条对话框
class CDlgControlBar : public CDialog
{
	//状态变量
public:
	CRect								m_rcViewItem;					//窗口位置
	IViewItem							* m_pActiveViewItem;			//当前视图
	CPlazaViewItem						* m_pPlazaViewItem;				//广场视图
	CRoomViewItem						* m_pRoomViewItemCreate;		//创建房间
	CRoomViewItem						* m_pRoomViewItem[MAX_SERVER];	//房间接口
	
	//资源变量
protected:
	CSkinImage							m_ImageLeft;					//图片资源
	CSkinImage							m_ImageMin;						//图片资源
	CSkinImage							m_ImageRight;					//图片资源

	//控件变量
public:
	CSkinButton							m_btGamePlaza;					//游戏广场
	CSkinButton							m_btViewItem1;					//房间按钮
	CSkinButton							m_btViewItem2;					//房间按钮
	CSkinButton							m_btSwitchUser;					//切换帐号
	CSkinButton							m_btGlobalOption;				//系统配置
	CSkinButton							m_btCompanion;					//关系管理
	CSkinButton							m_btQuitGame;					//退出游戏

	//对话框数据
	enum { IDD = IDD_CONTROL_BAR };

	//函数定义
public:
	//构造函数
	CDlgControlBar();
	//析构函数
	virtual ~CDlgControlBar();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//功能函数
public:
	//设置位置
	void SetViewItemRect(int nXPos, int nYPos, int nWidth, int nHeight);
	//获取广场
	CPlazaViewItem * GetPlazaViewItem() { return m_pPlazaViewItem; }
	//设置广场
	void SetPlazaViewItem(CPlazaViewItem * pPlazaViewItem) { m_pPlazaViewItem=pPlazaViewItem; }

	//功能函数
public:
	//显示网页
	void WebBrowse(LPCTSTR lpszUrl, bool bForceBrowse);
	//显示消息
    int ShowMessageBox(LPCTSTR pszMessage, UINT nType);

	//房间函数
public:
	//激活广场
	void ActivePlazaViewItem();
	//激活房间
	void ActiveRoomViewItem(CRoomViewItem * pRoomViewItem);
	//查找房间
	CRoomViewItem * SearchRoomViewItem(WORD wKindID, WORD wServerID);
	//进入房间
	CRoomViewItem * CreateRoomViewItem(CListServer * pListServer);
	//关闭房间
	void CloseRoomViewItem(IRoomViewItem * pIRoomViewItem);
	//关闭房间
	void CloseAllRoomViewItem();
	//关闭当前
	void CloseCurrentViewItem();

	//内部函数
private:
	//更新控制
	void UpdateControlButton();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//游戏广场
	afx_msg void OnBnClickedPlaza();
	//游戏房间
	afx_msg void OnBnClickedViewItem1();
	//游戏房间
	afx_msg void OnBnClickedViewItem2();
	//游戏房间
	afx_msg void OnBnClickedViewItem3();
	//游戏房间
	afx_msg void OnBnClickedViewItem4();
	//切换按钮
	afx_msg void OnBnClickedSwitchAccounts();
	//关系按钮
	afx_msg void OnBnClickedCompanion();
	//配置按钮
	afx_msg void OnBnClickedGlobalOption();
	//退出按钮
	afx_msg void OnBnClickedQuitGame();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//关闭房间
	LRESULT OnCloseRoomViewItem(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//控制栏指针
extern CDlgControlBar * g_pControlBar;

//////////////////////////////////////////////////////////////////////////

#endif