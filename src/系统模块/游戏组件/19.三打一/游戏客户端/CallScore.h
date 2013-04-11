#ifndef CALL_SCORE_HEAD_FILE
#define CALL_SCORE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

#define IDM_LAND_SCORE				(WM_USER+104)						//叫分消息

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CCallScore : public CWnd
{
	/*
	CSkinButton                    m_Score120;            //120分
	CSkinButton                    m_Score115;            //115分
	CSkinButton                    m_Score110;            //110分
	CSkinButton                    m_Score105;            //105分
	CSkinButton                    m_Score100;            //100分
	CSkinButton                    m_Score95;             //95分
	CSkinButton                    m_Score90;             //90分
	CSkinButton                    m_Score85;             //85分
	*/
	CSkinButton                    m_Score80;             //80分
	CSkinButton                    m_Score75;             //75分
	CSkinButton                    m_Score70;             //70分
	CSkinButton                    m_Score65;             //65分
	CSkinButton                    m_Score60;             //60分
	CSkinButton                    m_Score55;             //55分
	CSkinButton                    m_Score50;             //50分
	CSkinButton                    m_Score45;             //45分
	CSkinButton                    m_Score40;             //40分
	CSkinButton                    m_Score35;             //35分
	CSkinButton                    m_Score30;             //30分
	CSkinButton                    m_Score25;             //25分
	CSkinButton                    m_Score20;             //20分
	CSkinButton                    m_Score15;             //15分
	CSkinButton                    m_Score10;             //10分
	CSkinButton                    m_Score5;              //5分
	CSkinButton					   m_btGiveUpScore;		  //放弃

	CSkinImage					   m_ImageScoreBack;			  //背景图案

	//叫分信息
public:
	BYTE						   m_cbPriScore;				//上次叫分
	
	//函数定义
public:
	//构造函数
	CCallScore();

	//重画界面
	void ReloadView();

	//析构函数
	virtual ~CCallScore();

	
	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//叫分OK按钮
	afx_msg void OnCallScore80();
	//叫分OK按钮
	afx_msg void OnCallScore75();
	//叫分OK按钮
	afx_msg void OnCallScore70();
	//叫分OK按钮
	afx_msg void OnCallScore65();
	//叫分OK按钮
	afx_msg void OnCallScore60();
	//叫分OK按钮
	afx_msg void OnCallScore55();
	//叫分OK按钮
	afx_msg void OnCallScore50();
	//叫分OK按钮
	afx_msg void OnCallScore45();
	//叫分OK按钮
	afx_msg void OnCallScore40();
	//叫分OK按钮
	afx_msg void OnCallScore35();
	//叫分OK按钮
	afx_msg void OnCallScore30();
	//叫分OK按钮
	afx_msg void OnCallScore25();
	//叫分OK按钮
	afx_msg void OnCallScore20();
	//叫分OK按钮
	afx_msg void OnCallScore15();
	//叫分OK按钮
	afx_msg void OnCallScore10();
	//叫分OK按钮
	afx_msg void OnCallScore5();
	//放弃按钮
	afx_msg void OnGiveUpScore();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif