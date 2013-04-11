//NetTalk
/*------------------------------------------------------------------------------*\
 =============================
   模块名称: AidopRec.cpp
 =============================
 
 [版权]
 
   2000-2002  cctvdodo.zj.com  版权所有
                                              
\*------------------------------------------------------------------------------*/
//doctor #include "WndX.h"
//#include "WaveShow.h"
//#include "UDPSocket.h"	
#include "stdafx.h"
#include "WaveIn.h"
#include "AudioRec.h"
#include "stdio.h"


/*------------------------------------------------------------------------------*/

CAudioRec::CAudioRec()
{
	

}
/*------------------------------------------------------------------------------*/
CAudioRec::~CAudioRec()
{
	Destroy();
}
/*------------------------------------------------------------------------------*/
//开始录音
BOOL CAudioRec::Start()
{
	BOOL bRet=FALSE;
	if(!m_hIn)
		goto RET;
	if(!AllocBuffer())
		goto RET;
	m_mmr=waveInStart(m_hIn);
	if(m_mmr)
		goto RET;	
	
	bRet=TRUE;
	
RET:

	return bRet;
}

/*------------------------------------------------------------------------------*/
//停止录音
BOOL CAudioRec::Stop()
{
	BOOL bRet=FALSE;
	
	if(!m_hIn)
		goto RET;
	
	m_mmr=waveInReset(m_hIn);
	if(m_mmr)
		goto RET;
	if(!FreeBuffer())
		goto RET;
	bRet=TRUE;
RET:

	return TRUE;
}

/*------------------------------------------------------------------------------*/
//为录音分配一组内存
BOOL CAudioRec::AllocBuffer()
{
	BOOL bRet=FALSE;
	UINT i;
	
	m_pHdr=new WAVEHDR[NUM_BUF];
	//为了使录音连续，采用多个缓冲区
	for(i=0;i<NUM_BUF;i++)
	{
		ZeroMemory(&m_pHdr[i],sizeof(WAVEHDR));
		m_pHdr[i].lpData=new char[m_nBufSize];
		m_pHdr[i].dwBufferLength=m_nBufSize;
		m_mmr=waveInPrepareHeader(m_hIn,&m_pHdr[i],sizeof(WAVEHDR));
		if(m_mmr)		
			goto RET;
		
		m_mmr=waveInAddBuffer(m_hIn,&m_pHdr[i],sizeof(WAVEHDR));
		if(m_mmr)
			goto RET;
		

	}
	
	bRet=TRUE;
RET:
	return bRet;
}

/*------------------------------------------------------------------------------*/
//释放内存
BOOL CAudioRec::FreeBuffer()
{
	BOOL bRet=FALSE;
	
	UINT i;
	if(!m_pHdr)
		goto RET;

	for(i=0;i<NUM_BUF;i++)
	{
		waveInUnprepareHeader(m_hIn,&m_pHdr[i],sizeof(WAVEHDR));
		if(m_pHdr[i].lpData)
			delete []m_pHdr[i].lpData;
		
	}
	delete []m_pHdr;
	bRet=TRUE;
RET:
	return bRet;

}

/*------------------------------------------------------------------------------*/
//打开录音设备
BOOL CAudioRec::Create(WAVEFORMATEX *pwf,DWORD dwCallBack,DWORD dwInst,DWORD fdwOpen ,DWORD dwBufSize)
{
	BOOL bRet=FALSE;
	if(m_hIn)
	{
		bRet=TRUE;
		goto RET;
	}
	if(!OpenDev(pwf,dwCallBack,dwInst,fdwOpen))
		goto RET;
	m_nBufSize=dwBufSize;
	bRet=TRUE;
RET:
	return bRet;
}

/*------------------------------------------------------------------------------*/
BOOL CAudioRec::Destroy()
{
	BOOL bRet=FALSE;
	if(!CloseDev())
		goto RET;
	bRet=TRUE;
RET:
	return bRet;
}
