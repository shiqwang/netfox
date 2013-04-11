#pragma once

#include "DShow.h"
#pragma comment( lib, "Strmiids.lib" )

typedef HRESULT (CALLBACK *ENUMCALLBACK)( int index, VARIANT* var/*返回值*/, IMoniker *pM/*设备*/, LPVOID lp/*参数*/, BOOL& bCancel/*是否退出循环*/ );
typedef HRESULT (CALLBACK *ENUMINPUTCALLBACK)( const char* pName/*返回值*/, IPin * pin /*设备*/, LPVOID lp/*参数*/, BOOL& bCancel/*是否退出循环*/ );

//截取类
class CCaptureDevice
{
private:
	IBaseFilter* m_pBaseFilter; //Filter接口 用来选择设备
public:
	CCaptureDevice(void);
	virtual ~CCaptureDevice(void);

	operator IBaseFilter*( void ){ return m_pBaseFilter; };

	//枚举设备 参数1：设备ID 例：CLSID_VideoInputDeviceCategory //按F12跳到定义
	HRESULT EnumDevices( REFCLSID clsidDeviceClass, ENUMCALLBACK pEnumCallBack, LPVOID lp );

	//释放
	void Release( void );

	//帮定设备
	HRESULT BindFilter( REFCLSID clsidDeviceClass/*设备类型ID*/, int DeviceIndex/*设备索引序号,按枚举顺序得到*/ );

	//取设备输出Pin
	IPin* GetPin(void);

	//格式
	void AdjustOutput(void);
	AM_MEDIA_TYPE * SelectMediaType(void);
	IAMStreamConfig * GetStreamConfig(void);
	void EnumInputList( ENUMINPUTCALLBACK EnumInputCallBack, LPVOID lp );
	void SelectInput( char* pName = "麦克风" );
	void SetCaptureBufferSize(void);
};
