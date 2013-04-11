//FlyIpCall
/*------------------------------------------------------------------------------*\
 =============================
   模块名称: AudioPlay.h
 =============================
 
 [版权]
 
   2000-2002  cctvdodo.zj.com  版权所有
                                              
\*------------------------------------------------------------------------------*/
#ifndef _AUDIOPLAY_H_
#define _AUDIOPLAY_H_

#include "WaveOut.h"


class CAudioPlay:public CWaveOut
{
public:
	
	CAudioPlay();
	virtual  ~CAudioPlay();

	BOOL Create(WAVEFORMATEX *pwf,DWORD dwCallBack,DWORD dwInst,DWORD fdwOpen );
	BOOL Play(char* buf,UINT uSize);
	BOOL Destroy();
	
protected:

};


#endif