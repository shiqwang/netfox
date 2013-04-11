#pragma once

#include "windows.h"
#include <vector>
using namespace std;

#pragma pack(1)
typedef struct _USER_INFO
{
	DWORD W_IP; //外部IP
	DWORD W_PORT; //外部PORT
	DWORD N_IP; //内部IP
	DWORD N_PORT; //内部PORT
	DWORD POS; //位置
}USER_INFO,*PUSER_INFO;
#pragma pack()

typedef vector<PUSER_INFO> USER_LIST;

class CDeskID
{
private:
	USER_LIST m_UserList;
public:
	CDeskID(void);
	~CDeskID(void);

	PUSER_INFO AddUser( DWORD W_IP, DWORD W_PORT, DWORD N_IP, DWORD N_PORT, DWORD& Pos );
	PUSER_INFO GetUser( int index );
	BOOL DeleteUser( PUSER_INFO pUserInfo );
	int GetUserCount(void){ return (int)m_UserList.size(); };
};
