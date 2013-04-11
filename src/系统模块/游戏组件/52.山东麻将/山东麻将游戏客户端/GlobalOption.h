#ifndef SYSTEM_NEW_OPTION_HEAD_FILE
#define SYSTEM_NEW_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//全局配置类
class CGlobalOption
{
	//消息设置
public:
	bool							m_bShowInOutMessage;				//进出信息

	//颜色设置
public:
	COLORREF						m_crChatTX;							//聊天字体
	COLORREF						m_crMsgName;						//名字消息
	COLORREF						m_crMsgNormal;						//普通消息
	COLORREF						m_crMsgSysHead;						//系统消息
	COLORREF						m_crMsgSysString;					//系统消息

	//函数定义
public:
	//构造函数
	CGlobalOption();
	//析构函数
	virtual ~CGlobalOption();

	//功能函数
public:
	//加载设置
	void LoadOptionParameter();
	//保存设置
	void SaveOptionParameter();
};

//////////////////////////////////////////////////////////////////////////

//全局信息 
extern	CGlobalOption					g_GlobalOption;					//系统配置

//////////////////////////////////////////////////////////////////////////

#endif