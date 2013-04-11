// FlySoundText.h: interface for the CFlySoundText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLYSOUNDTEXT_H__34FA2725_4462_4F82_BAC3_9F87512CA746__INCLUDED_)
#define AFX_FLYSOUNDTEXT_H__34FA2725_4462_4F82_BAC3_9F87512CA746__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UtilitFun.h"

//场景播放管理类
class CFlySoundText  
{
public:
	CFlySoundText();
	virtual ~CFlySoundText();
//private:
	SCENE_ITEM  mySceneItemList[MAX_SCENE_NUM];
	int	useSceneInt;//使用的场景个数
	//加载系统配置
	void OnInitSysConfig();
	CUtilitFun myCUtilitFun;
public:
	//从INI文件读取数据，并保存到数据结构中
	void ReadDataFromIni(CString iniFileNameStr);

	//从INI文件读取一个场景数据，并保存到数据结构中
	void ReadOneSceneDataFromIni(CString iniFileNameStr, int sceneInt);

	//获得场景名称
	CString GetOneSceneName(int sceneInt);

	//获得场景数据
	bool GetOneSceneDataItem(int sceneInt,SCENE_ITEM * myItemPtr );

	//获得one sound数据
	bool GetOneSoundDataItem(int sceneInt,int soundInt,SOUND_TEXT_ITEM * myItemPtr );

};

#endif // !defined(AFX_FLYSOUNDTEXT_H__34FA2725_4462_4F82_BAC3_9F87512CA746__INCLUDED_)
