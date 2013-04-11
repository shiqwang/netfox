#include "StdAfx.h"
#include ".\capturedevice.h"

#include <streams.h>

//检测返回值
#ifndef V_RETURN
	#define V_RETURN(x)	{ hr = x; if( hr!=S_OK ) { /*DXTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true );*/ return hr; } }
#endif

//安全释放
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

CCaptureDevice::CCaptureDevice(void)
{
	m_pBaseFilter = NULL;
	if( FAILED(CoInitialize(NULL)) )
	{
		AfxMessageBox("CoInitialize Failed!\r\n");
	}
}

CCaptureDevice::~CCaptureDevice(void)
{
	Release();
	CoUninitialize();
}

//枚举设备
HRESULT CCaptureDevice::EnumDevices( REFCLSID clsidDeviceClass, ENUMCALLBACK pEnumCallBack, LPVOID lp )
{
	//创建枚举组件
    ICreateDevEnum *pCreateDevEnum;
	HRESULT hr;
	VARIANT var;
	hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum );
	if( hr==S_OK )
	{
		//创建枚举器
		IEnumMoniker* pEm;
		hr = pCreateDevEnum->CreateClassEnumerator( clsidDeviceClass, &pEm, 0 );
		if( hr==S_OK )
		{
			if( pEm )
			{
				//复位
				pEm->Reset();

				//准备顺序找设备
				ULONG cFetched;
				IMoniker *pM;
				BOOL bCancel = false;
				int index = 0;
				while( !bCancel && (hr = pEm->Next(1, &pM, &cFetched), hr==S_OK) )
				{
					IPropertyBag *pBag;
					hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
					if(hr==S_OK)
					{
						ZeroMemory( &var, sizeof(var) );
						var.vt = VT_BSTR;
						hr = pBag->Read(L"FriendlyName", &var, NULL);
						if(hr==S_OK && var.bstrVal!=0 )
						{
							if(pEnumCallBack)
								hr = pEnumCallBack( index, &var, pM, lp, bCancel );
							SysFreeString(var.bstrVal);
						}
						pBag->Release();
					}
					pM->Release();
					index++;
				}

				//释放
				pEm->Release();
			}
		}
		pCreateDevEnum->Release();
	}
	return hr;
}

void CCaptureDevice::Release( void )
{
	//释放
	SAFE_RELEASE(m_pBaseFilter);
}

typedef struct _BIND_DEVICE_PARAMETER
{
	int DeviceIndex;
	IBaseFilter** ppBaseFilter;
}BIND_DEVICE_PARAMETER;

//设置绑定设备
HRESULT CALLBACK BindFilterCallback( int index, VARIANT* var/*返回值*/, IMoniker* pM/*设备*/, LPVOID lp/*参数*/, BOOL& bCancel/*是否退出循环*/ )
{
	BIND_DEVICE_PARAMETER *pBindDeviceParameter = (BIND_DEVICE_PARAMETER*)lp;

 	if( index == pBindDeviceParameter->DeviceIndex )
	{
        HRESULT hr = pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pBindDeviceParameter->ppBaseFilter);
		bCancel = TRUE;
		return hr;
	}
    return S_FALSE;
}

//设置绑定设备
HRESULT CCaptureDevice::BindFilter( REFCLSID clsidDeviceClass, int DeviceIndex )
{
	if( DeviceIndex<0 )
		return S_FALSE;

	BIND_DEVICE_PARAMETER BindDeviceParameter; 
	BindDeviceParameter.DeviceIndex = DeviceIndex;
	BindDeviceParameter.ppBaseFilter = &m_pBaseFilter;

	//枚举绑定
	HRESULT hr;
	hr = EnumDevices( clsidDeviceClass, BindFilterCallback, &BindDeviceParameter );
	if( hr==S_OK )
		AdjustOutput();
	return hr;
}

