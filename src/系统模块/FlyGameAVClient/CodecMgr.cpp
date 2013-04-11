// CodecMgr.cpp: implementation of the CCodecMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlyGameAVClient.h"
#include "CodecMgr.h"
#include "g729a.h"
//#include "FlyMsgManager.h"

#include "x264\extras\stdint.h"
extern "C"
{
	#include "x264\x264.h"
}

extern "C"
{
	int Decode( char* Source, int size, char* OutData );
}
#include "DirectShow\Compress\CConvertRgbYuv.h"
CConvertRgbYuv m_ConvertRgbYuv;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#pragma comment(lib,"vfw32")

enum {H263_CODE=1, MP4_CODE, H264_CODE};
//#define MAX_UDP_SIZE   4096

//定义
typedef int (* pFlyH263Init)(CParam *cparams);
typedef void (*pFlyH263SetCallBackFunction)(void * myFun);

typedef int(* pFlyH263ConvertRGB2YUV)(int w,int h,unsigned char *rgbdata,unsigned int *yuv);
typedef int (* pFlyH263CompressFrame)(CParam *params, Bits *bits);

typedef int (* pFlyH263DecompressFrame)(unsigned char *cdata,int size,unsigned char *outdata,int outsize);

typedef void (* pFlyH263Exit) (CParam *cparams);


typedef int (* pFlyH263GetVersion) (char * getVersion);

// Global varialbes...
int count=0;
unsigned char cdata[10000];
int cbuffer_size=10000;
unsigned char rgbdata[80000];
int buffersize=80000;
double             CCodecMgr::m_preVol=20.0;

//Doctor DWORD CCodecMgr::DecCallback(double *pVal, double *pSpectrum,int nChannel);

/* * x264_malloc:
 ****************************************************************************/
void *x264_malloc( int i_size )
{
#ifdef SYS_MACOSX
    /* Mac OS X always returns 16 bytes aligned memory */
    return malloc( i_size );
#elif defined( HAVE_MALLOC_H )
    return memalign( 16, i_size );
#else
    uint8_t * buf;
    uint8_t * align_buf;
    buf = (uint8_t *) malloc( i_size + 15 + sizeof( void ** ) +
              sizeof( int ) );
    align_buf = buf + 15 + sizeof( void ** ) + sizeof( int );
    align_buf -= (long) align_buf & 15;
    *( (void **) ( align_buf - sizeof( void ** ) ) ) = buf;
    *( (int *) ( align_buf - sizeof( void ** ) - sizeof( int ) ) ) = i_size;
    return align_buf;
#endif
}

/****************************************************************************
 * x264_free:
 ****************************************************************************/
void x264_free( void *p )
{
    if( p )
    {
#if defined( HAVE_MALLOC_H ) || defined( SYS_MACOSX )
        free( p );
#else
        free( *( ( ( void **) p ) - 1 ) );
#endif
    }
}

int Encode( char* Source, void** OutData, int width, int height )
{
	x264_param_t param;
	x264_param_default( &param );
	param.i_width = IMAGE_WIDTH;
	param.i_height = IMAGE_HEIGHT;
	param.analyse.i_me_method = X264_ME_HEX;
	param.rc.i_rc_method = X264_RC_CQP;

	x264_t *x264 = NULL;
	if( ( x264 = x264_encoder_open( &param ) ) == NULL )
	{
		return 0;
	}

	x264_picture_t pic;
	x264_picture_alloc( &pic, X264_CSP_I420, width, height );
	
	memcpy( pic.img.plane[0], Source, width*height );
	memcpy( pic.img.plane[1], Source+width*height, width*height/4 );
	memcpy( pic.img.plane[2], Source+width*height+width*height/4, width*height/4 );

    x264_picture_t pic_out;
    x264_nal_t *nal;
    int i_nal;

	if( x264_encoder_encode( x264, &nal, &i_nal, &pic, &pic_out ) < 0 )
	{
		x264_picture_clean( &pic );
		x264_encoder_close( x264 );
		return 0;
	}

	uint8_t *mux_buffer = NULL;
	int mux_buffer_size = 0;

	int len = 0;

	for( int i = 0; i < i_nal; i++ )
	{
		len += nal[i].i_payload *3/2 + 4;
	}
	*OutData = malloc( len );
	char* p = (char*)*OutData;


	len=0;
	for( int i = 1; i < i_nal; i++ )
	{
        int i_size;

        if( mux_buffer_size < nal[i].i_payload * 3/2 + 4 )
        {
            mux_buffer_size = nal[i].i_payload * 2 + 4;
            x264_free( mux_buffer );
            mux_buffer = (uint8_t*)x264_malloc( mux_buffer_size );
        }

        i_size = mux_buffer_size;
        x264_nal_encode( mux_buffer, &i_size, 1, &nal[i] );
		memcpy( (char*)p+len, mux_buffer, i_size );
		len += i_size;
	}
/*
	mux_buffer_size = nal[i_nal-1].i_payload * 2 + 4;
	mux_buffer = (uint8_t*)x264_malloc( mux_buffer_size );
	len = mux_buffer_size;
	x264_nal_encode( mux_buffer, &len, 1, &nal[i_nal-1] );
           	
	*OutData = malloc( len );
	memcpy( *OutData, mux_buffer, len );
*/
	x264_free( mux_buffer );
	x264_picture_clean( &pic );
	x264_encoder_close( x264 );

	return len;
}

