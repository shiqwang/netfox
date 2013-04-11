#include "StdAfx.h"
#include ".\outmin.h"

COutMin::COutMin(void)
{
	m_pOutMyBaseFilter = NULL;
	m_pOutGraphBuilder = NULL;
}

COutMin::~COutMin(void)
{
	Release();
}

void COutMin::Init(void)
{
	Release();
	//½ÓÊÕ
	m_pOutGraphBuilder = new CGraphBuilder( CLSID_DSoundRender );
	if( m_pOutGraphBuilder )
	{
		IPin* InPin = m_pOutGraphBuilder->GetPin();

		m_pOutMyBaseFilter = new CMyBaseFilter();
		m_pOutMyBaseFilter->CreatePin<COutputPin>();
		m_pOutGraphBuilder->AddFilter( *m_pOutMyBaseFilter );
		IPin* OutPin = m_pOutMyBaseFilter->GetPin();

		m_pOutGraphBuilder->Connect( OutPin, InPin );
		m_pOutGraphBuilder->Run();
	}
}

void COutMin::OutSound( char* pData, int len )
{
	BOOL pass = FALSE;
	PBYTE pSampleBuffer = NULL;
	if( m_pOutMyBaseFilter )
	{
		pass = m_pOutMyBaseFilter->GetSampleBuffer(&pSampleBuffer);
		if( pass )
		{
			memcpy( pSampleBuffer, pData, len );
			m_pOutMyBaseFilter->DeliverHoldingSample( len );
		}
	}
}

void COutMin::Release(void)
{
	if( m_pOutMyBaseFilter && m_pOutGraphBuilder )
	{
		m_pOutGraphBuilder->Stop();
		m_pOutMyBaseFilter->Stop();
		m_pOutGraphBuilder->Disconnect( m_pOutMyBaseFilter->GetPin() );
		m_pOutGraphBuilder->RemoveFilter( *m_pOutMyBaseFilter );
	}

	if( m_pOutGraphBuilder )
	{
		delete m_pOutGraphBuilder;
		m_pOutGraphBuilder = NULL;
	}
	if( m_pOutMyBaseFilter )
	{
		delete m_pOutMyBaseFilter;
		m_pOutMyBaseFilter = NULL;
	}
}