#pragma once

#include "DShow.h"
#pragma comment( lib, "Strmiids.lib" )
//
class CGraphBuilder
{
private:
	IGraphBuilder* m_pGraphBuilder; //图表接口
	IMediaControl* m_pMediaControl; //控制图表控件
	IVideoWindow* m_pVideoWindow; //视频窗口
	IBaseFilter* m_pBaseFilter; //输出接口
public:
	CGraphBuilder(REFCLSID rclist);
	virtual ~CGraphBuilder(void);

	operator IGraphBuilder*( void ){ return m_pGraphBuilder; };
	IMediaControl* GetMediaControl(void){ return m_pMediaControl; };

	//初始化
	HRESULT Initialize( REFCLSID rclist );

	//释放
	void Release( void );

	//开始
	HRESULT Run( void );

	//暂停
	HRESULT Pause( void );

	//停止
	HRESULT Stop( void );

	//设置显示窗口
	HRESULT SetupVideoWindow( HWND hWnd=NULL );

	//显示
	HRESULT Visible( BOOL Visible=true );

	//取PIN
	IPin* GetPin(PIN_DIRECTION PinDirection = PINDIR_INPUT);

	//添加过滤器
	HRESULT AddFilter( IBaseFilter* pBaseFilter, LPCWSTR pName=L"" );
	HRESULT RemoveFilter( IBaseFilter* pBaseFilter );

	//添加
	HRESULT Connect( IPin* OutPin, IPin* InPin );
	HRESULT ConnectDirect( IPin* pOutPin, IPin* pInPin, AM_MEDIA_TYPE* pmt=0 );

	HRESULT Disconnect( IPin* Pin );

	//取文件
	HRESULT RenderFile( LPCWSTR lpFileName, LPCWSTR lpFilterName=NULL );
};
