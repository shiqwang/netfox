// CodecMgr.h: interface for the CCodecMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODECMGR_H__D2EAD0B0_B4FB_4AA5_9A8C_EFA286CBC007__INCLUDED_)
#define AFX_CODECMGR_H__D2EAD0B0_B4FB_4AA5_9A8C_EFA286CBC007__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vfw.h>
#include "../Include/FlyH323Interface.h"
#include "../Include/MapMsgItem.h"
#include "../Include/AMMsgType.h"
//#include "../Include/FlyEnumDefine.h"
#include "../Include/UDPSendMsgDefine.h"
#include "../Include/StructType.h"
//#include "TheFIle.h"
//#include "LayerIII.h"

//#include "../Include/UDPSendMsgDefine.h"

#define QCIF_WIDTH  176
#define QCIF_HEIGHT 144

#define IMAGE_WIDTH       QCIF_WIDTH
#define IMAGE_HEIGHT      QCIF_HEIGHT


#define YUV_SIZE IMAGE_WIDTH*IMAGE_HEIGHT+IMAGE_WIDTH*IMAGE_HEIGHT/2

class CFlyMsgManager ; 

class CCodecMgr  
{
public:
	int codeModeType;//编解码方式1=h263,2=mpeg4,3=h264
	BOOL InitMP4Compressor();
	void UnInitMP4Compressor();
	//压缩FRAME
//	void MP4CompressFrame(LPVIDEOHDR lpVHdr);
BOOL MP4CompressFrame(char* pin,int len,char* pout,int* lenr,BOOL* pKey);

	void FillBitmapStruct();
//	BITMAPINFO *m_pOutInfo;
	COMPVARS m_CV;
	DWORD m_OutFormatSize,m_OutBufferSize;
	long m_OutActSize;
	DWORD m_SampleNum;
	//mp3
		static double     m_preVol;
	static DWORD        DecCallback(double *pVal, double *pSpectrum,int nChannel);
//	CLayerIII decoder;//解压类
//	CTheFIle	m_pFile;
    DWORD		m_frmLength;
    DWORD       m_freq;
    DWORD       m_mode;
    DWORD       m_modeEx;
    DWORD       m_frqIdx;
    DWORD       m_version;
	BOOL		m_highProcSpeed;

public:
	HINSTANCE hDLL;
	CParam cparams;
	unsigned int yuv[ QCIF_WIDTH*QCIF_HEIGHT  + (QCIF_WIDTH*QCIF_HEIGHT)/2 ];

	void DestroyCodecA();
	BOOL InitCodecA();
	
	void DestroyCodecV();
	BOOL InitCodecV();
	virtual  ~CCodecMgr();
	CCodecMgr();
	
	friend class CFlyMsgManager;
protected:
	
	
	BITMAPINFO m_BmpU;
	BITMAPINFO m_BmpC;
//	COMPVARS m_cv;
public:

	BOOL EncodeVideoData(char* pin,int len,char* pout,int* lenr,BOOL* pKey);
	BOOL DecodeVideoData(char *pin,int len,char* pout,int *lenr,DWORD flag);
	BOOL EncodeAudioData(char *pin,int len,char* pout,int* lenr);
	BOOL DecodeAudioData(char *pin,int len,char* pout,int* lenr);

int isMP3Init;
BOOL DecodeMP3Data(BYTE *pin,DWORD len,BYTE * pout,DWORD * lenr);

//	HIC m_hIC;
};

#endif // !defined(AFX_CODECMGR_H__D2EAD0B0_B4FB_4AA5_9A8C_EFA286CBC007__INCLUDED_)
