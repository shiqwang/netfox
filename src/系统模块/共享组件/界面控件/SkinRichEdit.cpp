#include "Stdafx.h"
#include "SkinRichEdit.h"

//////////////////////////////////////////////////////////////////////////

#define ID_RICH_COPY					WM_USER+20
#define ID_RICH_SELECT_ALL				WM_USER+21
#define ID_RICH_CLEAR					WM_USER+22
#define ID_RICH_CLEAR_ALL				WM_USER+24
#define ID_RICH_SAVE					WM_USER+25

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinRichEdit, CRichEditCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(EN_LINK, OnEnLink)
	ON_COMMAND(ID_RICH_COPY, OnCopy)
	ON_COMMAND(ID_RICH_CLEAR, OnClear)
	ON_COMMAND(ID_RICH_SAVE, OnSave)
	ON_COMMAND(ID_RICH_CLEAR_ALL, OnClearAll)
	ON_COMMAND(ID_RICH_SELECT_ALL, OnSelectAll)
	ON_NOTIFY_REFLECT(EN_LINK, OnEnLink)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CImageDataObject::CImageDataObject()
{
	m_ulRef=0;
	memset(&m_Stgmed,0,sizeof(m_Stgmed));
	memset(&m_Format,0,sizeof(m_Format));
}

//析构函数
CImageDataObject::~CImageDataObject()
{
	ReleaseStgMedium(&m_Stgmed);
}

//设置图片
void CImageDataObject::SetBitmap(HBITMAP hBitmap)
{
	STGMEDIUM Stgm;
	FORMATETC Format;
	Stgm.tymed=TYMED_GDI;
	Stgm.hBitmap=hBitmap;
	Stgm.pUnkForRelease=NULL;
	Format.cfFormat=CF_BITMAP;
	Format.ptd=NULL;
	Format.dwAspect=DVASPECT_CONTENT;
	Format.lindex=-1;
	Format.tymed=TYMED_GDI;
	SetData(&Format,&Stgm,TRUE);

	return;
}

//获取对象
IOleObject * CImageDataObject::GetOleObject(IOleClientSite * pOleClientSite, IStorage * pStorage)
{
	IOleObject * pOleObject=NULL;
	::OleCreateStaticFromData(this,IID_IOleObject,OLERENDER_FORMAT,&m_Format,pOleClientSite,pStorage,(void **)&pOleObject);
	return pOleObject;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinRichEdit::CSkinRichEdit()
{
	m_bScroll=true;
	m_pIRichEditOle=NULL;
	m_strFontName=TEXT("宋体");
}

//析构函数
CSkinRichEdit::~CSkinRichEdit()
{
	if (m_pIRichEditOle!=NULL) m_pIRichEditOle->Release();
}

//控件绑定
void CSkinRichEdit::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//初始化控件
	SetEventMask(ENM_LINK);
	SetAutoURLDetect(TRUE);
	SetOptions(ECOOP_OR,ECO_NOHIDESEL|ECO_AUTOWORDSELECTION);

	//获取接口
	m_pIRichEditOle=GetIRichEditOle();

	return;
}

//创建消息
int CSkinRichEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//初始化控件
	SetEventMask(ENM_LINK);
	SetAutoURLDetect(TRUE);
	SetOptions(ECOOP_OR,ECO_NOHIDESEL);

	//获取接口
	m_pIRichEditOle=GetIRichEditOle();

	return 0;
}

//清除屏幕
void CSkinRichEdit::CleanScreen()
{
	//清除屏幕
	SetSel(0,GetTextLength());
	ReplaceSel(TEXT(""));

	return;
}

//加载消息
bool CSkinRichEdit::LoadMessage(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//打开文件
	CFile File;
	if (File.Open(pszFileName,CFile::modeRead,NULL)==FALSE) return false;

	//读取文件
	EDITSTREAM EditStream;
	EditStream.dwCookie=(DWORD)(&File);
	EditStream.pfnCallback=LoadCallBack;
    StreamIn(SF_RTF,EditStream);

	return true;
}

//保存消息
bool CSkinRichEdit::SaveMessage(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//打开文件
	CFile File;
	if (File.Open(pszFileName,CFile::modeWrite|CFile::modeCreate,NULL)==FALSE) return false;

	//写入文件
	EDITSTREAM EditStream;
	EditStream.dwCookie=(DWORD)(&File);
	EditStream.pfnCallback=SaveCallBack;
    StreamOut(SF_RTF,EditStream);

	return true;
}

