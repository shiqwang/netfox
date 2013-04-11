#include "StdAfx.h"
#include ".\rinputpin.h"
#include "OutputPin.h"
#include "..\MyFile\VideoClient.h"
#include "..\MyFile\UserProcess.h"
#include "..\DirectShow\Compress\CConvertRgbYuv.h"

#include "..\zlib123\zlib.h"
#pragma comment(lib, "zlib.lib")

extern CVideoClient* g_pVideoClient;

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

STDMETHODIMP CRInputPin::Receive(IMediaSample *pSample)
{
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

					if( m_mt.formattype == FORMAT_VideoInfo )
					{
						g_pVideoClient->OutVideo( INFINITE, (char*)pData, length );
						m_ConvertRgbYuv.RGB24_YUV( IMAGE_WIDTH, IMAGE_HEIGHT, pData, YUVBuffer );
						char tmp[MAX_PATH];
						int len = m_MyPacket.GetPacket( tmp, MAX_PATH, PACKET_USER_VEDIO_NEW, length );
						g_pVideoClient->SendAllUser( tmp, len );
					}
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
				}
			}
		}
	}
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