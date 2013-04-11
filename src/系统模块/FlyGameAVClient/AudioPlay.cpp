//NetTalk
/*------------------------------------------------------------------------------*\
 =============================
   模块名称: AudioPlay.cpp
 =============================
 
 [版权]
 
   2000-2002  cctvdodo.zj.com  版权所有
                                              
\*------------------------------------------------------------------------------*/
//doctor #include "WndX.h"
#include "stdafx.h"
#include <MMREG.H>

#include "WaveOut.h"
#include "AudioPlay.h"

/*------------------------------------------------------------------------------*/


CAudioPlay::CAudioPlay()
{

}
/*------------------------------------------------------------------------------*/
//播放一块数据
BOOL CAudioPlay::Play(char *buf, UINT uSize)
{

	BOOL bRet=FALSE;
	char* p;
	LPWAVEHDR pwh=new WAVEHDR;
	if(!pwh)
		goto RET;
	p=new char[uSize];//重新分配一块内存(在播放结束后删除)
	if(!p)
		goto RET;
	CopyMemory(p,buf,uSize);
	ZeroMemory(pwh,sizeof(WAVEHDR));
	pwh->dwBufferLength=uSize;
	pwh->lpData=p;
	//
	m_mmr=waveOutPrepareHeader(m_hOut,pwh,sizeof(WAVEHDR));
    if(m_mmr)
		goto RET;
	m_mmr=waveOutWrite(m_hOut,pwh,sizeof(WAVEHDR));
	if(m_mmr)
		goto RET;
	bRet=TRUE;
	
	
RET:
	return bRet;
}

/*------------------------------------------------------------------------------*/
//打开音频输出设备
BOOL CAudioPlay::Create(WAVEFORMATEX *pwf,DWORD dwCallBack,DWORD dwInst,DWORD fdwOpen )
{
	BOOL bRet=FALSE;
	if(m_hOut)
	{
		bRet=TRUE;
		goto RET;
	}
	//打开设备
	if(!OpenDev(pwf,dwCallBack,dwInst,fdwOpen))
		goto RET;
	bRet=TRUE;
RET:
	return bRet;
}

/*------------------------------------------------------------------------------*/

CAudioPlay::~CAudioPlay()
{
	Destroy();
}

/*------------------------------------------------------------------------------*/

BOOL CAudioPlay::Destroy()
{
	BOOL bRet=FALSE;
	if(!CloseDev())
		goto RET;
	bRet=TRUE;
RET:
	return bRet;

}
