
#pragma once

#include "Interface.h"

//游戏等级管理
class CGameRankManager : public IGameRankManager
{
	//函数定义
public:
	CGameRankManager(void);
	virtual ~CGameRankManager(void);

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CGameRankManager))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//组件接口
public:
	//获取等级
	virtual LPCTSTR __cdecl GetGameRank(WORD wKindID, LONG lScore, WORD wGameGenre);
	static LPCTSTR __cdecl GetCurrencyRank(LONG lScore);
	static LPCTSTR __cdecl GetShowHandRank(LONG lScore);
	static LPCTSTR __cdecl GetSanDaHaRank(LONG lScore);
	static LPCTSTR __cdecl GetUncoverPigRank(LONG lScore);
	static LPCTSTR __cdecl GetUpgradeRank(LONG lScore);
	static LPCTSTR __cdecl GetSDragonBallRank(LONG lScore);
	static LPCTSTR __cdecl GetCavernRank(LONG lScore);
	static LPCTSTR __cdecl GetHoeRank(LONG lScore);
	static LPCTSTR __cdecl GetCChessRank(LONG lScore);
	static LPCTSTR __cdecl GetSeZhongRank(LONG lScore);	
	static LPCTSTR __cdecl GetFourEnsignRank(LONG lScore);
	static LPCTSTR __cdecl GetRunFastRank(LONG lScore);

	static LPCTSTR __cdecl GetDoShouQiRank(LONG lScore);
	static LPCTSTR __cdecl GetGoRank(LONG lScore);
	static LPCTSTR __cdecl GetLanddRank(LONG lScore);
	static LPCTSTR __cdecl GetSparrowCSRank(LONG lScore);
	static LPCTSTR __cdecl GetTFZGameRank(LONG lScore);

	static LPCTSTR __cdecl GetDefaultRank(LONG lScore);
};


//-----------------------------------------------
//					the end
//-----------------------------------------------