/*------------------------------------------------------------------------------*/
void OwnWriteFunction(int byte)
{
	if(count<cbuffer_size)
	cdata[count]=(unsigned char)byte;
	
	count++;

}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodecMgr::CCodecMgr()
{
	//编码方式，程序编译死。。。。。
	codeModeType = H264_CODE;//H263_CODE;//H263_CODE;//MP4_CODE;//MP4_CODE; //H263_CODE;//MP4_CODE; 
	//MP3
	isMP3Init = 0;
	m_preVol = 10;


	//VoiceAge g729a codec初始化
	va_g729a_init_encoder();
	va_g729a_init_decoder();
//	m_hIC=NULL;
	/*
	ZeroMemory(&m_cv,sizeof(m_cv));
	m_cv.cbSize=sizeof(m_cv);
	m_cv.dwFlags=ICMF_COMPVARS_VALID ;
	m_cv.hic=m_hIC;
	m_cv.fccType=ICTYPE_VIDEO ;
	m_cv.fccHandler=859189837;
	m_cv.lpbiOut=0;
	m_cv.lKey=10;
	m_cv.lDataRate=6;
	m_cv.lQ=10000;
	*/
//将视频格式固定（这是非常常用的格式，一般的视频捕捉设备应该会支持）
	ZeroMemory(&m_BmpU,sizeof(m_BmpU));

	m_BmpU.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_BmpU.bmiHeader.biWidth=QCIF_WIDTH;
	m_BmpU.bmiHeader.biHeight=QCIF_HEIGHT;
	m_BmpU.bmiHeader.biPlanes=1;	
	m_BmpU.bmiHeader.biBitCount=24;	
	m_BmpU.bmiHeader.biSizeImage=76032;

	CString dllNameStr="FlyH263.dll";
	CString showMsg;
		//初始化
	hDLL=LoadLibrary(dllNameStr);//加载动态链接库MyDll.dll文件；
	if (!hDLL)
	{
		showMsg.Format("加载文件<%s>失败!", dllNameStr);
		AfxMessageBox(showMsg);
	}



}