//取设备输出Pin
IPin* CCaptureDevice::GetPin(void)
{
	IPin * foundPin = NULL;

	if( m_pBaseFilter )
	{
		//创建枚举
		IEnumPins * pinEnum = NULL;
		if (m_pBaseFilter->EnumPins(&pinEnum)==S_OK)
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
					if (S_OK==pin->QueryPinInfo(&pinInfo))
					{
						//检测是否是输出PIN
						if (pinInfo.dir == PINDIR_OUTPUT)
						{
//							pin->AddRef(); //加一引用
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
//	if (foundPin)
//	{
//		foundPin->Release();
//	}
    return foundPin;
}

// Preferred capture size
const long Preferred_Width		= 160;
const long Preferred_Height		= 120;

void CCaptureDevice::AdjustOutput(void)
{
	HRESULT  hr         = S_OK;
	AM_MEDIA_TYPE * pmt = NULL;
	LONGLONG avgTimePerFrame = 2000000;  // 20fps

	pmt = SelectMediaType();
	if (pmt)
	{
		if (pmt->formattype == FORMAT_VideoInfo) 
		{
			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pmt->pbFormat;
			pvi->AvgTimePerFrame       = avgTimePerFrame;
			pvi->bmiHeader.biWidth     = Preferred_Width;
			pvi->bmiHeader.biHeight    = Preferred_Height;
			pvi->bmiHeader.biSizeImage = Preferred_Width * Preferred_Height * pvi->bmiHeader.biBitCount / 8;
			
			IAMStreamConfig * pCfg = GetStreamConfig();
			hr = pCfg->SetFormat(pmt);
		}
		DeleteMediaType(pmt);
	}
}

AM_MEDIA_TYPE * CCaptureDevice::SelectMediaType(void)
{
	// Preferred sequence: UYVY, YUY2, RGB565, RGB555, RGB24, RGB32  
	VIDEO_STREAM_CONFIG_CAPS  pSCC;
	AM_MEDIA_TYPE * pmt = NULL;
	HRESULT hr = S_OK;
	int nCounts=0, nSize=0;
	int preferredIndex = -1;
	enum {
		UYVY = 0, YUY2, RGB565, RGB555, RGB24, RGB32, Unknown
	} currentPreferred, temp;
	currentPreferred = Unknown;

	IAMStreamConfig * pCfg = GetStreamConfig();
	pCfg->GetNumberOfCapabilities(&nCounts, &nSize);
	for (int i = 0; i < nCounts; i++)
	{
		if (pCfg->GetStreamCaps(i, &pmt, (BYTE *)&pSCC) == S_OK)
		{
			if (pmt->subtype == MEDIASUBTYPE_RGB32)
			{
				temp = RGB32;
			}
			else if (pmt->subtype == MEDIASUBTYPE_RGB24)
			{
				temp = RGB24;
			}
			else if (pmt->subtype == MEDIASUBTYPE_RGB565)
			{
				temp = RGB565;
			}
			else if (pmt->subtype == MEDIASUBTYPE_RGB555)
			{
				temp = RGB555;
			}
			else if (pmt->subtype == MEDIASUBTYPE_YUY2)
			{
				temp = YUY2;
			}
			else if (pmt->subtype == MEDIASUBTYPE_UYVY)
			{
				temp = UYVY;
			}
			else
			{
				temp = Unknown;
			}

			if (temp < currentPreferred)
			{
				currentPreferred = temp;
				preferredIndex   = i;
			}
			DeleteMediaType(pmt);
		}
	}

	// Get the preferred media type
	if (preferredIndex != -1)
	{
		hr = pCfg->GetStreamCaps(preferredIndex, &pmt, (BYTE *)&pSCC);
	}
	else
	{
		hr = pCfg->GetFormat(&pmt);
	}

	return pmt;
}

IAMStreamConfig * CCaptureDevice::GetStreamConfig(void)
{
	IAMStreamConfig * pConfig = NULL;
	if (m_pBaseFilter)
	{
		// Get the capture output pin first
		IPin * pCapture = GetPin();
		if (pCapture)
		{
			pCapture->QueryInterface(IID_IAMStreamConfig, (void **)&pConfig);
		}

		if (pConfig)
		{
			pConfig->Release();
		}
	}
	return pConfig;
}

void CCaptureDevice::EnumInputList( ENUMINPUTCALLBACK EnumInputCallBack, LPVOID lp )
{
	char pinName[128];
	if (m_pBaseFilter)
	{
		IEnumPins * pinEnum = NULL;
		if ( m_pBaseFilter->EnumPins(&pinEnum) == S_OK )
		{
			pinEnum->Reset();

			BOOL bCancel  = FALSE;
			IPin * pin = NULL;
			ULONG fetchCount = 0;
			while (!bCancel && pinEnum->Next(1, &pin, &fetchCount)==S_OK && fetchCount)
			{
				if (pin)
				{
					PIN_INFO pinInfo;
					if ( pin->QueryPinInfo(&pinInfo)==S_OK )
					{
						pinInfo.pFilter->Release();
						if (pinInfo.dir == PINDIR_INPUT)
						{
							::WideCharToMultiByte(CP_ACP, 0, pinInfo.achName, 
								-1,	pinName, 128, NULL, NULL);
							if( EnumInputCallBack( pinName, pin, lp, bCancel ) != S_OK )
								bCancel = TRUE;
						}
						pin->Release();
						pin = NULL;
					}
				}
				else
				{
					bCancel = FALSE;
				}
			}
			pinEnum->Release();
		}
	}
}

IAMAudioInputMixer * GetMixer(IPin * inPin)
{
	IAMAudioInputMixer * pMixer = NULL;
	inPin->QueryInterface(IID_IAMAudioInputMixer, (void**) &pMixer);
	if (pMixer)
	{
		pMixer->Release();
	}
	return pMixer;
}

HRESULT CALLBACK EnumInputCallBack( const char* pName/*返回值*/, IPin * pin /*设备*/, LPVOID lp/*参数*/, BOOL& bCancel/*是否退出循环*/ )
{
	char* p = (char*)lp;
	if( strcmp( p, pName )==0 )
	{
		bCancel = TRUE;
		IAMAudioInputMixer * pMixer = GetMixer(pin);
		if( pMixer->put_Enable(TRUE)==S_OK )
		{
			if( pMixer->put_MixLevel(AMF_AUTOMATICGAIN) != S_OK )
			{
				return S_FALSE;
			}
		}
		else
			return S_FALSE;
	}
	return S_OK;
}

void CCaptureDevice::SelectInput( char* pName )
{
    EnumInputList( EnumInputCallBack, pName );
}

void CCaptureDevice::SetCaptureBufferSize(void)
{
	IPin * pCapturePin = GetPin();
	if (pCapturePin)
	{
		DWORD  dwBytesPerSec = 0;
		AM_MEDIA_TYPE * pmt = {0};
		IAMStreamConfig * pCfg = NULL;
		HRESULT hr = pCapturePin->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
		if ( hr==S_OK )
		{
            hr = pCfg->GetFormat(&pmt);
			if ( hr==S_OK )
			{
				WAVEFORMATEX *pWF = (WAVEFORMATEX *) pmt->pbFormat;
				dwBytesPerSec = pWF->nAvgBytesPerSec;
				pWF->nChannels = 1;
				pWF->wBitsPerSample = 8;
				pWF->nSamplesPerSec = 11025;
				pWF->nAvgBytesPerSec = pWF->nSamplesPerSec * pWF->nChannels * pWF->wBitsPerSample / 8;
				pWF->nBlockAlign = 1;
/*
	info.cbSize = sizeof(WAVEFORMATEX);
	info.wFormatTag = 1;
	info.nChannels = 2;
	info.nSamplesPerSec = 44100;
	//info.nSamplesPerSec = 22050;
	11025
	info.wBitsPerSample = 16;
	info.nAvgBytesPerSec = info.nSamplesPerSec * info.nChannels * info.wBitsPerSample / 8;
	info.nBlockAlign = 4;
	*/
				pCfg->SetFormat( pmt );
				DeleteMediaType(pmt);
			}
			pCfg->Release();
		}
/*		if (dwBytesPerSec)
		{
			IAMBufferNegotiation * pNeg = NULL;
			hr = pCapturePin->QueryInterface(IID_IAMBufferNegotiation, 
				(void **)&pNeg);
			if (SUCCEEDED(hr))
			{
				ALLOCATOR_PROPERTIES AllocProp;
				AllocProp.cbAlign  = -1;  // -1 means no preference.
				AllocProp.cbBuffer = dwBytesPerSec *  dwLatencyInMilliseconds / 1000;
				AllocProp.cbPrefix = -1;
				AllocProp.cBuffers = -1;
				hr = pNeg->SuggestAllocatorProperties(&AllocProp);
				pNeg->Release();
			}
		}*/
	}
}