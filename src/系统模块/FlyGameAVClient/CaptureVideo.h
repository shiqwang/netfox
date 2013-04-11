#if !defined(AFX_CAPTUREVIDEO_H__4CBD2E80_74FD_4D0F_B9F1_35914CFB48BA__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__4CBD2E80_74FD_4D0F_B9F1_35914CFB48BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptureVideo.h : header file
//
//#include <Gdiplus.h> //GDI++
//using namespace Gdiplus;

//#include <atlbase.h>
//#include <windows.h>
#include <dshow.h>
//#include "streams.h"

#include <stdio.h>
#include <windows.h>
#include "streams.h"
#include <qedit.h>
#include <atlbase.h>

#include <vfw.h>
#pragma comment(lib,"vfw32")

/////////////////////////////////////////////////////////////////////////////
// CCaptureVideo window
class CSampleGrabberCB;
class CFlyMsgManager ;
class CCaptureVideo : public CWnd
{
	friend class CSampleGrabberCB;
	
public:
	BOOL m_IsHaveVideoDev;
	//功能:判断是否有视频设备
	//输入:
	//返回:
	BOOL IsHaveVideoDev();


	BOOL fPreviewing;//预览中
    BOOL fPreviewGraphBuilt;//预览是否建立成功

//建立数据流
	HRESULT BuildDataStream(CFlyMsgManager * myP);

	// stop the preview graph
	//停止预览
	BOOL StopPreview();
	// Start previewing
	//
	BOOL StartPreview();
	
	//建立预览 GRAPH
	BOOL BuildPreviewGraph();
	
	HRESULT BuildCaptureGraph(int iDeviceID, HWND hWnd);
	// stop the preview graph
	
	//销毁GRAPH
	void NukeDownstream(IBaseFilter *pf);
	void TearDownGraph();
	void FreeCapFilters();
	void CloseCaptureDev();
	
	//CSampleGrabberCB mCB;
	void GrabOneFrame(BOOL bGrab);
	HRESULT Init(int iDeviceID,HWND hWnd,CFlyMsgManager * myP);//0=小视频;1=大视频
	//HRESULT Init(int iDeviceID,HWND hWnd);//0=小视频;1=大视频
	
	int EnumDevices(HWND hList);
	
private:
	HWND m_hWnd;
	IGraphBuilder *m_pGB;//GRAPH BUILER
	ICaptureGraphBuilder2* m_pCapture;//
	IBaseFilter* m_pBF;//基本
	IMediaControl* m_pMC;//控制
	IVideoWindow* m_pVW;//视频窗口
	CComPtr<ISampleGrabber> m_pGrabber;//
	
protected:
	void FreeMediaType(AM_MEDIA_TYPE& mt);
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
	void ResizeVideoWindow();
	HRESULT SetupVideoWindow();
	HRESULT InitCaptureGraphBuilder();
	
	// Construction
public:
	CCaptureVideo();
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptureVideo)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CCaptureVideo();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CCaptureVideo)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTUREVIDEO_H__4CBD2E80_74FD_4D0F_B9F1_35914CFB48BA__INCLUDED_)