CCodecMgr::~CCodecMgr()
{
	DestroyCodecA();
	DestroyCodecV();

	FreeLibrary(hDLL);
}
//解压缩视频帧
BOOL CCodecMgr::DecodeVideoData(char *pin,int len,char* pout,int *lenr,DWORD flag)
{
		CString strFile,strTemp;
		CTime tmCur = CTime::GetCurrentTime();
		CString strTime = tmCur.Format("%m%d");
		strFile.Format("log\\%sDecodeVideoData.log",strTime);

	
	BOOL bRet=FALSE;

	switch(codeModeType)
	{
	case H264_CODE:
{

			try 
			{

				BYTE m_YUVBuffer[YUV_SIZE];

				int outSize =	Decode( pin, len, (char *)m_YUVBuffer );

				strTemp.Format("Decode=%d, %d", len,  outSize);	
				//写入日志
				theApp.WriteLog(strFile, strTemp);


				m_ConvertRgbYuv.YUV_RGB24( m_YUVBuffer, (BYTE*)rgbdata, IMAGE_WIDTH, IMAGE_HEIGHT );

				CopyMemory(pout, rgbdata, m_BmpU.bmiHeader.biSizeImage);

				strTemp.Format("out=, %d", m_BmpU.bmiHeader.biSizeImage);	
				//写入日志
				theApp.WriteLog(strFile, strTemp);

				if(lenr)
				*lenr=m_BmpU.bmiHeader.biSizeImage;

				
				bRet=TRUE;
			}
			catch(...)
			{
				goto RET;
			}
			
		}
		break;
		break;
	case H263_CODE:
		{

			try 
			{
				int retvalue;
				pFlyH263DecompressFrame DecompressFrame;
				DecompressFrame=(pFlyH263DecompressFrame)GetProcAddress(hDLL,"FlyH263DecompressFrame");
			//
			//	unsigned char *data;
				int size;
			//	data = (unsigned char *)pin;
				size = len;
				//输入char *pin,int len，输出char* pout,int *lenr
			  //  retvalue=DecompressFrame( data, size, rgbdata, buffersize);
				retvalue=DecompressFrame( (unsigned char *)pin, size, rgbdata, buffersize);

			//	pout = (char* )rgbdata; 拷贝！！！！！！
			//	*lenr = retvalue;  返回值
				CopyMemory(pout, rgbdata, m_BmpU.bmiHeader.biSizeImage);

				if(lenr)
					*lenr=m_BmpU.bmiHeader.biSizeImage;

				
				bRet=TRUE;
			}
			catch(...)
			{
				goto RET;
			}
			
		}
		break;

	case MP4_CODE:
		{
			try
			{
				if(!pin||!pout||!m_CV.hic)
					goto RET;
				//
				if(ICDecompress( m_CV.hic,flag,&m_BmpC.bmiHeader,pin,&m_BmpU.bmiHeader,pout)!=ICERR_OK)
					goto RET;
				
				if(lenr)
					*lenr=m_BmpU.bmiHeader.biSizeImage;
				
				bRet=TRUE;
			}
			catch(...)
			{
					goto RET;
			}


		}
		break;
	}
RET:
	return bRet;	
}

