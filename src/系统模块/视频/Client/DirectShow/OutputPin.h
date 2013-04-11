#pragma once

#include "MyBaseFilter.h"
#include <streams.h>

class COutputPin :
	public CBaseOutputPin
{
public:
	CMediaType m_PreferredMt;
	CCritSec m_OutputPinLock;
public:
	COutputPin(CMyBaseFilter* pFilter, HRESULT* phr=NULL, LPCWSTR pName=L"");
	~COutputPin(void);

	void SetupMediaType( CMediaType mPreferredMt );
	virtual HRESULT CheckMediaType(const CMediaType *);
	virtual HRESULT DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pprop);
	// returns the preferred formats for a pin
	HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);
};
