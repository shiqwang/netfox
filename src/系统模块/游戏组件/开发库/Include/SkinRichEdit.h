#ifndef SKIN_RICH_EDIT_HEAD_FILE
#define SKIN_RICH_EDIT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SkinControls.h"
#include "SkinAttribute.h"

//////////////////////////////////////////////////////////////////////////

//图像 OLE 类
class CImageDataObject : IDataObject
{
	//变量定义
private:
	ULONG								m_ulRef;						//引用计数
	FORMATETC 							m_Format;						//结构信息
	STGMEDIUM 							m_Stgmed;						//结构信息

	//函数定义 
public:
	//构造函数
	CImageDataObject();
	//析构函数
	~CImageDataObject();

	//功能函数
public:
	//设置图片
	void SetBitmap(HBITMAP hBitmap);
	//获取对象
	IOleObject * GetOleObject(IOleClientSite * pOleClientSite, IStorage * pStorage);

	//接口函数
public:
	//查询接口
	STDMETHOD(QueryInterface)(REFIID iid, void * * ppvObject)
	{
		if ((iid==IID_IUnknown)||(iid==IID_IDataObject))
		{
			*ppvObject=this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	//增加引用
	STDMETHOD_(ULONG, AddRef)(void)
	{
		m_ulRef++;
		return m_ulRef;
	}
	//释放对象
	STDMETHOD_(ULONG, Release)(void)
	{
		if (--m_ulRef==0) delete this;
		return m_ulRef;
	}
	//设置数据
	STDMETHOD(SetData)(FORMATETC * pformatetc, STGMEDIUM * pmedium, BOOL fRelease) 
	{
		m_Format=*pformatetc;
		m_Stgmed=*pmedium;
		return S_OK;
	}
	//获取数据
	STDMETHOD(GetData)(FORMATETC * pformatetcIn, STGMEDIUM * pmedium) 
	{
		HANDLE hDst =OleDuplicateData(m_Stgmed.hBitmap,CF_BITMAP,NULL);
		if (hDst==NULL) return E_HANDLE;
		pmedium->tymed=TYMED_GDI;
		pmedium->hBitmap=(HBITMAP)hDst;
		pmedium->pUnkForRelease=NULL;
		return S_OK;
	}
	STDMETHOD(EnumFormatEtc)(DWORD dwDirection, IEnumFORMATETC * *  ppenumFormatEtc ) {	return E_NOTIMPL; }
	STDMETHOD(DAdvise)(FORMATETC * pformatetc, DWORD advf, IAdviseSink * pAdvSink,DWORD * pdwConnection) { return E_NOTIMPL; }
	STDMETHOD(DUnadvise)(DWORD dwConnection) { return E_NOTIMPL; }
	STDMETHOD(EnumDAdvise)(IEnumSTATDATA * * ppenumAdvise) { return E_NOTIMPL; }
	STDMETHOD(GetDataHere)(FORMATETC * pformatetc, STGMEDIUM *  pmedium ) { return E_NOTIMPL;	}
	STDMETHOD(QueryGetData)(FORMATETC * pformatetc) { return E_NOTIMPL; }
	STDMETHOD(GetCanonicalFormatEtc)(FORMATETC * pformatectIn, FORMATETC * pformatetcOut) { return E_NOTIMPL; }
};

//////////////////////////////////////////////////////////////////////////

//消息框类
class SKIN_CONTROL_CLASS CSkinRichEdit : public CRichEditCtrl
{
	//变量定义
protected:
	bool								m_bScroll;						//是否滚动
	CString								m_strFontName;					//字体名字
	IRichEditOle						* m_pIRichEditOle;				//OLE 指针

	//函数定义
public:
	//构造函数
	CSkinRichEdit();
	//析构函数
	virtual ~CSkinRichEdit();

	//重载函数
protected:
	//控件绑定
	virtual void PreSubclassWindow();

	//功能函数 
public:
	//清除屏幕
	void CleanScreen();
	//是否滚动
	bool IsAutoScroll() { return m_bScroll; }
	//设置滚动
	void SetAutoScroll(bool bScroll) { m_bScroll=bScroll; }
	//加载消息
	bool LoadMessage(LPCTSTR pszFileName);
	//保存信息
	bool SaveMessage(LPCTSTR pszFileName);

	//信息插入
public:
	//插入图片
	bool InsertImage(CBitmap * pBitmap);
	//插入字符
	bool InsertString(LPCTSTR pszString, COLORREF crColor, bool bLink=false, WORD wSize=0, LPCTSTR pszFont=NULL);

	//内部函数
public:
	//加载回调
	static DWORD CALLBACK LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);
	//保存回调
	static DWORD CALLBACK SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);

	//消息映射
public:
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//销毁消息
	afx_msg void OnDestroy();
	//按下连接
	afx_msg void OnEnLink(NMHDR * pNMHDR, LRESULT * pResult);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	//右键消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//复制
	afx_msg void OnCopy() { Copy(); }
	//全部选择
	afx_msg void OnSelectAll() { SetSel(0, -1); }
	//清除
	afx_msg void OnClear() { ReplaceSel(TEXT("")); }
	//清除屏幕
	afx_msg void OnClearAll() { SetSel(0,GetWindowTextLength()); ReplaceSel(NULL); }
	//保存
	afx_msg void OnSave();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif