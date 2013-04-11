#ifndef USER_FACE_HEAD_FILE
#define USER_FACE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "..\..\模板库\Template.h"
#include "..\..\组件接口\IUnknownEx.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//宏定义
#define LIT_FACE_WIDTH						16							//头像宽度
#define LIT_FACE_HEIGHT						16							//头像高度
#define NOR_FACE_WIDTH						32							//头像宽度
#define NOR_FACE_HEIGHT						32							//头像高度

//模块定义
#ifdef _DEBUG
	#define USER_FACE_DLL_NAME	TEXT("UserFaceD.dll")					//组件 DLL 名字
#else
	#define USER_FACE_DLL_NAME	TEXT("UserFace.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#define VER_IUserFaceRes INTERFACE_VERSION(1,1)
static const GUID IID_IUserFaceRes={0xd9c22bd3,0x986b,0x4513,0x83,0x5d,0x52,0xb4,0x13,0x20,0x76,0x34};

//头像组件接口
interface IUserFaceRes : public IUnknownEx
{
	//获取数目
	virtual WORD __cdecl GetFaceCount()=NULL;
	//获取大小
	virtual const CSize & __cdecl GetSmallFaceSize()=NULL;
	//获取大小
	virtual const CSize & __cdecl GetNormalFaceSize()=NULL;
	//绘画头像
	virtual void __cdecl DrawSmallFace(CDC * pDC, int nXPos, int nYPos, WORD wFaceID)=NULL;
	//绘画头像
	virtual void __cdecl DrawNormalFace(CDC * pDC, int nXPos, int nYPos, WORD wFaceID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IFaceControl INTERFACE_VERSION(1,1)
static const GUID IID_IFaceControl={0xd2c483c,0xbdfa,0x46a6,0x8e,0x60,0xf8,0xc8,0x77,0xed,0x59,0xc3};

//头像控件接口
interface IFaceControl : public IUnknownEx
{
	//信息接口
public:
	//头像宽度
	virtual int __cdecl GetFaceWidth()=NULL;
	//头像高度
	virtual int __cdecl GetFaceHeight()=NULL;
	//头像数目
	virtual WORD __cdecl GetFaceItemCount()=NULL;

	//绘画接口
public:
	//绘画头像
	virtual void __cdecl DrawFaceItem(CDC * pDC, int nXPos, int nYPos, WORD wFaceID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//头像组件辅助类
class CUserFaceResHelper : public CTempldateHelper<IUserFaceRes>
{
	//函数定义
public:
	//构造函数
	CUserFaceResHelper(void) : CTempldateHelper<IUserFaceRes>(IID_IUserFaceRes,
		VER_IUserFaceRes,USER_FACE_DLL_NAME,TEXT("CreateUserFaceRes"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

//头像控件辅助类
class CFaceControlHelper : public CTempldateHelper<IFaceControl>
{
	//函数定义
public:
	//构造函数
	CFaceControlHelper(void) : CTempldateHelper<IFaceControl>(IID_IFaceControl,
		VER_IFaceControl,USER_FACE_DLL_NAME,TEXT("CreateFaceControl"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

#endif