/*------------------------------------------------------------------------------*/
//压缩视频帧
BOOL CCodecMgr::EncodeVideoData(char* pin,int len,char* pout,int* lenr,BOOL* pKey)
{
		CString strFile,strTemp;
		CTime tmCur = CTime::GetCurrentTime();
		CString strTime = tmCur.Format("%m%d");
		strFile.Format("log\\%sEncodeVideoData.log",strTime);

	BOOL bRet=FALSE;
				
	switch(codeModeType)
	{
	case H264_CODE:
		{
				try
				{
					BYTE YUV[YUV_SIZE];
					m_ConvertRgbYuv.RGB24_YUV( IMAGE_WIDTH, IMAGE_HEIGHT,(unsigned char *) pin,(unsigned char *) YUV );
					char* OutData = NULL;
					int OutLen =  Encode( (char*)YUV, (void **)&OutData, IMAGE_WIDTH, IMAGE_HEIGHT );
					if( OutLen>0 && OutLen< 7168 )
					{
						*lenr = OutLen;

						//拷贝数据	
						CopyMemory(pout, OutData, OutLen);

						strTemp.Format("H264_CODE=%d)", OutLen);
						theApp.WriteLog(strFile, strTemp);
					}
					if( OutData )
						free( OutData );

					bRet=TRUE;		
		
		
		}
				catch (...)
				{
				
					AfxMessageBox("视频编码异常!!!");
				}	
		}
		break;
	case H263_CODE:
		{
				pFlyH263ConvertRGB2YUV ConvertRGB2YUV;
				pFlyH263CompressFrame  CompressFrame;


				ConvertRGB2YUV=(pFlyH263ConvertRGB2YUV)GetProcAddress(hDLL,"FlyH263ConvertRGB2YUV");
				CompressFrame = (pFlyH263CompressFrame)GetProcAddress(hDLL,"FlyH263CompressFrame");

				

				char*p;
				long s=2000;
				BOOL k;

				try
				{
					/*
					if(!pin||!pout||len!=(int)m_BmpU.bmiHeader.biSizeImage||!m_hIC)
					goto RET;
					*/

					//doctor	p=(char*)ICSeqCompressFrame(&m_cv,0,pin,&k,&s);
					//输入char* pin,int len，输出 char* pout,int* lenr
					BYTE *data;
					Bits bits;		// Various count about compression


					data = (BYTE *)pin;

					if(data==NULL)
					return bRet;
					ConvertRGB2YUV(IMAGE_WIDTH,IMAGE_HEIGHT,data,yuv);


					// Reset the counter
					count=0;

					//Compress the data...to h263
					cparams.format=CPARAM_QCIF;
					cparams.inter = CPARAM_INTRA;  
					cparams.Q_intra = 8;
					cparams.data=yuv;		//  Data in YUV format...
					CompressFrame(&cparams, &bits);

					//			dvideo.SendVideoData(cdata,count);
					p=(char*)cdata;
					s=count;

					if(!p)
					goto RET;
					if(lenr)
					*lenr=s;
					if(pKey)
					*pKey=k;
					if(s>MAX_UDP_SIZE-sizeof(PACK_VIDEO))
					goto RET;

					//拷贝数据	
					CopyMemory(pout,p,s);


					bRet=TRUE;
				}
				catch (...)
				{
				
					AfxMessageBox("视频编码异常!!!");
				}		
		
		}
		break;
	case MP4_CODE:
		{
	
			try
			{
				char*p;
				long s=2000;
				BOOL k;
				if(!pin||!pout||len!=(int)m_BmpU.bmiHeader.biSizeImage||!m_CV.hic)
					goto RET;
				
				p=(char*)ICSeqCompressFrame(&m_CV,0,pin,&k,&s);
				if(!p)
					goto RET;
				if(lenr)
					*lenr=s;
				if(pKey)
					*pKey=k;
				if(s>MAX_UDP_SIZE-sizeof(PACK_VIDEO))
					goto RET;
				
				CopyMemory(pout,p,s);
				bRet=TRUE;
			
			}
			catch(...)
			{
				goto RET; 
			
			}
		
		
		}
		break;
	}//End 	switch(codeModeType)

RET:
	return bRet;	
}


/*------------------------------------------------------------------------------*/
//压缩音频帧
BOOL CCodecMgr::EncodeAudioData(char *pin,int len,char* pout,int* lenr)
{
		CString strFile,strTemp;
		CTime tmCur = CTime::GetCurrentTime();
		CString strTime = tmCur.Format("%m%d");
		strFile.Format("log\\%sEncodeAudioData.log",strTime);

	BOOL bRet=FALSE;
	if(!pin||len!=SIZE_AUDIO_FRAME||!pout)
		goto RET;
	//encode the audio frames
	va_g729a_encoder((short*)pin,(BYTE*)pout);
	va_g729a_encoder((short*)(pin+160),(BYTE*)pout+10);
	va_g729a_encoder((short*)(pin+320),(BYTE*)pout+20);
	va_g729a_encoder((short*)(pin+480),(BYTE*)pout+30);
	va_g729a_encoder((short*)(pin+640),(BYTE*)pout+40);
	va_g729a_encoder((short*)(pin+800),(BYTE*)pout+50);

	if(lenr)
		*lenr=SIZE_AUDIO_PACKED;


		strTemp.Format("into EncodeAudioData");
		theApp.WriteLog(strFile, strTemp);


	bRet=TRUE;
RET:
	return bRet;
}

//MP3 解码
BOOL CCodecMgr::DecodeMP3Data(BYTE *pin,DWORD len,BYTE * pout,DWORD * lenr)
{

	return false;	
}

