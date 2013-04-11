#pragma once

#include "MyBaseFilter.h"
#include <streams.h>
//
class CRInputPin : public CRenderedInputPin
{
private:
	CMyBaseFilter* m_pMyBaseFilter;
	CCritSec m_RInputPinLock;
public:
	CRInputPin(CMyBaseFilter* pFilter, HRESULT* phr, LPCWSTR pName);
	virtual ~CRInputPin(void);

	STDMETHODIMP Receive(IMediaSample *pSample);
	STDMETHODIMP EndOfStream(void);
	STDMETHODIMP BeginFlush(void);
	STDMETHODIMP EndFlush(void);
	STDMETHODIMP ReceiveCanBlock(void);
	HRESULT CheckMediaType(const CMediaType * inMediaType);
};
