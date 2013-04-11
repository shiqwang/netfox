#ifndef GAME_FRAME_CONTROL_HEAD_FILE
#define GAME_FRAME_CONTROL_HEAD_FILE

#pragma once

#include "GameFrame.h"
#include "UserListView.h"
#include "GameFrameDlg.h"

//类说明
class CClientKernelSink;

//////////////////////////////////////////////////////////////////////////

//框架控制视图
class GAME_FRAME_CLASS CGameFrameControl : public CDialog
{
public:
	//发送语音
	void SendChatMsg(CString chatMsg);

	//变量定义
protected:
	bool								m_bRectify;						//调整标志
	bool								m_bInitDialog;					//创建标志

	//组件变量
protected:
	CHtmlBrower							* m_pAdBrower;					//广告窗口
	CGameFrameDlg						* m_pGameFrameDlg;				//对话框
	IClientKernel						* m_pIClientKernel;				//内核接口

	//左视图区
protected:
	CSkinImage							m_ImageLeftViewLM;			//资源图片
	CSkinImage							m_ImageLeftViewMM;			//资源图片
	CSkinImage							m_ImageLeftViewRM;			//资源图片
	CSkinImage							m_ImageLeftViewLTM;			//资源图片
	CSkinImage							m_ImageLeftViewRTM;			//资源图片
	CSkinImage							m_ImageLeftViewLT;			//资源图片
	CSkinImage							m_ImageLeftViewMT;			//资源图片
	CSkinImage							m_ImageLeftViewRT;			//资源图片
	CSkinImage							m_ImageLeftViewLB;			//资源图片
	CSkinImage							m_ImageLeftViewMB;			//资源图片
	CSkinImage							m_ImageLeftViewRB;			//资源图片
	CSkinImage							m_ImageLeftViewLBM;			//资源图片
	CSkinImage							m_ImageLeftViewRBM;			//资源图片

	//聊天控件
public:
	CEdit								m_ChatInput;					//聊天输入
	CComboBox							m_ChatObject;					//聊天对象
	CSkinButton							m_btPhrase;						//短语按钮
	CSkinButton							m_btColorSet;					//颜色设置
	CSkinButton							m_btCleanScreen;				//清屏按钮
	CSkinButton							m_btSendChat;					//发送按钮
	CSkinButton							m_btExpression;					//表情按钮
//	CSkinButton							m_btGameRule;					//游戏规则
	CExpression							m_Expression;					//表情窗口
	CSkinRichEdit						m_ChatMessage;					//聊天组件

	//其他变量
public:
//	CSkinButton							m_btQuitGame;					//退出游戏
//	CSkinButton							m_btGameOption;					//游戏设置
	CToolTipCtrl						m_ToolTipCtrl;					//提示控件
	CUserListView						m_UserListView;					//用户列表

	//函数定义
public:
	//构造函数
	CGameFrameControl(CGameFrameDlg * pGameFrameDlg); 
	//析构函数
	virtual ~CGameFrameControl();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK() { return; };
	//取消函数
	virtual void OnCancel() { return; };
	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//配置函数
public:
	//设置内核
	void SetClientKernel(IClientKernel * pIClientKernel);

	//功能函数
public:
	//重置控制
	void ResetGameFrameControl();
	//加入用户
	bool InsertUserItem(tagUserData * pUserData);
	//更新用户
	bool UpdateUserItem(tagUserData * pUserData);
	//删除用户
	bool DeleteUserItem(tagUserData * pUserData);

	//内部函数
protected:
	//左视图区
	void DrawLeftViewFrame(CDC * pDC);
	//调整控件
	void RectifyControl(int nWidth, int nHeight);

	//消息函数
protected:
	//绘画消息
	afx_msg void OnPaint();

	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//发送按钮
	afx_msg void OnBnClickedSendChat();
	//表情按钮
	afx_msg void OnBnClickedExpression();
	//颜色按钮
	afx_msg void OnBnClickedColorSet();
	//清屏按钮
	afx_msg void OnBnClickedCleanScreen();
	//表情消息
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif