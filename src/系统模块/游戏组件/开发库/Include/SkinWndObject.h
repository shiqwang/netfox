#ifndef SKIN_WND_OBJECT_HEAD_FILE
#define SKIN_WND_OBJECT_HEAD_FILE

#pragma once

#include "SkinImage.h"
#include "SkinControls.h"
#include "SkinAttribute.h"

//////////////////////////////////////////////////////////////////////////

//属性名字
#define SKIN_WND_CRBACKFRAME		TEXT("窗体框架颜色")			// COLORREF 类型 （读写）
#define SKIN_WND_CRBACKGROUND		TEXT("窗体背景颜色")			// COLORREF 类型 （读写）
#define SKIN_WND_CRINSIDEBORDER		TEXT("窗体内框颜色")			// COLORREF 类型 （读写）
#define SKIN_WND_CRCAPTIONTXCOLOR	TEXT("窗体标题颜色")			// COLORREF 类型 （读写）
#define SKIN_WND_CRCONTROLTXCOLOR	TEXT("窗体控件字体颜色")		// COLORREF 类型 （读写）
#define SKIN_WND_STRBUTTONMINPATH	TEXT("窗体最小化位图")			// CString  类型 （读写）
#define SKIN_WND_STRBUTTONMAXPATH	TEXT("窗体最大化位图")			// CString  类型 （读写）
#define SKIN_WND_STRBUTTONRESPATH	TEXT("窗体还原位图")			// CString  类型 （读写）
#define SKIN_WND_STRBUTTONCLOSEPATH	TEXT("窗体关闭位图")			// CString  类型 （读写）
#define SKIN_WND_STRTITLELEFTPATH	TEXT("窗体左标题")				// CString  类型 （读写）
#define SKIN_WND_STRTITLEMIDPATH	TEXT("窗体中标题")				// CString  类型 （读写）
#define SKIN_WND_STRTITLERIGHTPATH	TEXT("窗体右标题")				// CString  类型 （读写）

//////////////////////////////////////////////////////////////////////////

//窗口类属性
class CSkinWndAttribute : public CSkinAttribute
{
	friend class CSkinWndObject;

	//设置部分
public:
	COLORREF						m_crBackFrame;						//框架背景
	COLORREF						m_crBackGround;						//背景颜色
	
	COLORREF						m_crBackGround1;						//背景颜色
	COLORREF						m_crBackGround2;						//背景颜色
	
	
	COLORREF						m_crInsideBorder;					//内框颜色
	COLORREF						m_crCaptionTXColor;					//标题颜色
	COLORREF						m_crControlTXColor;					//控制颜色
	CString							m_strButtonMinPath;					//位图路径
	CString							m_strButtonMaxPath;					//位图路径
	CString							m_strButtonResPath;					//位图路径
	CString							m_strButtonClosePath;				//位图路径
	CString							m_strTitleLeftPath;					//标题路径
	CString							m_strTitleMidPath;					//标题路径
	CString							m_strTitleRightPath;				//标题路径

	//属性变量
public:
	int								m_nButtonWidth;						//按钮宽度
	int								m_nButtonHeigth;					//按钮高度
	int								m_nCaptionHeigth;					//标题高度

	//GDI 对象
public:
	CBrush							m_brBackFrame;						//背景刷
	CBrush							m_brBackGround;						//背景刷
	CSkinImage						m_ButtonMin;						//最小化按钮
	CSkinImage						m_ButtonMax;						//最大化按钮
	CSkinImage						m_ButtonRes;						//还原按钮
	CSkinImage						m_ButtonClose;						//关闭按钮	
	CSkinImage						m_TitleLeft;						//标题图片
	CSkinImage						m_TitleMid;							//标题图片
	CSkinImage						m_TitleRight;						//标题图片

	//函数定义
protected:
	//构造函数
	CSkinWndAttribute();
	//析构函数
	virtual ~CSkinWndAttribute();

	//接口函数
public:
	//加载配置
	virtual bool __cdecl LoadSkinOption();
	//保存配置
	virtual bool __cdecl SaveSkinOption();
};

//////////////////////////////////////////////////////////////////////////

//窗口类控制
class SKIN_CONTROL_CLASS CSkinWndObject
{
	//状态变量
protected:
	bool							m_bActive;							//是否激活
	bool							m_bMaxSize;							//最大标志
	int								m_nXExcursionPos;					//X 偏移
	int								m_nYExcursionPos;					//Y 偏移
	BYTE							m_cbNowHotButton;					//现在焦点
	BYTE							m_cbNowDownButton;					//按下按钮
	BYTE							m_cbButtonState[3];					//按钮状态
	CRect							m_rcButton[3];						//按钮位置

	//内部变量
private:
	CWnd							* m_pWndHook;						//挂接窗口
	CRect							m_crNormalSize;						//正常位置

	//静态变量
public:
	static CSkinWndAttribute		m_SkinAttribute;					//窗口属性

	//函数定义
public:
	//构造函数
	CSkinWndObject(CWnd * pWndSkin);
	//析构函数
	virtual ~CSkinWndObject(void);

	//功能函数
public:
	//绘画界面
	void DrawSkinView(CDC * pDC);
	//最大窗口
	void MaxSizeWindow();
	//还原窗口
	void RestoreWindow();
	//获取偏移
	int GetXExcursionPos() { return m_nXExcursionPos; }
	//获取偏移
	int GetYExcursionPos() { return m_nYExcursionPos; }
	//获取属性接口
	static CSkinWndAttribute * GetSkinAttribute() { return &m_SkinAttribute; };

	//内部函数
protected:
	//初始化
	void InitSkinObject();
	//调整按钮
	void AdjustTitleButton(int nCXWindow, int nCYWindow);
	//绘画标题
	bool DrawCaption(CDC * pDC, int cx, int cy);

	//事件处理
protected:
	//设置改变事件
	void OnSettingChange();
	//鼠标移动事件
	bool OnMouseMoveEvent(CPoint ClientPoint);
	//鼠标左键事件
	bool OnLMouseDownEvent(CPoint ClientPoint);
	//鼠标左键事件
	bool OnLMouseUpEvent(CPoint ClientPoint);
	//鼠标左键事件
	bool OnLMouseDoubleEvent(CPoint ClientPoint);
	//鼠标右键事件
	bool OnRMouseDownEvent(CPoint ClientPoint);
	//鼠标右键事件
	bool OnRMouseUpEvent(CPoint ClientPoint);
	//鼠标右键事件
	bool OnRMouseDoubleEvent(CPoint ClientPoint);
};

//////////////////////////////////////////////////////////////////////////

#endif