#if !defined(AFX_FLYGAMESOUND_H__996BD949_71F8_4971_B5C6_5A0CC87A15DE__INCLUDED_)
#define AFX_FLYGAMESOUND_H__996BD949_71F8_4971_B5C6_5A0CC87A15DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlyGameSound.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlyGameSound window
//声音效果类
#include <mmsystem.h>//播放声音的头文件

class CFlyGameSound : public CWnd
{
// Construction
private:
		DWORD cdfrom,cdto;
	    DWORD cdlen;
		DWORD m_count;
		DWORD getinfo(DWORD item);
public:
	//播放MP3声音文件
	void PlayMP3Sound(CString fileStr);
public:
		//是否播放声音
	bool isPlaySound;//1=Y,0 = N

BOOL PlaySounds(UINT IDSoundRes, WORD wFlag = SND_ASYNC);


public:
	CFlyGameSound();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlyGameSound)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFlyGameSound();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFlyGameSound)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLYGAMESOUND_H__996BD949_71F8_4971_B5C6_5A0CC87A15DE__INCLUDED_)
