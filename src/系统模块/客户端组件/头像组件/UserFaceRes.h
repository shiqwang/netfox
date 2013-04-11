#ifndef USER_FACE_RES_HEAD_FILE
#define USER_FACE_RES_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "AtlImage.h"
#include "UserFace.h"

//用户头像资源类
class CUserFaceRes : public IUserFaceRes
{
	//变量定义
protected:
	WORD								m_wCount;						//头像数目
	CSize								m_NormalSize;					//头像大小
	CSize								m_SmallSize;					//头像大小

	//资源变量
protected:
	CImage								m_SmallFace;					//小型头像
	CImage								m_NormalFace;					//普通头像

	//函数定义
public:
	//构造函数
	CUserFaceRes(void);
	//析构函数
	virtual ~CUserFaceRes(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CUserFaceRes))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//头像数目
	virtual WORD __cdecl GetFaceCount() { return m_wCount; }
	//获取大小
	virtual const CSize & __cdecl GetSmallFaceSize() { return m_SmallSize; };
	//获取大小
	virtual const CSize & __cdecl GetNormalFaceSize() { return m_NormalSize; }
	//绘画头像
	virtual void __cdecl DrawSmallFace(CDC * pDC, int nXPos, int nYPos, WORD wFaceID);
	//绘画头像
	virtual void __cdecl DrawNormalFace(CDC * pDC, int nXPos, int nYPos, WORD wFaceID);

	//内部函数
protected:
	//透明绘画
	bool AlphaDrawImage(CDC * pDestDC, int xDest, int yDest, int cxDest, int cyDest, CDC * pScrDC, int xSrc, int ySrc);
};

//////////////////////////////////////////////////////////////////////////

#endif