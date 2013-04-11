#include "StdAfx.h"
#include ".\rinputpin.h"
#include "OutputPin.h"
#include "..\MyFile\VideoClient.h"
#include "..\MyFile\UserProcess.h"
#include "..\DirectShow\Compress\CConvertRgbYuv.h"

#include "..\zlib123\zlib.h"
#pragma comment(lib, "zlib.lib")

extern CVideoClient* g_pVideoClient;
extern void WriteLog(CString strFileName, CString strText);

CConvertRgbYuv m_ConvertRgbYuv;

BYTE YUVBuffer[YUV_SIZE];

CRInputPin::CRInputPin(CMyBaseFilter* pFilter, HRESULT* phr, LPCWSTR pName ) :
	CRenderedInputPin( NAME("RInputPin"), pFilter, *pFilter, phr, pName )
{
	m_pMyBaseFilter = pFilter;
}

CRInputPin::~CRInputPin(void)
{
	m_Connected=FALSE;
	m_cRef = 0;
}
//视频网络输出
STDMETHODIMP CRInputPin::Receive(IMediaSample *pSample)
{
	CString strFile,strTemp;
	CTime tmCur = CTime::GetCurrentTime();
	CString strTime = tmCur.Format("%m%d");
	strFile.Format("log\\%sVideo.log",strTime);

	strTemp.Format("CRInputPin::Receive");
	WriteLog(strFile, strTemp);


	CAutoLock  lck(&m_RInputPinLock);
	HRESULT hr = CRenderedInputPin::Receive(pSample);

	if ( hr==S_OK )
	{
		if( g_pVideoClient )
		{
			if (m_SampleProps.dwStreamId == AM_STREAM_MEDIA)
			{
				PBYTE pData;
				if( pSample->GetPointer(&pData) == S_OK )
				{
					int length = pSample->GetActualDataLength();

					//视频
					if( m_mt.formattype == FORMAT_VideoInfo )
					{
						//本地显示
						g_pVideoClient->OutVideo( INFINITE, (char*)pData, length );
						//
						m_ConvertRgbYuv.RGB24_YUV( IMAGE_WIDTH, IMAGE_HEIGHT, pData, YUVBuffer );
						char tmp[MAX_PATH];
						//封包
						strTemp.Format("视频封包 PACKET_USER_VEDIO_NEW");
						WriteLog(strFile, strTemp);

						int len = m_MyPacket.GetPacket( tmp, MAX_PATH, PACKET_USER_VEDIO_NEW, length );
						//
						g_pVideoClient->SendAllUser( tmp, len );
					}
					//语音
					else if( m_mt.formattype == FORMAT_WaveFormatEx )
					{
						int index=0;
						for( int i=0;i<length;i++ )
						{
							if( pData[i]>=0x60 && pData[i]<=0xA0 )
							{
								//pData[i]=0x7F;
								continue;
							}
							index++;
						}
						if( index>0x20 )
						{
							char AVData[6000];
							uLong OutLen  = 6000;
							int err = compress2( (Bytef *)AVData, &OutLen, pData, length, 9 );
							if (err == Z_OK)
							{
								g_pVideoClient->SaveSoundData( AVData, (int)OutLen );                                                   
							}
							/*
							if( g_pVideoClient )
							{
								g_pVideoClient->OutSound( (char*)pData, length );
							}*/
						}
					}
				}//End if( pSample->GetPointer(&pData) == S_OK )
			}//End if (m_SampleProps.dwStreamId == AM_STREAM_MEDIA)
		}//End if( g_pVideoClient )
	}//End if ( hr==S_OK )
	return S_OK;
}

STDMETHODIMP CRInputPin::EndOfStream(void)
{
	CAutoLock  lck(&m_RInputPinLock);
	return CRenderedInputPin::EndOfStream();
}

STDMETHODIMP CRInputPin::BeginFlush(void)
{
	CAutoLock  lck(&m_RInputPinLock);
	return CRenderedInputPin::BeginFlush();
}

STDMETHODIMP CRInputPin::EndFlush(void)
{
	CAutoLock  lck(&m_RInputPinLock);
	return CRenderedInputPin::EndFlush();
}

STDMETHODIMP CRInputPin::ReceiveCanBlock(void)
{
	return S_OK;
}

HRESULT CRInputPin::CheckMediaType(const CMediaType * inMediaType)
{
	if (inMediaType->formattype == FORMAT_VideoInfo || 
		inMediaType->formattype == FORMAT_WaveFormatEx )
	{
		return S_OK;
	}
	return E_FAIL;
}