/*------------------------------------------------------------------------------*/
//解压缩音频帧
BOOL CCodecMgr::DecodeAudioData(char *pin,int len,char* pout,int* lenr)
{
	
	BOOL bRet=FALSE;
	if(!pin||len!=SIZE_AUDIO_PACKED||!pout)
		goto RET;
	//decode the audio frames
	va_g729a_decoder((BYTE*)pin,(short*)(pout),0);
	va_g729a_decoder((BYTE*)pin+10,(short*)(pout+160),0);
	va_g729a_decoder((BYTE*)pin+20,(short*)(pout+320),0);
	va_g729a_decoder((BYTE*)pin+30,(short*)(pout+480),0);
	va_g729a_decoder((BYTE*)pin+40,(short*)(pout+640),0);
	va_g729a_decoder((BYTE*)pin+50,(short*)(pout+800),0);

	if(lenr)
		*lenr=SIZE_AUDIO_FRAME;
	
	bRet=TRUE;
RET:
	return bRet;	
}
//初始化编码器
BOOL CCodecMgr::InitMP4Compressor()
{
		BOOL bRet=FALSE;
////////Let the user select the compressor and fill the COMPVRAS Struct automatically
/*	
	m_CV.cbSize=sizeof(m_CV);	
    ICCompressorChoose(this->m_hWnd,0,NULL,NULL,&m_CV,"Choose a Compressor");
	afxDump<<m_CV.lKeyCount<<"\n";
*/	
////////Fill the COMPVRAS Struct manually
	
    memset(&m_CV,0,sizeof(COMPVARS));
	m_CV.dwFlags=ICMF_COMPVARS_VALID;
	m_CV.cbSize=sizeof(m_CV);
	m_CV.cbState=0;
	m_CV.fccHandler=mmioFOURCC('d','i','v','x');

//	m_CV.fccHandler=mmioFOURCC('x','v','i','d');
	m_CV.fccType=ICTYPE_VIDEO;
//	m_CV.hic=ICOpen(ICTYPE_VIDEO,mmioFOURCC('d','i','v','x'),ICMODE_COMPRESS);
//	m_CV.hic=ICOpen(ICTYPE_VIDEO,mmioFOURCC('x','v','i','d'),ICMODE_COMPRESS);
//	m_CV.hic=ICOpen(ICTYPE_VIDEO,mmioFOURCC('x','v','i','d'),ICMODE_COMPRESS | ICMODE_DECOMPRESS);
//	m_CV.hic=ICOpen(ICTYPE_VIDEO,mmioFOURCC('d','i','v','x'),ICMODE_COMPRESS | ICMODE_DECOMPRESS);

	m_CV.hic=ICOpen(ICTYPE_VIDEO,m_CV.fccHandler /*mmioFOURCC('d','i','v','x')*/,ICMODE_COMPRESS | ICMODE_DECOMPRESS);


	m_CV.lDataRate=100;//780;// kb/s  
//	AfxMessageBox("10");
  
	m_CV.lFrame=0;
	m_CV.lKey=0;//15;
	m_CV.lKeyCount=0;
	m_CV.lpbiIn=NULL;
	m_CV.lpBitsOut=NULL;
	m_CV.lpBitsPrev=m_CV.lpState=NULL;
	m_CV.lQ= 100;//ICQUALITY_DEFAULT;
	
	if (m_CV.hic!=NULL)
	{
		m_OutFormatSize=ICCompressGetFormatSize(m_CV.hic,&m_BmpU);
//	    m_pOutInfo=(BITMAPINFO *)new BYTE[m_OutFormatSize];
	    ICCompressGetFormat(m_CV.hic,&m_BmpU,&m_BmpC);

		m_OutBufferSize=ICCompressGetSize(m_CV.hic,&m_BmpU,&m_BmpC);
		
		ICSeqCompressFrameStart(&m_CV,&m_BmpU);

	//star sequence of frames compression
	if(!ICSeqCompressFrameStart(&m_CV,&m_BmpU))
		goto RET;
	//star decompression;
	if(ICDecompressBegin(m_CV.hic,&m_BmpC,&m_BmpU)!=ICERR_OK)
		goto RET;

		return TRUE;
	}
	else
	{
		AfxMessageBox("初始化编码器失败!请先安装编码器");

	//运行编码器安装程序
		//启动应用程序
		CString  fileName, gamePath, foldName, sPath;
//		gamePath = applicationPath + "\\MPEG4";
		fileName = "FlyMpeg4.exe";
		
		ShellExecute(NULL,NULL,_T(fileName),NULL, gamePath,NULL); 

		return FALSE;
	}



RET:
	AfxMessageBox("初始化divx编码器失败!");
	return bRet;
}
/*------------------------------------------------------------------------------*/
//初始化视频编码解码器
BOOL CCodecMgr::InitCodecV()
{
	BOOL bRet=FALSE;

	switch(codeModeType)
	{
	case H263_CODE:
		{
			pFlyH263Init  H263Init;
			pFlyH263SetCallBackFunction  FlyH263SetCallBackFunction;


			H263Init=(pFlyH263Init)GetProcAddress(hDLL,"FlyH263Init");
			FlyH263SetCallBackFunction=(pFlyH263SetCallBackFunction)GetProcAddress(hDLL,"FlyH263SetCallBackFunction");

			cparams.format = CPARAM_QCIF;
			H263Init(&cparams);
			FlyH263SetCallBackFunction(OwnWriteFunction);

			bRet=TRUE;
		}

		break;
	case MP4_CODE:
		{
//			FillBitmapStruct();
			bRet = InitMP4Compressor();
		}
		break;
	case H264_CODE:
		{
					bRet=TRUE;
		}
		break;

	}	

	/*
	DestroyCodecV();
	BOOL bRet=FALSE;
	//
	m_hIC=ICOpen(ICTYPE_VIDEO,m_cv.fccHandler,ICMODE_COMPRESS|ICMODE_DECOMPRESS);
	if(!m_hIC)
		goto RET;
	
	ICCompressGetFormat(m_hIC,&m_BmpU,&m_BmpC);
	//微软的H.263 codec需要消息确认，得到这个不容易
	ICSendMessage(m_hIC,0x60c9,0xf7329ace,0xacdeaea2);

	m_cv.hic=m_hIC;
	m_cv.dwFlags=ICMF_COMPVARS_VALID;
	//star sequence of frames compression
	if(!ICSeqCompressFrameStart(&m_cv,&m_BmpU))
		goto RET;
	//star decompression;
	if(ICDecompressBegin(m_hIC,&m_BmpC,&m_BmpU)!=ICERR_OK)
		goto RET;
*/

	
RET:
	if(!bRet)
		DestroyCodecV();
	return bRet;
}
//关闭编码器
void CCodecMgr::UnInitMP4Compressor()
{
	if (m_CV.hic!=NULL)
	{
//		delete[] m_pOutInfo;
		::ICSeqCompressFrameEnd(&m_CV);
		ICCompressorFree(&m_CV);
		ICClose(m_CV.hic);
	}

}
/*------------------------------------------------------------------------------*/
void CCodecMgr::DestroyCodecV()
{	
	/*
	if(m_hIC)
	{
		ICDecompressEnd(m_hIC);
		ICSeqCompressFrameEnd(&m_cv);
		ICCompressorFree(&m_cv);
		ICClose(m_hIC);
		m_hIC=NULL;
	}	
	*/
	switch(codeModeType)
	{
	case H264_CODE:
		{}
		break;
	case H263_CODE:
		{}
		break;
	case MP4_CODE:
		{
		UnInitMP4Compressor();
		
		}
		break;
	}
	
}
/*------------------------------------------------------------------------------*/
BOOL CCodecMgr::InitCodecA()
{	
	return TRUE;
}
/*------------------------------------------------------------------------------*/
void CCodecMgr::DestroyCodecA()
{
	

}

//回调函数,设置不同的进度条
DWORD CCodecMgr::DecCallback(double *pData,double *pSpectrum, 
                                int nChannel)
{
    static int c=0;
    WORD w = 0;

    if(pData) // volum control preamp
    {
	    *pData*=m_preVol/20.0;
 
        w = (* pData > 32767.0f) ? 32767 :    ((* pData< -32768.0f) ? -32768 :
		                                           (WORD) * pData) ;
    }

	// spectrum forward to graph wnd
	/*
    if(nChannel==0) 
    {
        CRichiMP3Dlg::m_scope->AddSample(w-32767);
//		CMcPlayerDlg *pDlg = (CMcPlayerDlg*)GetParent()->m_scope->AddSample(w-32767);
		int v;
		CRichiMP3Dlg::m_ctrl1->SetPick(v = rand()%99 );
		CRichiMP3Dlg::m_ctrl2->SetPick(v = rand()%99 );
    }
	*/
    
    return w;
}
