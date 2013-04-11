
// CaptureVideo.cpp : implementation file
//
/*
本文提供的视频捕捉类CcaptureVideo和示例，在Win2K + DirectX9 SDK + VC6 环境下调试通过。注意：编译时需要Strmiids.lib Quartz.lib两个库文件(DirectX9 SDK自带)。
DirectShow是微软公司提供的一套在Windows平台上进行流媒体处理的开发包，与DirectX开发包
一起发布。DirectShow为多媒体流的捕捉和回放提供了强有力的支持。用DirectShow开发应用
程序，我们可以很方便地从支持WDM驱动模型的采集卡上捕获数据，并且进行相应的后期处理乃
至存储到文件中。
DirectShow是基于COM的，为了编写DirectShow应用程序，需要了解COM客户程序编写的基础知
识。DirectShow提供了大量的接口，但在编程中发现还是不够方便，如果能构建一个视频捕捉
类把常用的一些动作封装起来，那么就更方便了
编程思路

  　　为了更加容易建立视频捕捉应用程序，DirectShow提供了一个叫做Capture Graph Builder
  的对象，Capture Graph Builder提供IcaptureGraphBuilder2接口，该接口可以建立和控制
  Capture Graph。
  
	　　建立视频捕捉程序，必须首先获取并初始化IcaptureGraphBuilder2接口，然后选择一个
	适当的视频捕捉设备。选择好设备后，为该设备创建Capture filter，然后调用AddFilter
	把Capture filter添加到Filter Graph。
	
	  　　如果仅仅希望用摄像头来进行实时监控的话，只需要在上面的基础上调用
	  ICaptureGraphBuilder2::RenderStream就可以了：
	  
		DirectShow提供了一个捕捉静态图像的方法：使用Sample Grabber filter。依次按照以下三个步骤就可以了：
		
		  
*/

#include "stdafx.h"
#include "FlyGameAVClient.h"
#include "CaptureVideo.h"
//#include "FlyGameBJLDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int allNum=0;
/////////////////////////////////////////////////////////////////////////////
// CCaptureVideo
//定义一个类实现Sample Grabber的回调接口IsampleGrabberCB
class CSampleGrabberCB : public ISampleGrabberCB 
{
public:
	int videoTypeInt;//0=小;1=大视频
	long lWidth;
	long lHeight;
	CFlyMsgManager* pFly;

	TCHAR m_szFileName[MAX_PATH];// 位图文件名称
	
	CSampleGrabberCB( ){
		strcpy(m_szFileName, "c:\\donaldo.bmp");
	} 
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv){
		if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ){ 
			*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
			return NOERROR;
		} 
		return E_NOINTERFACE;
	}
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample ){
		return 0;
	}
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize ){
		//doctor 		if( !bOneShot )return 0;
		
		if (!pBuffer)return E_POINTER;
		CString strFile,strTemp;
		CTime tmCur = CTime::GetCurrentTime();
		CString strTime = tmCur.Format("%m%d");
		strFile.Format("log\\%sCSampleGrabberCB.log",strTime);

//		CFlyMsgManager* pDlg=(CFlyMsgManager*)::AfxGetMainWnd();
		if (pFly->m_bVideoSend)
		{
			
			
			int len;
			
			len = lBufferSize;
			//	pDlg->OnEncodeVideoData( Portbuffer,  len);
			strTemp.Format("videoTypeInt=%d)", videoTypeInt);
			theApp.WriteLog(strFile, strTemp);
			
		if (len > 0)
		{

					pFly->OnEncodeVideoData( (char *)pBuffer,  len);
					strTemp.Format("OnEncodeVideoData()");
					theApp.WriteLog(strFile, strTemp);
		}
	
		}

		strTemp.Format("SaveBitmap all=%d", allNum);
		//	theApp.WriteLog(strFile, strTemp);
		
		//		SaveBitmap(pBuffer, lBufferSize);
		//		allNum++;
		//doctor 		bOneShot = FALSE;
		return 0;
	}
	//创建位图文件
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize )
	{
		HANDLE hf = CreateFile(
			m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, NULL, NULL );
		if( hf == INVALID_HANDLE_VALUE )return 0;
		//写文件头 
		BITMAPFILEHEADER bfh;
		memset( &bfh, 0, sizeof( bfh ) );
		bfh.bfType ='MB';
		bfh.bfSize = sizeof( bfh ) + lBufferSize + sizeof( BITMAPINFOHEADER );
		bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
		DWORD dwWritten = 0;
		WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );
		// 写位图格式
		BITMAPINFOHEADER bih;
		memset( &bih, 0, sizeof( bih ) );
		bih.biSize = sizeof( bih );
		bih.biWidth = lWidth;
		bih.biHeight = lHeight;
		bih.biPlanes = 1;
		bih.biBitCount = 24;
		WriteFile( hf, &bih, sizeof( bih ), &dwWritten, NULL );
		// 写位图数据
		WriteFile( hf, pBuffer, lBufferSize, &dwWritten, NULL );
		CloseHandle( hf );
		return 0;
	}
};
//CSampleGrabberCB mCB;
CSampleGrabberCB *mpCB = NULL;
//初始化
CCaptureVideo::CCaptureVideo()
{
	//COM Library Intialization
	if(FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
	{
		AfxMessageBox("CoInitialize Failed!\r\n"); 
		return;
	}
	m_hWnd = NULL;
	m_pVW = NULL;
	m_pMC = NULL;
	m_pGB = NULL;
	m_pCapture = NULL; 

	fPreviewing = FALSE;

	m_IsHaveVideoDev = IsHaveVideoDev();

}
CCaptureVideo::~CCaptureVideo()
{
	
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%s OutSys.log",strTime);
	

	
	if (m_IsHaveVideoDev)
	{
	strTemp.Format("~CCaptureVideo()");
	theApp.WriteLog(strFile, strTemp);
		CloseCaptureDev();
	}
}


BEGIN_MESSAGE_MAP(CCaptureVideo, CWnd)
//{{AFX_MSG_MAP(CCaptureVideo)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCaptureVideo message handlers



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//hList是下拉列表框的句柄，本函数用于枚举当前系统安装的所有视频捕捉设备
int CCaptureVideo::EnumDevices(HWND hList)
{
	if (!hList)
		return -1;
	int id = 0;
	
	//枚举视频扑捉设备
	ICreateDevEnum *pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	
	if (hr != NOERROR)return -1;
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	
	if (hr != NOERROR)return -1;
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				TCHAR str[2048]; 
				id++;
				WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
				::SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)str);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
	}
	return id;
}

//iDeviceID是视频捕捉设备序号，hWnd是视频捕捉窗口的句柄
HRESULT CCaptureVideo::Init(int iDeviceID, HWND hWnd  ,CFlyMsgManager * myP)
//HRESULT CCaptureVideo::Init(int iDeviceID, HWND hWnd )
{

	//没有视频设备,直接退出
	if (!m_IsHaveVideoDev)
		return 0;

	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sCCaptureVideo.log",strTime);
	
	strTemp.Format("CCaptureVideo::Init");
	theApp.WriteLog(strFile, strTemp);


	HRESULT hr;
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr)){
		AfxMessageBox("Failed to get video interfaces!");
		return hr;
	}

	hr =	BuildCaptureGraph( iDeviceID,  hWnd);
	if( FAILED( hr ) )
	{
		AfxMessageBox("Failed to BuildCaptureGraph!");
		return hr;
	}
	strTemp.Format("BuildCaptureGraph()");
	theApp.WriteLog(strFile, strTemp);

	BuildDataStream(myP);
	strTemp.Format("BuildDataStream()");
	theApp.WriteLog(strFile, strTemp);

	StartPreview();
	strTemp.Format("StartPreview()");
	theApp.WriteLog(strFile, strTemp);

	return S_OK;
}
//输入设备号码,捆绑
bool CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
		return false;
	
	// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	{
		return false;
	}
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR) 
	{
		return false;
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				if (index == deviceId)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}
	return true;
}
//初始化采集Capture Graph Builder
HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
	HRESULT hr;
	
	//IGraphBuilder  m_pGB
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
	//ICaptureGraphBuilder2 m_pCapture
	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
	if (FAILED(hr))
	{
		return hr;
	}
	m_pCapture->SetFiltergraph(m_pGB);

	//m_pMC
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	if (FAILED(hr))
	{
		return hr;
	}

	//m_pVW
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
	
}
//设置捕捉窗口
HRESULT CCaptureVideo::SetupVideoWindow()
{
	HRESULT hr;
	hr = m_pVW->put_Owner((OAHWND)m_hWnd);
	if (FAILED(hr))return hr;
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))return hr;
	ResizeVideoWindow();
	hr = m_pVW->put_Visible(OATRUE);
	return hr;
}
//重新设置窗口大小
void CCaptureVideo::ResizeVideoWindow()
{
	if (m_pVW){
		//è?í???3??ú????′°?ú
		CRect rc;
		::GetClientRect(m_hWnd,&rc);
		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	} 
}
//调用GrabOneFrame(true)就可以捕获当前的静态图像并保存到硬盘上
void CCaptureVideo::GrabOneFrame(BOOL bGrab)
{
	bOneShot = bGrab;
}
void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) {
		CoTaskMemFree((PVOID)mt.pbFormat);
		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) {
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
} 

void CCaptureVideo::CloseCaptureDev()
{
	try 
	{
		CString strFile,strTemp;
		CTime tmCur = CTime::GetCurrentTime();
		CString strTime = tmCur.Format("%m%d");
		strFile.Format("log\\%s OutSys.log",strTime);
		
		strTemp.Format("CloseCaptureDev()");
//		theApp.WriteLog(strFile, strTemp);
		

		// Stop media playback
//		if(m_pMC)m_pMC->Stop();
		StopPreview();
		strTemp.Format("CloseCaptureDev1()");
	//	theApp.WriteLog(strFile, strTemp);

		


		TearDownGraph();

		strTemp.Format("CloseCaptureDev2()");
	//	theApp.WriteLog(strFile, strTemp);
		
		FreeCapFilters();
		
		strTemp.Format("CloseCaptureDev3()");
	//	theApp.WriteLog(strFile, strTemp);
		
//		CoUninitialize( );
		
		strTemp.Format("CloseCaptureDev4()");
	//	theApp.WriteLog(strFile, strTemp);
	}
	catch (...)
	{
		
	}
	
}

//FREE FILTER
void CCaptureVideo::FreeCapFilters()
{
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	SAFE_RELEASE(m_pCapture);
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW);
}


//销毁GRAPH
void CCaptureVideo::TearDownGraph()
{
	//关闭窗口
	if(m_pVW)
	{
		m_pVW->put_Owner(NULL);
		m_pVW->put_Visible(OAFALSE);

		m_pVW->Release();
		m_pVW= NULL;
	}

    // destroy the graph downstream of our capture filters
    if(m_pBF)
        NukeDownstream(m_pBF);

    fPreviewGraphBuilt = FALSE;
}


// Tear down everything downstream of a given filter
void CCaptureVideo::NukeDownstream(IBaseFilter *pf)
{
    IPin *pP=0, *pTo=0;
    ULONG u;
    IEnumPins *pins = NULL;
    PIN_INFO pininfo;

    if (!pf)
        return;

    HRESULT hr = pf->EnumPins(&pins);
    pins->Reset();

    while(hr == NOERROR)
    {
        hr = pins->Next(1, &pP, &u);
        if(hr == S_OK && pP)
        {
            pP->ConnectedTo(&pTo);
            if(pTo)
            {
                hr = pTo->QueryPinInfo(&pininfo);
                if(hr == NOERROR)
                {
                    if(pininfo.dir == PINDIR_INPUT)
                    {
                        NukeDownstream(pininfo.pFilter);
                        m_pGB->Disconnect(pTo);
                        m_pGB->Disconnect(pP);
                        m_pGB->RemoveFilter(pininfo.pFilter);
                    }
                    pininfo.pFilter->Release();
                }
                pTo->Release();
            }
            pP->Release();
        }
    }

    if(pins)
        pins->Release();
}

// stop the preview graph
//停止预览
BOOL CCaptureVideo::StopPreview()
{
    // way ahead of you
    if(!fPreviewing)
    {
        return FALSE;
    }

     // stop the graph
    IMediaControl *pMC = NULL;
    HRESULT hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if(SUCCEEDED(hr))
    {
        hr = pMC->Stop();
        pMC->Release();
    }
    if(FAILED(hr))
    {
       // ErrMsg(TEXT("Error %x: Cannot stop preview graph"), hr);
		AfxMessageBox("Cannot stop preview graph!");
        return FALSE;
    }

    fPreviewing = FALSE;

    return TRUE;
}

// build the capture graph
//建立捕捉 GRAPH
HRESULT CCaptureVideo::BuildCaptureGraph(int iDeviceID, HWND hWnd)
{
	HRESULT hr;
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sCCaptureVideo.log",strTime);
	
	strTemp.Format("BuildCaptureGraph::Init");
	theApp.WriteLog(strFile, strTemp);

	// Bind Device Filter. We know the device because the id was passed in
	if(!BindFilter(iDeviceID, &m_pBF))return S_FALSE;

	strTemp.Format("BuildCaptureGraph::1");
	theApp.WriteLog(strFile, strTemp);

	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
	// hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, 
	// m_pBF, NULL, NULL);
	// create a sample grabber

	strTemp.Format("BuildCaptureGraph::2");
	theApp.WriteLog(strFile, strTemp);

	hr = m_pGrabber.CoCreateInstance( CLSID_SampleGrabber );
	if( !m_pGrabber )
	{
		AfxMessageBox("Fail to create SampleGrabber, maybe qedit.dll is not registered?");
		return hr;
	}
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabBase( m_pGrabber );

	strTemp.Format("BuildCaptureGraph::3");
	theApp.WriteLog(strFile, strTemp);

	//设置视频格式
	//1.定义IAMStreamConfig Interface
	IAMStreamConfig *pConfig;
	//2.初始化IAMStreamConfig Interface
	hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Interleaved, m_pBF,IID_IAMStreamConfig,(void **)&pConfig);
	
	
	if( hr != NOERROR )
	{
		hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,m_pBF,IID_IAMStreamConfig,(void **)&pConfig);
		if(hr == NOERROR)
		{
		//	AfxMessageBox("load config filter succeed!");
		}
		else
		{
			AfxMessageBox("load config filter failed!");
			return hr;
		}//End if(hr == NOERROR)
	}//End if( hr != NOERROR )
	
	AM_MEDIA_TYPE * mmt;
	
	pConfig->GetFormat(&mmt);    //取得默认参数
	
	VIDEOINFOHEADER * vvih = (VIDEOINFOHEADER*) mmt->pbFormat;
	
	AM_MEDIA_TYPE * pmt = mmt;
	
	VIDEOINFOHEADER * pvih = (VIDEOINFOHEADER*) pmt->pbFormat;
	
	
	pvih->bmiHeader.biHeight=QCIF_HEIGHT;//288;  //修改采集视频的高为288
	pvih->bmiHeader.biWidth= QCIF_WIDTH;//352;   //修改采集视频的宽为352
	mmt->majortype = MEDIATYPE_Video;
	mmt->subtype = MEDIASUBTYPE_RGB24;
	
	pmt->pbFormat = (unsigned char *) pvih;
	
	pConfig->SetFormat(pmt);   //重新设置参数
	
	
	
							   /*
							   AM_MEDIA_TYPE mt; 
							   ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
							   mt.majortype = MEDIATYPE_Video;
							   mt.subtype = MEDIASUBTYPE_RGB24;
							   hr = m_pGrabber->SetMediaType(&mt);
							   
								 if( FAILED( hr ) ){
								 AfxMessageBox("Fail to set media type!");
								 return hr;
								 }
	*/
	hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
	if( FAILED( hr ) )
	{
		AfxMessageBox("Fail to put sample grabber in graph");
		return hr;
	}
	//第二步、调用RenderStream依次把Still pin、Sample Grabber和系统默认Renderer Filter连接起来。
	
	// try to render preview/capture pin
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
	if( FAILED( hr ) )
	{
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
	}
	
	if( FAILED( hr ) )
	{
		AfxMessageBox("Can not build the graph");
		return hr;
	}
	/*	
	hr = m_pGrabber->GetConnectedMediaType( &mt );
	if ( FAILED( hr) ){
	AfxMessageBox("Failt to read the connected media type");
	return hr;
	}
	*/	
	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mmt->pbFormat;


			//设置类型
	//		mCB.pFly = myP ;
