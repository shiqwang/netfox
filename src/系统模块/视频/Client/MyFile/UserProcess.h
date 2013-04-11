#pragma once
#include "MyUserManage.h"
#include "..\..\udpsock\user.h"
#include "..\..\UdpSock\MyPacket.h"

#include <vector>
using namespace std;

#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120
#define IMAGE_SIZE IMAGE_WIDTH*IMAGE_HEIGHT*3
#define YUV_SIZE IMAGE_WIDTH*IMAGE_HEIGHT+IMAGE_WIDTH*IMAGE_HEIGHT/2
#define REGION_SIZE 450


#pragma pack(1)
typedef struct _USER_INFO
{
	DWORD W_IP; //外部IP
	DWORD W_PORT; //外部PORT
	DWORD N_IP; //内部IP
	DWORD N_PORT; //内部PORT
	DWORD POS;
	DWORD T;
}USER_INFO,*PUSER_INFO;
#pragma pack()

typedef vector<PUSER_INFO> USER_LIST;

typedef queue<char*> SOUND_LIST;

static CMyPacket m_MyPacket;

class CUserProcess : public CUser
{
private:
	static USER_LIST m_UserList;
	char m_pSampleBuffer[0x40000];
	BYTE m_CompressBuffer[YUV_SIZE];
	BYTE m_YUVBuffer[YUV_SIZE];
	BYTE m_Sound[6000];
	BYTE m_CheckSound[255];
	int m_YUVCount;
	DWORD m_YUVTestTime;
	int m_YUVLen;
	BYTE m_CheckYUV[255];
	BOOL m_NewVedio;
	SOUND_LIST m_SoundList;
	DWORD m_WindowPos;
public:
	CUserProcess(void);
	virtual ~CUserProcess(void);

	static USER_INFO m_ServerIP;

	DWORD GetWindowPos(){ return m_WindowPos; };

	virtual void Parsing( char* pData, int length );
	BOOL AddUser( DWORD W_IP, DWORD W_PORT, DWORD N_IP, DWORD N_PORT, DWORD Pos );
	BOOL DelUser( DWORD W_IP, DWORD W_PORT );
	BOOL TestServer(void){ return (inet_addr(m_IP)==m_ServerIP.W_IP && m_Port==m_ServerIP.W_PORT); };
	void SaveSoundData( char* Buffer, int length );
	void SendSoundData();
	void Send( char* Buf, int len, char* lpTo = NULL, unsigned int Port = 0 );
	BOOL Check( DWORD Time );
};
