#ifndef GAME_FRAME_VIEW_HEAD_FILE
#define GAME_FRAME_VIEW_HEAD_FILE

#pragma once

#include "GameFrame.h"
#include "ThreadDraw.h"

//////////////////////////////////////////////////////////////////////////

//绘画模式
enum enImageMode 
{
	enMode_Spread,					//平铺模式
	enMode_Centent,					//居中模式
	enMode_ElongGate,				//拉伸模式
};

//类说明
class CClientKernelSink;

//////////////////////////////////////////////////////////////////////////

//游戏视图框架
class GAME_FRAME_CLASS CGameFrameView : public CWnd, public IGameFrameView
{
	//友元定义
	friend class CGameFrameDlg;
	friend class CClientKernelSink;

	//位置变量
protected:
	POINT							m_ptName[MAX_CHAIR];				//名字位置
	POINT							m_ptFace[MAX_CHAIR];				//头像位置
	POINT							m_ptTimer[MAX_CHAIR];				//时间位置
	POINT							m_ptReady[MAX_CHAIR];				//准备位置
public:
//Add by 20080126
	CPoint							m_PtVideo[MAX_CHAIR];				//video位置
	UINT							m_uVideoInt[MAX_CHAIR];				//是否显示VIDEO 1=显示
//End add
	//用户变量
private:
	WORD							m_wTimer[MAX_CHAIR];				//用户时间
	tagUserData						* m_pUserItem[MAX_CHAIR];			//用户信息

	//组件指针
private:
	void							* m_pReserve;						//保留指针
	IUserFaceRes					* m_pIUserFaceRes;					//头像接口

	//画图变量
private:
	bool							m_bReDraw;							//是否重画
	CSize							m_ImageSize;						//缓冲大小
	CImage							m_ImageBuffer;						//缓冲位图
	const bool						m_bDoubleBuf;						//缓冲绘图
	const UINT						m_uColorCount;						//缓冲色深
	CThreadDraw						* const m_pThreadDraw;				//绘图信息

	//位图资源
private:
	CSkinImage						m_PicVideo;						//VIDEO标志
	CSkinImage						m_PicMyselfVideo;               //自己
	CSkinImage						m_PicOpBak;						//opback

	CSkinImage						m_ImageReady;						//准备标志
	CSkinImage						m_ImageTimer;						//时间标志
	CSkinImage						m_ImageViewT;						//框架图片
	CSkinImage						m_ImageViewB;						//框架图片
	CSkinImage						m_ImageViewL;						//框架图片
	CSkinImage						m_ImageViewR;						//框架图片
	CSkinImage						m_ImageViewBL;						//框架图片
	CSkinImage						m_ImageViewTL;						//框架图片
	CSkinImage						m_ImageViewBR;						//框架图片
	CSkinImage						m_ImageViewTR;						//框架图片

	//只读变量
public:
	static const int				m_nXFace;							//头像高度
	static const int				m_nYFace;							//头像宽度
	static const int				m_nXTimer;							//定时器宽
	static const int				m_nYTimer;							//定时器高
	static const int				m_nXBorder;							//框架宽度
	static const int				m_nYBorder;							//框架高度

	//函数定义
public:
	//构造函数
	CGameFrameView(bool bDoubleBuf=true ,UINT uColorCount=16, CThreadDraw * pThreadDraw=NULL);
	//析构函数
	virtual ~CGameFrameView();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CGameFrameView))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//继承函数
private:
	//重置界面
	virtual void ResetGameView()=NULL;
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight)=NULL;
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight)=NULL;

	//功能函数
public:
	//获取时间
	WORD GetUserTimer(WORD wChairID);
	//获取玩家
	const tagUserData * GetUserInfo(WORD wChairID);
	//更新界面
	void UpdateGameView(const CRect * pRect);
	//获取界面
	bool GetGameViewImage(CImage & ImageBuffer, int nXPos, int nYPos, int nWidth, int nHeight);

	//画图函数
public:
	//画self video标志
	void DrawMyselfVideoPic(CDC * pDC, int iXPos, int iYPos);

	//画video标志
	void DrawOtherVideoPic(CDC * pDC, int iXPos, int iYPos);

	//画op back标志
	void DrawOtherOpBackPic(CDC * pDC, int iXPos, int iYPos);

	//绘画背景
	void DrawViewImage(CDC * pDC, CSkinImage & SkinImage, enImageMode ImageMode);
	//绘画头像
	void DrawUserFace(CDC * pDC, WORD wFaceID, int nXPos, int nYPos, bool bOffLine);
	//绘画头像
	void DrawUserSmallFace(CDC * pDC, WORD wFaceID, int nXPos, int nYPos, bool bOffLine);
	//绘画准备
	void DrawUserReady(CDC * pDC, int nXPos, int nYPos);
	//绘画时间
	void DrawUserTimer(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea=99);
	//透明绘画
	void AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, CDC * pScrDC, int nXScr, int nYScr, COLORREF crTransColor);
	//透明绘画
	void AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, HBITMAP hBitBmp, int nXScr, int nYScr, COLORREF crTransColor);

	//内部函数
private:
	//重置界面
	void ResetData();
	//设置接口
	bool SetUserFaceRes(IUnknownEx * pIUnknownEx);
	//设置时间
	bool SetUserTimer(WORD wChairID, WORD wTimer);
	//设置用户
	bool SetUserInfo(WORD wChairID, tagUserData * pUserData);
	//绘画边框
	bool DrawGameViewFrame(CDC * pDC, int nWidth, int nHeight);

	//消息映射
protected:
	//绘画函数
	afx_msg void OnPaint();
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//销毁消息
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif