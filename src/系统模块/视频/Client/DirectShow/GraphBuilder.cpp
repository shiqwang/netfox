#include "StdAfx.h"
#include ".\graphbuilder.h"

//检测返回值
#ifndef V_RETURN
	#define V_RETURN(x)	{ hr = x; if( hr!=S_OK ) { /*DXTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true );*/ return hr; } }
#endif

//安全释放
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

CGraphBuilder::CGraphBuilder(REFCLSID rclist)
{
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pVideoWindow = NULL;
	m_pBaseFilter = NULL;

	if( FAILED(CoInitialize(NULL)) )
	{
		AfxMessageBox("CoInitialize Failed!\r\n");
	}

	Initialize(rclist);
}

CGraphBuilder::~CGraphBuilder(void)
{
	Release();
	CoUninitialize();
}

//初始化
HRESULT CGraphBuilder::Initialize( REFCLSID rclist )
{
	HRESULT hr = S_FALSE;
	// 创建IGraphBuilder接口
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraphBuilder);
	if( hr!=S_OK )
	{
		goto InitializeError;
	}

	//得到IMediaControl接口
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
	if( hr!=S_OK )
	{
		goto InitializeError;
	}

	//得到IVideoWindow接口
	hr = m_pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVideoWindow);
	if( hr!=S_OK )
	{
		goto InitializeError;
	}

	//初始化BaseFilter接口
	hr = CoCreateInstance( rclist, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&m_pBaseFilter);
	if( hr!=S_OK )
	{
		goto InitializeError;
	}

	hr = m_pGraphBuilder->AddFilter( m_pBaseFilter, L"" );
	if( hr!=S_OK )
	{
		goto InitializeError;
	}
	return hr;

InitializeError:
	Release();
	return hr;
}

//释放
void CGraphBuilder::Release( void )
{
	if(m_pMediaControl)
	{
		m_pMediaControl->Stop();
	}

	if( m_pBaseFilter )
	{
		m_pBaseFilter->Stop();
		m_pGraphBuilder->Disconnect( GetPin() );
		m_pGraphBuilder->RemoveFilter( m_pBaseFilter );
		m_pBaseFilter->Release();
		m_pBaseFilter=NULL;
	}

	//释放
	SAFE_RELEASE(m_pMediaControl);
	SAFE_RELEASE(m_pVideoWindow);
	SAFE_RELEASE(m_pGraphBuilder);
}

//开始
HRESULT CGraphBuilder::Run( void )
{
	if( m_pMediaControl )
	{
		return m_pMediaControl->Run();
	}
	return S_FALSE;
}

//暂停
HRESULT CGraphBuilder::Pause( void )
{
	if( m_pMediaControl )
	{
		return m_pMediaControl->Pause();
	}
	return S_FALSE;
}

//停止
HRESULT CGraphBuilder::Stop( void )
{
	if( m_pMediaControl )
	{
		return m_pMediaControl->Stop();
	}
	return S_FALSE;
}

HRESULT CGraphBuilder::SetupVideoWindow( HWND hWnd )
{
	//检查控件和句柄
	if( !m_pVideoWindow )
		return S_FALSE;

	HRESULT hr;

	//设置句柄
	V_RETURN( m_pVideoWindow->put_Owner( (OAHWND)hWnd ) );

	if( hWnd )
	{
		//设置类型
		V_RETURN( m_pVideoWindow->put_WindowStyle( WS_CHILD | WS_CLIPCHILDREN ) );

		//让图像充满整个窗口
		CRect rc;
		::GetClientRect( hWnd, &rc );
		V_RETURN( m_pVideoWindow->SetWindowPosition( 0, 0, rc.right, rc.bottom ) );

		//显示
		//V_RETURN( m_pVideoWindow->put_Visible( OATRUE ) );
	}
	else
	{
		//设置类型
		V_RETURN( m_pVideoWindow->put_WindowStyle( WS_POPUPWINDOW | WS_CAPTION ) );

		//让图像充满整个窗口
		V_RETURN( m_pVideoWindow->SetWindowPosition( 0, 0, 320, 240 ) );
	}

	return hr;
}

HRESULT CGraphBuilder::Visible( BOOL Visible )
{
	if(m_pVideoWindow)
	{
		HRESULT hr;
		V_RETURN( m_pVideoWindow->put_Visible(OAFALSE) );
		return hr;
	}
	return S_FALSE;
}

//取设备输出Pin
IPin* CGraphBuilder::GetPin(PIN_DIRECTION PinDirection)
{
	IPin * foundPin = NULL;

	if( m_pBaseFilter )
	{
		//创建枚举
		IEnumPins * pinEnum = NULL;
		if ( m_pBaseFilter->EnumPins(&pinEnum)==S_OK )
		{
			//复位
			pinEnum->Reset();

			//循环每个PIN
			ULONG fetchCount = 0;
			IPin * pin = NULL;
			while ( !foundPin && S_OK==(pinEnum->Next(1, &pin, &fetchCount)) && fetchCount )
			{
				if (pin)
				{
					//得到PIN信息
					PIN_INFO pinInfo;
					if ( S_OK==pin->QueryPinInfo(&pinInfo) )
					{
						//检测是否是输出PIN
						if (pinInfo.dir == PinDirection)
						{
							pin->AddRef(); //加一引用
							foundPin = pin; //返回PIN
						}
						pinInfo.pFilter->Release();
					}
					pin->Release();
				}
			}
			pinEnum->Release();
		}
	}
	if (foundPin)
	{
		foundPin->Release();
	}
    return foundPin;
}

HRESULT CGraphBuilder::AddFilter( IBaseFilter* pBaseFilter, LPCWSTR pName )
{
	if( !m_pGraphBuilder )
		return S_FALSE;
	return m_pGraphBuilder->AddFilter( pBaseFilter, pName );
	
}

HRESULT CGraphBuilder::RemoveFilter( IBaseFilter* pBaseFilter )
{
	if( !m_pGraphBuilder || !pBaseFilter )
		return S_FALSE;
	return m_pGraphBuilder->RemoveFilter( pBaseFilter );
}

HRESULT CGraphBuilder::Connect( IPin* pOutPin, IPin* pInPin )
{
	if( !m_pGraphBuilder )
		return S_FALSE;
	return m_pGraphBuilder->Connect( pOutPin, pInPin );
}

HRESULT CGraphBuilder::ConnectDirect( IPin* pOutPin, IPin* pInPin, AM_MEDIA_TYPE* pmt )
{
	if( !m_pGraphBuilder )
		return S_FALSE;
	return m_pGraphBuilder->ConnectDirect( pOutPin, pInPin, pmt );
}

HRESULT CGraphBuilder::Disconnect( IPin* Pin )
{
	if( !m_pGraphBuilder )
		return S_FALSE;
	return m_pGraphBuilder->Disconnect( Pin );
}

HRESULT CGraphBuilder::RenderFile( LPCWSTR lpFileName, LPCWSTR lpFilterName )
{
    HRESULT hr;
	V_RETURN( Stop() );

	if( !m_pGraphBuilder )
		return S_FALSE;
    return m_pGraphBuilder->RenderFile( lpFileName, NULL );
}