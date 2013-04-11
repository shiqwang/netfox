#include "StdAfx.h"
#include ".\mybasefilter.h"
#include <initguid.h>

DEFINE_GUID(CLSID_MyBaseFilter, 
0xa7f18a36, 0x2899, 0x4546, 0x8d, 0x88, 0x4d, 0x21, 0xd4, 0xf2, 0xda, 0x4d);

//°²È«ÊÍ·Å
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

CMyBaseFilter::CMyBaseFilter( TCHAR* pName, LPUNKNOWN lpunk ) :
	CBaseFilter(pName, lpunk, &m_FilterLock, CLSID_MyBaseFilter)
{
	m_Pin = NULL;
	m_pFilter = NULL;
	m_Sample = NULL;
	QueryInterface(IID_IBaseFilter, (void**)&m_pFilter);
}

CMyBaseFilter::~CMyBaseFilter(void)
{
	SAFE_RELEASE( m_Sample );
	if( m_Pin )
	{
		m_Pin->Release();
		delete m_Pin;
		m_Pin = NULL;
	}
}

int CMyBaseFilter::GetPinCount()
{
	return 1;
}

CBasePin * CMyBaseFilter::GetPin(int n)
{
	if (n == 0) 
	{
		return m_Pin;
	}
	else 
	{
		return NULL;
	}
}

STDMETHODIMP CMyBaseFilter::Stop()
{
	CAutoLock lck(&m_FilterLock);
	if (m_Sample)
	{
		m_Sample->Release();
		m_Sample = NULL;
	}

	return CBaseFilter::Stop();
}

BOOL CMyBaseFilter::GetSampleBuffer(PBYTE * outBuffer)
{
	if (m_Sample)
	{
		m_Sample->Release();
		m_Sample = NULL;
	}

	HRESULT hr = ((CBaseOutputPin*)m_Pin)->GetDeliveryBuffer(&m_Sample,NULL,NULL,0);
	if (m_Sample)
	{
		m_Sample->GetPointer(outBuffer);
	}
	return hr==S_OK;
}

BOOL CMyBaseFilter::DeliverHoldingSample(long inSampleSize)
{
	if (m_Sample)
	{
		m_Sample->SetActualDataLength(inSampleSize);
		m_Sample->SetSyncPoint(TRUE);

//		if (mIsVideo)
//		{
//			// Do not use time stamp for video
//			mSample->SetTime(NULL, NULL);
//		}
//		else
		{
			REFERENCE_TIME rtStart = 0;
			//REFERENCE_TIME mLastSampleTime += (UNITS * inSampleSize / mAudioBytesPerSecond);
			REFERENCE_TIME rtEnd   = (UNITS * inSampleSize / 11025);
			m_Sample->SetTime(&rtStart, &rtEnd);
		}

		HRESULT hr = ((CBaseOutputPin*)m_Pin)->Deliver(m_Sample);
		if( m_Sample )
		{
			m_Sample->Release();
			m_Sample = NULL;
		}
		return hr==S_OK;
	}
	return TRUE;
}