//插入图片
bool CSkinRichEdit::InsertImage(CBitmap * pBitmap)
{
	//效验状态
	if (pBitmap==NULL) return false;
	if (m_pIRichEditOle==NULL) return false;

	//保存信息
	CHARRANGE CharRange;
	GetSel(CharRange);

	//设置插入点
	SetSel(GetWindowTextLength(),GetWindowTextLength());

	//插入位图
	IStorage * pStorage=NULL;
	IOleObject * pOleObject=NULL;
	LPDATAOBJECT lpDataObject=NULL;
	IOleClientSite * pOleClientSite=NULL;
	CImageDataObject * pImageObject=NULL;
	try
	{
		//获取信息
		BITMAP BitmapInfo;
		pBitmap->GetBitmap(&BitmapInfo);

		//获取 DC
		CWnd * pMainWnd=AfxGetMainWnd();
		CDC * pDC=pMainWnd->GetDC();

		//拷贝位图
		CBitmap DestBitmap;
		CDC DCSource,DCDest;
		DCDest.CreateCompatibleDC(pDC);
		DCSource.CreateCompatibleDC(pDC);
		DestBitmap.CreateCompatibleBitmap(pDC,BitmapInfo.bmWidth,BitmapInfo.bmHeight);
		DCSource.SelectObject(pBitmap->GetSafeHandle());
		DCDest.SelectObject(&DestBitmap);
		DCDest.BitBlt(0,0,BitmapInfo.bmWidth,BitmapInfo.bmHeight,&DCSource,0,0,SRCCOPY);

		//清理资源
		DCDest.DeleteDC();
		DCSource.DeleteDC();
		pMainWnd->ReleaseDC(pDC);

		//建立新对象
		pImageObject=new CImageDataObject;
		pImageObject->QueryInterface(IID_IDataObject,(void * *)&lpDataObject);
		pImageObject->SetBitmap((HBITMAP)DestBitmap.Detach());
		m_pIRichEditOle->GetClientSite(&pOleClientSite);

		LPLOCKBYTES lpLockBytes=NULL;
		CreateILockBytesOnHGlobal(NULL,TRUE,&lpLockBytes);
		StgCreateDocfileOnILockBytes(lpLockBytes,STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE,0,&pStorage);
		pOleObject=pImageObject->GetOleObject(pOleClientSite,pStorage);
		OleSetContainedObject(pOleObject,TRUE);

		REOBJECT ReObject;
		ZeroMemory(&ReObject,sizeof(ReObject));
		ReObject.cbStruct=sizeof(ReObject);

		CLSID ClsID;
		pOleObject->GetUserClassID(&ClsID);
		ReObject.clsid=ClsID;
		ReObject.cp=REO_CP_SELECTION;
		ReObject.dvaspect=DVASPECT_CONTENT;
		ReObject.poleobj=pOleObject;
		ReObject.polesite=pOleClientSite;
		ReObject.pstg=pStorage;
		m_pIRichEditOle->InsertObject(&ReObject);
	}
	catch (...) { delete pImageObject; }
	if (pOleObject!=NULL) pOleObject->Release();
	if (pOleClientSite!=NULL) pOleClientSite->Release();
	if (pStorage!=NULL) pStorage->Release();
	if (lpDataObject!=NULL) lpDataObject->Release();

	//恢复信息
	if (m_bScroll==false) SetSel(CharRange);

	return true;
}

//插入字符
bool CSkinRichEdit::InsertString(LPCTSTR pszString, COLORREF crColor, bool bLink, WORD wSize, LPCTSTR pszFont)
{
	//保存信息
	CHARRANGE CharRange;
	GetSel(CharRange);

	//设置格式
	CHARFORMAT2 CharFormat;
	memset(&CharFormat,0,sizeof(CharFormat));
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR;
	CharFormat.crTextColor=crColor;
	if (bLink==true)
	{
		CharFormat.dwMask|=CFM_LINK;
		CharFormat.dwEffects=CFE_LINK;
	}
	
	//变量定义
	LONG lTextLength=GetTextLengthEx(GTL_NUMCHARS);
	bool bResume=(CharRange.cpMax!=CharRange.cpMin);//||(lTextLength!=CharRange.cpMin));
	
	//插入消息
	SetSel(-1,-1);
	SetSelectionCharFormat(CharFormat);
	ReplaceSel(pszString);

	//恢复信息
	if (bResume==true) SetSel(CharRange);

	return true;
}

//设置光标
BOOL CSkinRichEdit::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest==HTCAPTION)
	{
		SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}
	return __super::OnSetCursor(pWnd,nHitTest,message);
}

