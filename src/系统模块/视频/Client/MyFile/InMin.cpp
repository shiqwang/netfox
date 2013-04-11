#include "StdAfx.h"
#include ".\inmin.h"

CInMin::CInMin(void)
{
	m_pVideoInGraphBuilder = NULL;
	m_pVideoInMyBaseFilter = NULL;

	m_pAudioInGraphBuilder = NULL;
	m_pAudioInMyBaseFilter = NULL;
}

CInMin::~CInMin(void)
{
	Release();
}

void CInMin::Init( int AudioDeviceIndex, int VedioDeviceIndex )
{
	//音频
	if( m_AudioCaptureDevice.BindFilter( CLSID_AudioInputDeviceCategory, AudioDeviceIndex ) == S_OK )
	{
		m_pAudioInGraphBuilder = new CGraphBuilder( CLSID_DSoundRender );
		if( m_pAudioInGraphBuilder )
		{
			m_AudioCaptureDevice.SelectInput();
			m_AudioCaptureDevice.SetCaptureBufferSize();
			m_pAudioInGraphBuilder->AddFilter( m_AudioCaptureDevice );
			IPin* OutPin = m_AudioCaptureDevice.GetPin();

			//Filter
			m_pAudioInMyBaseFilter = new CMyBaseFilter();
			if( m_pAudioInMyBaseFilter )
			{
				CRInputPin* pRinputPin = m_pAudioInMyBaseFilter->CreatePin<CRInputPin>();
				m_pAudioInGraphBuilder->AddFilter( *m_pAudioInMyBaseFilter );
				IPin* InPin = m_pAudioInMyBaseFilter->GetPin();

				m_pAudioInGraphBuilder->Connect( OutPin, InPin );
				m_pAudioInGraphBuilder->Run();
			}
		}
	}

	//视频
	if( m_VideoCaptureDevice.BindFilter( CLSID_VideoInputDeviceCategory, VedioDeviceIndex ) == S_OK )
	{
		m_pVideoInGraphBuilder = new CGraphBuilder( CLSID_VideoRenderer );
		if( m_pVideoInGraphBuilder )
		{
			m_pVideoInGraphBuilder->AddFilter( m_VideoCaptureDevice );
			IPin* OutPin = m_VideoCaptureDevice.GetPin();

			//Filter
			m_pVideoInMyBaseFilter = new CMyBaseFilter();
            if( m_pVideoInMyBaseFilter )
			{
				CRInputPin* pRinputPin = m_pVideoInMyBaseFilter->CreatePin<CRInputPin>();
				m_pVideoInGraphBuilder->AddFilter( *m_pVideoInMyBaseFilter );
				IPin* InPin = m_pVideoInMyBaseFilter->GetPin();

				m_pVideoInGraphBuilder->Connect( OutPin, InPin );
				m_pVideoInGraphBuilder->Run();
			}
		}
	}
}

void CInMin::Release(void)
{
	//断开
	if( m_pAudioInMyBaseFilter && m_pAudioInGraphBuilder )
	{
		m_pAudioInGraphBuilder->Stop();
		m_pAudioInMyBaseFilter->Stop();
		m_pAudioInGraphBuilder->Disconnect( m_pAudioInMyBaseFilter->GetPin() );
		m_pAudioInGraphBuilder->RemoveFilter( *m_pAudioInMyBaseFilter );
	}

	//释放
	if( m_pAudioInGraphBuilder )
	{
		delete m_pAudioInGraphBuilder;
		m_pAudioInGraphBuilder = NULL;
	}

	if( m_pAudioInMyBaseFilter )
	{
		delete m_pAudioInMyBaseFilter;
		m_pAudioInMyBaseFilter = NULL;
	}

	//断开
	if( m_pVideoInMyBaseFilter && m_pVideoInGraphBuilder )
	{
		m_pVideoInGraphBuilder->Stop();
		m_pVideoInMyBaseFilter->Stop();
		m_pVideoInGraphBuilder->Disconnect( m_pVideoInMyBaseFilter->GetPin() );
		m_pVideoInGraphBuilder->RemoveFilter( *m_pVideoInMyBaseFilter );
	}

	//释放
	if( m_pVideoInGraphBuilder )
	{
		delete m_pVideoInGraphBuilder;
		m_pVideoInGraphBuilder = NULL;
	}

	if( m_pVideoInMyBaseFilter )
	{
		delete m_pVideoInMyBaseFilter;
		m_pVideoInMyBaseFilter = NULL;
	}
}