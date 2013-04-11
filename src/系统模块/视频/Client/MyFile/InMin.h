#pragma once

#include "..\DirectShow\GraphBuilder.h"
#include "..\DirectShow\MyBaseFilter.h"
#include "..\DirectShow\RInputPin.h"
#include "..\DirectShow\CaptureDevice.h"

class CInMin
{
private:
	CGraphBuilder* m_pVideoInGraphBuilder;
	CCaptureDevice m_VideoCaptureDevice;
	CMyBaseFilter* m_pVideoInMyBaseFilter;

	CGraphBuilder* m_pAudioInGraphBuilder;
	CCaptureDevice m_AudioCaptureDevice;
	CMyBaseFilter* m_pAudioInMyBaseFilter;
public:
	CInMin(void);
	~CInMin(void);

	void Init(int AudioDeviceIndex=0, int VedioDeviceIndex=0);
	void Release(void);
};