//mpCB->pFly = myP ;;
			
			
//			strTemp.Format("mCB.videoTypeInt=%d, myWopType=%d)", mCB.videoTypeInt, myWopType);
//			theApp.WriteLog(strFile, strTemp);
			
	/*		
			mCB.lWidth = vih->bmiHeader.biWidth;
			mCB.lHeight = vih->bmiHeader.biHeight;
			*/
//mpCB->lWidth = vih->bmiHeader.biWidth;
//mpCB->lHeight = vih->bmiHeader.biHeight;
			//	FreeMediaType(mt);
			
			//配置Sample Grabber以捕获数据
//			hr = m_pGrabber->SetBufferSamples( FALSE );
//			hr = m_pGrabber->SetOneShot( FALSE );
//			hr = m_pGrabber->SetCallback( &mCB, 1 );
			
//			hr = m_pGrabber->SetCallback( mpCB, 1 );			

	
	//设置视频捕捉窗口
	m_hWnd = hWnd ; 
	SetupVideoWindow();


    return TRUE;
}

//建立预览 GRAPH
BOOL CCaptureVideo::BuildPreviewGraph()
{
    return TRUE;
}

// Start previewing
//
BOOL CCaptureVideo::StartPreview()
{
	HRESULT hr;
 // way ahead of you
    if(fPreviewing)
        return TRUE;

	hr = m_pMC->Run();//开始视频捕捉
	if(FAILED(hr))
	{
		AfxMessageBox("Couldn?ˉt run the graph!");
		return FALSE;
	}


    return TRUE;
}

//建立数据流
HRESULT CCaptureVideo::BuildDataStream(CFlyMsgManager * myP)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sCCaptureVideo.log",strTime);
	
	strTemp.Format("BuildDataStream::Init");
	theApp.WriteLog(strFile, strTemp);

	HRESULT hr;
	if (!mpCB)
	{
		mpCB = new CSampleGrabberCB;
	}

	strTemp.Format("BuildDataStream::1");
	theApp.WriteLog(strFile, strTemp);

	//		mCB.pFly = myP ;
	mpCB->pFly = myP ;

	strTemp.Format("BuildDataStream::1A");
	theApp.WriteLog(strFile, strTemp);

	if (mpCB)
	{
		mpCB->lWidth = QCIF_WIDTH;
		mpCB->lHeight = QCIF_HEIGHT;
	}
	else
	{
		strTemp.Format("BuildDataStream::1c");
	theApp.WriteLog(strFile, strTemp);	
	}
	//	FreeMediaType(mt);
	strTemp.Format("BuildDataStream::1B");
	theApp.WriteLog(strFile, strTemp);

	//配置Sample Grabber以捕获数据
	if (m_pGrabber)
	{
		hr = m_pGrabber->SetBufferSamples( FALSE );
		if( FAILED( hr ) )
		{
					return hr;
		}
	
	strTemp.Format("BuildDataStream::2");
	theApp.WriteLog(strFile, strTemp);

	hr = m_pGrabber->SetOneShot( FALSE );
	if( FAILED( hr ) )
	{
				return hr;
	}
	strTemp.Format("BuildDataStream::3");
	theApp.WriteLog(strFile, strTemp);
	//			hr = m_pGrabber->SetCallback( &mCB, 1 );

	hr = m_pGrabber->SetCallback( mpCB, 1 );			
	if( FAILED( hr ) )
	{
			return hr;
	}
	}
  	strTemp.Format("BuildDataStream::4");
	theApp.WriteLog(strFile, strTemp); 
	return hr;
}

	//功能:判断是否有视频设备
	//输入:
	//返回:
BOOL CCaptureVideo::IsHaveVideoDev()
{
BOOL resultBOOL = TRUE;
    HRESULT hr;
    // enumerate all video capture devices
    ICreateDevEnum *pCreateDevEnum=0;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                          IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if(hr != NOERROR)
    {
      //  ErrMsg(TEXT("Error Creating Device Enumerator"));
		resultBOOL = FALSE;
        return resultBOOL;
    }

    IEnumMoniker *pEm=0;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    if(hr != NOERROR)
    {
       // ErrMsg(TEXT("Sorry, you have no video capture hardware.\r\n\r\n")
       //        TEXT("Video capture will not function properly."));
       // goto EnumAudio;
		resultBOOL = FALSE;
    }
return resultBOOL;	
}