//加载回调
DWORD CALLBACK CSkinRichEdit::LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	*pcb=((CFile *)dwCookie)->Read(pbBuff,cb);
	return 0;
}

//保存回调
DWORD CALLBACK CSkinRichEdit::SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	*pcb=cb;
	((CFile *)dwCookie)->Write(pbBuff,cb);
	return 0;
}

//销毁消息
void CSkinRichEdit::OnDestroy()
{
	__super::OnDestroy();

	//释放接口
	if (m_pIRichEditOle!=NULL)
	{
		m_pIRichEditOle->Release();
		m_pIRichEditOle=NULL;
	}

	return;
}

//右键消息
void CSkinRichEdit::OnRButtonDown(UINT nFlags, CPoint Point)
{
	SetFocus();
	CMenu PopMenu;

	//建立菜单
	PopMenu.CreatePopupMenu();
	PopMenu.AppendMenu(0,ID_RICH_COPY,TEXT("复制(&C) \tCtrl+C"));
	PopMenu.AppendMenu(0,ID_RICH_SELECT_ALL,TEXT("全选(&A) \tCtrl+A"));
	PopMenu.AppendMenu(0,ID_RICH_CLEAR,TEXT("删除(&D)"));
	PopMenu.AppendMenu(0,ID_RICH_CLEAR_ALL,TEXT("清除屏幕"));
	PopMenu.AppendMenu(0,MF_SEPARATOR);
	PopMenu.AppendMenu(0,ID_RICH_SAVE,TEXT("保存信息..."));

	//设置菜单
	UINT nSel=((GetSelectionType()!=SEL_EMPTY)?0:MF_GRAYED);
	PopMenu.EnableMenuItem(ID_RICH_COPY,MF_BYCOMMAND|nSel);
	PopMenu.EnableMenuItem(ID_RICH_CLEAR,MF_BYCOMMAND|nSel);
	PopMenu.EnableMenuItem(ID_RICH_SELECT_ALL,MF_BYCOMMAND);

	//显示菜单
	ClientToScreen(&Point);
	PopMenu.TrackPopupMenu(TPM_RIGHTBUTTON,Point.x,Point.y,this);
	PopMenu.DestroyMenu();

	return;
}

//保存信息
afx_msg void CSkinRichEdit::OnSave()
{
	CFileDialog FileDlg(FALSE,"RTF",0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"文本文件(*.RTF)|*.RTF||",this);
	if (FileDlg.DoModal()==IDOK)
	{
		CFile File;
		if (File.Open(FileDlg.GetPathName(),CFile::modeCreate|CFile::modeReadWrite)==TRUE)
		{
            EDITSTREAM EditStream;
			EditStream.dwCookie=(DWORD)&File;
			EditStream.pfnCallback=SaveCallBack; 
			StreamOut(SF_RTF,EditStream);
			File.Close();
		}
		else AfxMessageBox(TEXT("目标路径不可写，信息保存失败！"));
	}
	return;
}

//按下连接
void CSkinRichEdit::OnEnLink(NMHDR * pNMHDR, LRESULT * pResult)
{
	ENLINK * pEnLink=reinterpret_cast<ENLINK *>(pNMHDR);

	//发送消息
	if ((pEnLink->msg==WM_LBUTTONDOWN)||(pEnLink->msg==WM_LBUTTONDBLCLK))
	{
		CString strBuffer;
		strBuffer.GetBufferSetLength((pEnLink->chrg.cpMax-pEnLink->chrg.cpMin)*2);
		strBuffer.ReleaseBuffer();
		GetTextRange(pEnLink->chrg.cpMin,pEnLink->chrg.cpMax,strBuffer);

		//网页分析
		LPCTSTR pszHttp[]=
		{
			TEXT("http://www.fox2046.cn"),
			TEXT("http://www.fox2046.net"),
			TEXT("http://www.fox2046.com"),
			TEXT("http://service.fox2046.cn"),
			TEXT("http://service.fox2046.net"),
			TEXT("http://service.fox2046.com"),
		};
		for (int i=0;i<CountArray(pszHttp);i++)
		{
			if (strBuffer.GetLength()>=lstrlen(pszHttp[i]))
			{
				if (memcmp((LPCTSTR)strBuffer,pszHttp[i],lstrlen(pszHttp[i]))==0)
				{
					ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_SHOWDEFAULT);
					return;
				}
			}
		}
	}

	*pResult=0;
}

//////////////////////////////////////////////////////////////////////////
