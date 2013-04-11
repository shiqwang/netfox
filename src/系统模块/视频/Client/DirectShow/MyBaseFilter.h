#pragma once

#include <streams.h>

class CMyBaseFilter : public CBaseFilter
{
private:
	CBasePin* m_Pin;
	CCritSec m_FilterLock;
	IBaseFilter* m_pFilter;
	IMediaSample* m_Sample;
public:
	CMyBaseFilter( TCHAR* pName=_T(""), LPUNKNOWN lpunk=NULL );
	virtual ~CMyBaseFilter(void);

	operator CCritSec*( void ){ return &m_FilterLock; };
	operator IBaseFilter*( void ){ return m_pFilter; };

	virtual int GetPinCount();
	virtual CBasePin * GetPin(int n=0);
	STDMETHODIMP Stop();

	template <typename T>
	T* CreatePin( HRESULT* phr=NULL, LPCWSTR pName=L"" )
	{
		m_Pin = new T( this, phr, pName );
		m_Pin->AddRef();
        return (T*)m_Pin;
	}

	BOOL GetSampleBuffer(PBYTE * outBuffer);
	BOOL DeliverHoldingSample(long inSampleSize);
};
