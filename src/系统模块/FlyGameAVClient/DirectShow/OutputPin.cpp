#include "StdAfx.h"
#include ".\outputpin.h"

COutputPin::COutputPin(CMyBaseFilter* pFilter, HRESULT* phr, LPCWSTR pName) :
	CBaseOutputPin(NAME("OutputPin"), pFilter, *pFilter, phr, pName)
{
	m_PreferredMt.InitMediaType();

/*	m_PreferredMt.SetType(&MEDIATYPE_Video);
	m_PreferredMt.SetSubtype(&MEDIASUBTYPE_RGB24);
	m_PreferredMt.SetFormatType(&FORMAT_VideoInfo);
	m_PreferredMt.SetTemporalCompression(FALSE);
	VIDEOINFOHEADER     info;
	ZeroMemory(&info, sizeof(VIDEOINFOHEADER));
	info.AvgTimePerFrame = 400000;
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth  = 160;
	info.bmiHeader.biHeight = 120;
	info.bmiHeader.biBitCount  = 24;
	info.bmiHeader.biSizeImage = 160 * 120 * 24 / 8;
	info.bmiHeader.biCompression = 0;
	info.bmiHeader.biPlanes = 1;
	m_PreferredMt.SetFormat((BYTE*)&info, sizeof(VIDEOINFOHEADER));
	m_PreferredMt.SetSampleSize(info.bmiHeader.biSizeImage);*/

	m_PreferredMt.SetType(&MEDIATYPE_Audio);
	m_PreferredMt.SetSubtype(&MEDIASUBTYPE_PCM);
	m_PreferredMt.SetFormatType(&FORMAT_WaveFormatEx);
	m_PreferredMt.SetTemporalCompression(FALSE);
	WAVEFORMATEX info;
//	ZeroMemory(&info, sizeof(WAVEFORMATEX));
	info.cbSize = 0;
	info.wFormatTag = WAVE_FORMAT_PCM;
	info.nChannels = 1;
	//info.nSamplesPerSec = 44100;
	//info.nSamplesPerSec = 22050;
	info.nSamplesPerSec = 11025;
	info.wBitsPerSample = 8;
	info.nBlockAlign = 1;
	info.nAvgBytesPerSec = info.nSamplesPerSec * info.nBlockAlign * info.wBitsPerSample / 8;
	
	m_PreferredMt.SetFormat((BYTE*)&info, sizeof(WAVEFORMATEX));
}

COutputPin::~COutputPin(void)
{
}

void COutputPin::SetupMediaType( CMediaType mPreferredMt )
{
    m_PreferredMt = mPreferredMt;
	m_PreferredMt.SetTemporalCompression(FALSE);
}

HRESULT COutputPin::CheckMediaType(const CMediaType * inMediaType)
{
	if (*inMediaType == m_PreferredMt)
	{
		return NOERROR;
	}
	return E_FAIL;
}

HRESULT COutputPin::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pprop)
{
	ASSERT(pAlloc);
	ASSERT(pprop);
	HRESULT hr = NOERROR;

	// Determine sample size
	if (m_PreferredMt.formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * info = (VIDEOINFOHEADER *) m_PreferredMt.pbFormat;
		pprop->cbBuffer = info->bmiHeader.biSizeImage;
	}
	else if (m_PreferredMt.formattype == FORMAT_WaveFormatEx)
	{
		WAVEFORMATEX * info = (WAVEFORMATEX *) m_PreferredMt.pbFormat;
		pprop->cbBuffer = info->nAvgBytesPerSec;
	}
	else
		return S_FALSE;
	pprop->cBuffers  = 1;
	pprop->cbAlign   = 1;

	ASSERT(pprop->cbBuffer);

	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pprop, &Actual);
	if (hr!=S_OK) 
	{
		return hr;
	}

	ASSERT( Actual.cBuffers == 1 );

	if (pprop->cBuffers > Actual.cBuffers ||
		pprop->cbBuffer > Actual.cbBuffer) 
	{
		return E_FAIL;
	}
	return NOERROR;
}

HRESULT COutputPin::GetMediaType(int iPosition,CMediaType *pMediaType)
{
	if (iPosition == 0)
	{
		*pMediaType = m_PreferredMt;
		return NOERROR;
	}
	return E_INVALIDARG;
}