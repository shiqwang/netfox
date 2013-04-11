#include "StdAfx.h"
#include ".\userprocess.h"
#include "VideoClient.h"
#include "..\DirectShow\Compress\CConvertRgbYuv.h"
//#include "..\DirectShow\Compress\LZARI.h"
#include "..\zlib123\zlib.h"
#pragma comment(lib, "zlib123\\lib\\zlib.lib")

USER_INFO CUserProcess::m_ServerIP = {0,0,0,0,0};
USER_LIST CUserProcess::m_UserList;

extern CVideoClient* g_pVideoClient;
extern CConvertRgbYuv m_ConvertRgbYuv;
extern USER_MAP m_UserMap;
extern BYTE YUVBuffer[YUV_SIZE];

CUserProcess::CUserProcess(void)
{
	m_UserList.reserve( 5 );
	m_NewVedio = TRUE;
	m_YUVCount = 0;
	m_YUVTestTime = 0;
	ZeroMemory( m_CheckSound, 255 );
	ZeroMemory( m_YUVBuffer, YUV_SIZE );
	m_WindowPos = 0;
}

CUserProcess::~CUserProcess(void)
{
	char tmp[MAX_PATH];
	int len = m_MyPacket.GetPacket( (char*)tmp, MAX_PATH, PACKET_QUIT, 1, NULL );
	Send( tmp, len );

	char* pSound;
	for( int i=0; i<(int)m_SoundList.size();i++ )
	{
		pSound = m_SoundList.front();
		m_SoundList.pop();
		if( pSound )
		{
			delete pSound;
		}
	}
	if( g_pVideoClient )
	{
		g_pVideoClient->OutString( m_WindowPos, "断开连接" );
	}
	m_WindowPos = 0;
}

void CUserProcess::Parsing( char* pData, int length )
{
	WORD Packet = *PWORD(pData);
	switch( Packet )
	{
	case PACKET_LOGIN_OK:
		{
			m_ServerIP.W_IP = inet_addr(m_IP);
			m_ServerIP.W_PORT = m_Port;
			if( m_MyPacket.GetValue( pData, length, Packet, &m_ServerIP.N_IP, &m_ServerIP.N_PORT, &m_ServerIP.POS ) > 0 )
			{
#ifdef _DEBUG
				CString Debug;
				Debug.Format( "PACKET_LOGIN_OK POS:%d", m_ServerIP.POS );
				g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			}
		}
		break;
	case PACKET_ADDCLIENT: //登陆
		{
			DWORD W_IP = 0, W_PORT = 0, N_IP = 0, N_PORT = 0, Pos = 0, bb;
			if( m_MyPacket.GetValue( pData, length, Packet, &W_IP, &W_PORT, &N_IP, &N_PORT, &Pos ) > 0 )
			{
#ifdef _DEBUG
				CString Debug;
				Debug.Format( "PACKET_ADDCLIENT WIP:%d WPORT:%d NIP:%d NPORT:%d POS:%d", W_IP, W_PORT, N_IP, N_PORT, Pos );
				g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
				CUserManage* p = (CUserManage*)g_pVideoClient->GetSocket(Pos);
				if( p==NULL )
					break;
				//if( AddUser( W_IP, W_PORT, N_IP, N_PORT, bb ) )
				{
					char Tmp[MAX_PATH];
					if( W_IP == m_ServerIP.N_IP )
					{
						//AddUser( N_IP, N_PORT, 0, 0, Pos );
						int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT, m_ServerIP.POS );
						p->Send( Tmp, len, inet_ntoa(*((in_addr*)&N_IP)), N_PORT ); //内部连接
					}
					else
					{
						int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_SWAP_CONNECT, g_pVideoClient->m_Room,  g_pVideoClient->m_Desk, m_ServerIP.POS, Pos );
						p->Send( Tmp, len, m_IP, m_Port );
						//AddUser( W_IP, W_PORT, 0, 0, Pos );
						//p->Send( Tmp, len, inet_ntoa(*((in_addr*)&W_IP)), W_PORT );
					}
				}
			}
		}
		break;
	case PACKET_SWAP_CONNECT:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format( "PACKET_SWAP_CONNECT" );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			DWORD IP = 0, PORT = 0, Pos = 0, wPos = 0;
			char Tmp[MAX_PATH];
			if( m_MyPacket.GetValue( pData, length, Packet, &IP, &PORT, &Pos, &wPos ) > 0 )
			{
				CUserManage* p = (CUserManage*)g_pVideoClient->GetSocket(Pos);
				if(p)
				{
					int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT, m_ServerIP.POS );
					p->Send( Tmp, len, inet_ntoa(*((in_addr*)&IP)), PORT ); //内部连接
				}
			}
		}
		break;
	case PACKET_LOGIN_ERR:
		{
			DWORD Error = 0;
#ifdef _DEBUG
			CString Debug;
			Debug.Format( "PACKET_LOGIN_ERR POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			if( m_MyPacket.GetValue( pData, length, Packet, &Error ) > 0 )
			{
				switch( Error )
				{
				case 5:
					AfxMessageBox( "房间以满" );
				}
			}
		}
		break;
	case PACKET_QUIT:
		{
			CheckTime = 0;
		}
		break;
	case PACKET_USER_CONNECT:
		{
			int Pos;
			if( m_MyPacket.GetValue( pData, length, Packet, &Pos ) > 0 )
			{
#ifdef _DEBUG
				CString Debug;
				Debug.Format( "PACKET_USER_CONNECT IP:%d PORT:%d POS:%d", m_IP, m_Port, Pos );
				g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
				if( !(Pos & 0x80000000) )
				{
					char Tmp[MAX_PATH];
					int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT, m_ServerIP.POS | 0x80000000 );
					CUserManage* p = (CUserManage*)g_pVideoClient->GetSocket(Pos);
					if(p)
					{
						p->Send( Tmp, len, m_IP, m_Port );
					}
				}
				else
				{
					Pos-=0x80000000;
				}
				m_WindowPos = Pos;

/*				char Index[MAX_PATH];
				sprintf( Index, "%s:%d", m_IP, m_Port );
				CUserProcess* pUser = (CUserProcess*)CUserManage::m_UserMap.GetData( Index );
				if( pUser )
				{
					pUser->m_WindowPos = Pos;
					char Tmp[MAX_PATH];
					AddUser( inet_addr(m_IP), m_Port, 0, 1, Pos );
//					int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT_OK, 3 );
//					CUser::Send( Tmp, len, m_IP, m_Port );
					int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT_OK, 1 );
//					pUser->Send( Tmp, len, inet_ntoa( *(in_addr*)&m_ServerIP.W_IP), m_ServerIP.W_PORT );
					pUser->Send( Tmp, len );
				}*/
			}
		}
		break;
	case PACKET_USER_CONNECT_OK:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format( "PACKET_USER_CONNECT_OK IP:%d PORT:%d POS:%d", m_IP, m_Port, m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			int flag;
			if( m_MyPacket.GetValue( pData, length, Packet, &flag ) > 0 )
			{
				switch( flag )
				{
				case 1:
					{
						char Tmp[MAX_PATH];
						int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT_OK, 2 );
						Send( Tmp, len );
					}
					break;
				}
				PUSER_INFO pUserInfo;
/*				for( int i=0;i<(int)m_UserList.size();i++ )
				{
					pUserInfo = m_UserList[i];
					if( pUserInfo )
					{
						if( pUserInfo->W_IP ==inet_addr(m_IP) && pUserInfo->W_PORT==m_Port )
						{
							m_UserList.erase(find(m_UserList.begin(),m_UserList.end(),pUserInfo));
							delete pUserInfo;
							break;
						}
					}
				}*/
				g_pVideoClient->OutString( m_WindowPos, "已连接" );
			}
		}
		break;
	case PACKET_USER_VEDIO:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format( "PACKET_USER_VEDIO POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			if( length>=9 )
			{
				m_YUVTestTime = GetTickCount()+1000;
				int i = *PDWORD(&pData[2]);
                int len = *PWORD(&pData[6]);
				if( len+8 ==  length )
				{
					m_CheckYUV[i] = 1;
					memcpy( &m_pSampleBuffer[i*REGION_SIZE], pData+8, length-8 );
				}
			}
		}
		break;
	case PACKET_USER_SOUND:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_SOUND POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			if( length>=9 )
			{
				int i = *PDWORD(&pData[2]);
                int len = *PWORD(&pData[6]);
				if( len+8 ==  length )
				{
					m_CheckSound[i] = 1;
					memcpy( &m_Sound[i*REGION_SIZE], pData+8, length-8 );
				}
			}
		}
		break;
	case PACKET_USER_VEDIO_OK:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_VEDIO_OK POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			int len = 0, index = 0, b= 0;
			BYTE p[255];
			if( m_MyPacket.GetValue( pData, length, Packet, &b, &len ) > 0 )
			{
				for( int i=0; i<b ;i++ )
				{
					if( !m_CheckYUV[i] )
					{
						p[index] = i;
						index++;
					}
				}
				if( index==0 )
				{
#ifdef _DEBUG
					g_pVideoClient->SendDebugMsg("PACKET_USER_VEDIO_OK OK");
#endif
					uLongf OutLen = YUV_SIZE;
					BYTE m_YUVBuffer[YUV_SIZE];
					int err = uncompress((Bytef *)m_YUVBuffer, &OutLen, (Bytef *)m_pSampleBuffer, len);
					if (err == Z_OK)
					{
						m_ConvertRgbYuv.YUV_RGB24( m_YUVBuffer, (BYTE*)m_pSampleBuffer, IMAGE_WIDTH, IMAGE_HEIGHT );
						g_pVideoClient->OutVideo( m_WindowPos-1 ,m_pSampleBuffer, IMAGE_SIZE );
					}
					m_NewVedio = TRUE;
				}
				else
				{
#ifdef _DEBUG
					g_pVideoClient->SendDebugMsg("PACKET_USER_VEDIO_OK NO");
#endif
					char Tmp[MAX_PATH];
					int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_GET_VEDIO, index, p, NULL );
					Send( Tmp, len );
				}
			}
		}
		break;
	case PACKET_USER_SOUND_OK:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_SOUND_OK POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			int len = 0, index = 0, b= 0;
			BYTE p[255] = {0};
			uLongf OutLen = 6000;
			BYTE m_SoundBuffer[6000] = {0};
			int err = 0;
			if( m_MyPacket.GetValue( pData, length, Packet, &b, &len ) > 0 )
			{
				for( int i=0; i<b ;i++ )
				{
					if( !m_CheckSound[i] )
					{
						p[index] = i;
						index++;
					}
				}
				if( index==0 )
				{
#ifdef _DEBUG
					g_pVideoClient->SendDebugMsg("PACKET_USER_SOUND_OK OK");
#endif
					err = uncompress((Bytef *)m_SoundBuffer, &OutLen, (Bytef *)m_Sound, len);
					if (err == Z_OK)
					{
						g_pVideoClient->OutSound( (char*)m_SoundBuffer, OutLen );
					}
					ZeroMemory( m_CheckSound, 255 );
					int len = m_MyPacket.GetPacket( (char*)m_SoundBuffer, MAX_PATH, PACKET_USER_SOUND_NEXT, 1, NULL );
					Send( (char*)m_SoundBuffer, len );
				}
				else
				{
#ifdef _DEBUG
					g_pVideoClient->SendDebugMsg("PACKET_USER_SOUND_OK NO");
#endif
					char Tmp[MAX_PATH];
					int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_GET_SOUND, index, p, NULL );
					Send( Tmp, len );
				}
			}
		}
		break;
	case PACKET_USER_VEDIO_NEW:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_VEDIO_NEW POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			if( m_NewVedio )
			{
#ifdef _DEBUG
				g_pVideoClient->SendDebugMsg("PACKET_USER_VEDIO_NEW OK");
#endif
				m_YUVTestTime = GetTickCount()+1000;
				m_NewVedio = FALSE;
				ZeroMemory( m_CheckYUV, 255 );
				char Tmp[MAX_PATH];
				int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_GET_NEW_VEDIO, 0, NULL );
				Send( Tmp, len );
			}
			else
			{
#ifdef _DEBUG
				g_pVideoClient->SendDebugMsg("PACKET_USER_VEDIO_NEW NO");
#endif
				if( m_YUVTestTime < GetTickCount() )
				{
					char Tmp[MAX_PATH];
					int len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_GET_VEDIO, 0, Tmp, NULL );
					Send( Tmp, len );
				}
			}
		}
		break;
	case PACKET_USER_GET_NEW_VEDIO:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_GET_NEW_VEDIO POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
//			LZARI lzari;
//			BYTE* p = NULL;
//			int outlen;
//			lzari.Compress( YUVBuffer, YUV_SIZE, p, outlen );
			uLong OutLen  = YUV_SIZE;
			//BYTE SendData[YUV_SIZE*2];
			//BYTE* pSendData = SendData;
			//int i = 0, j = 0, k = 0;
            /*for(;i<YUV_SIZE;i++)
			{
				if( m_YUVBuffer[i]!=YUVBuffer[i] )
				{
					k = 0;
					for( j=i+1;j<YUV_SIZE, k<10;j++ )
					{
                        if( m_YUVBuffer[j]==YUVBuffer[j])
						{
                            k++;							
						}
					}
					if( k>=10 )
					{
                        *PDWORD( pSendData ) = i; //地址
						pSendData += 4;
						*PDWORD( pSendData ) = j-i-k; //长度
                        pSendData += 4;
						memcpy( (char*)pSendData, &YUVBuffer[i], j-i-k );
						pSendData += j-i-k;
						i = j+1;
					}
				}
			}
			memcpy( m_YUVBuffer, YUVBuffer, YUV_SIZE );*/
			int err = compress2( (Bytef *)m_CompressBuffer, &OutLen, YUVBuffer, YUV_SIZE, 9 );
			//int err = compress2( (Bytef *)m_CompressBuffer, &OutLen, SendData, pSendData-SendData, 9 );
			if (err != Z_OK)
			{
                char tmp[MAX_PATH];
				int len = m_MyPacket.GetPacket( tmp, MAX_PATH, PACKET_USER_VEDIO_ERR, 1 );
				Send(tmp,len);
                break;
			}
//			memcpy( m_YUVBuffer, p, OutLen );
//			lzari.Release();

			int sendlen=0, max = REGION_SIZE, len = 0;
			m_YUVCount=0;
			char tmp[REGION_SIZE+100];
			int i = 0;
			while( sendlen<(int)OutLen )
			{
				if( sendlen+max>(int)OutLen )
				{
					max = OutLen - sendlen;
				}
				len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_VEDIO, i, max, (char*)&m_CompressBuffer[sendlen] );
				Send( tmp, len );
				sendlen += max;
				i++;
			}
			m_YUVCount = i;
			m_YUVLen = OutLen;
			len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_VEDIO_OK, i, OutLen );
			Send( tmp, len );
		}
		break;
	case PACKET_USER_GET_VEDIO:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_GET_VEDIO POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			BYTE p[1024];
			int Count = 0;
			int sendlen=0, max = REGION_SIZE, i = 0, len = 0;
			char tmp[REGION_SIZE+100];
            if( m_MyPacket.GetValue( pData, length, Packet, &Count, 1024, p ) > 0 )
			{
#ifdef _DEBUG
				CString Debug;
				Debug.Format( "PACKET_USER_GET_VEDIO COUNT:%d", Count );
				g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
				for( int i=0; i<Count; i++ )
				{
					sendlen = p[i]*REGION_SIZE;
					if( sendlen+max>m_YUVLen )
					{
						max = m_YUVLen - sendlen;
					}
					len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_VEDIO, p[i], max, (char*)&m_CompressBuffer[sendlen] );
					Send( tmp, len );
				}
				len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_VEDIO_OK, m_YUVCount, m_YUVLen );
				Send( tmp, len );
			}
		}
		break;
	case PACKET_USER_GET_SOUND:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_GET_SOUND POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			BYTE p[255];
			char tmp[REGION_SIZE+100];
			int sendlen=0, max = REGION_SIZE, i = 0, len = 0;
			char* pSound = NULL;
			int Count = 0;
            if( m_MyPacket.GetValue( pData, length, Packet, &Count, 255, p ) > 0 )
			{
#ifdef _DEBUG
				CString Debug;
				Debug.Format( "PACKET_USER_GET_SOUND COUNT:%d", Count );
				g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
				if( m_SoundList.empty() )
				{
					len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_SOUND_ERR, 1 );
					Send( tmp, len );
					break;
				}
				pSound = m_SoundList.front();
				if( pSound )
				{
					int SoundLen = *PDWORD(pSound);
					pSound += 4;
					for( int i=0; i<Count; i++ )
					{
						sendlen = p[i]*REGION_SIZE;
						if( sendlen+max>SoundLen )
						{
							max = SoundLen - sendlen;
						}
						len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_SOUND, p[i], max, (char*)&pSound[sendlen] );
						Send( tmp, len );
					}
					max = SoundLen / REGION_SIZE + (SoundLen % REGION_SIZE > 0);
					len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_SOUND_OK, max, SoundLen );
					Send( tmp, len );
				}
			}
		}
        break;
	case PACKET_USER_VEDIO_ERR:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_VEDIO_ERR POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			m_NewVedio = TRUE;
		}
		break;
	case PACKET_USER_SOUND_ERR:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_SOUND_ERR POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			ZeroMemory( m_CheckSound, 10 );
		}
		break;
	case PACKET_USER_SOUND_NEXT:
		{
#ifdef _DEBUG
			CString Debug;
			Debug.Format("PACKET_USER_SOUND_NEXT POS:%d", m_WindowPos );
			g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
			if( !m_SoundList.empty() )
			{
				char* pSound = m_SoundList.front();
				m_SoundList.pop();
				delete pSound;
			}
			SendSoundData();
		}
		break;
	case 0:
		{
/*			CString s = m_IP;
			if( s=="" )
				s = "";
			PUSER_INFO pUserInfo;
			char Tmp[MAX_PATH];
			int len = 0;
			DWORD T = GetTickCount();

			for( int i=m_UserList.size()-1;i>=0;i-- )
			{
                pUserInfo = m_UserList[i];
				if( pUserInfo )
				{
					CUserManage* p = (CUserManage*)g_pVideoClient->GetSocket(pUserInfo->POS);
					if( p )
					{
						switch( pUserInfo->N_PORT )
						{
						case 0:
							len = m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT, m_ServerIP.POS );
							break;
						case 1:
							m_MyPacket.GetPacket( Tmp, MAX_PATH, PACKET_USER_CONNECT_OK, 1 );
							break;
						}
						p->Send( Tmp, len, inet_ntoa( *(in_addr*)&pUserInfo->W_IP), pUserInfo->W_PORT );
					}
					pUserInfo->N_IP++;
					if( pUserInfo->N_IP>5 )
					{
						m_UserList.erase(find(m_UserList.begin(),m_UserList.end(),pUserInfo));
						delete pUserInfo;
					}					
				}
			}*/
		}
		break;
	}
}

BOOL CUserProcess::AddUser( DWORD W_IP, DWORD W_PORT, DWORD N_IP, DWORD N_PORT, DWORD Pos )
{
	USER_INFO UserInfo = { W_IP, W_PORT, N_IP, N_PORT, Pos };
	PUSER_INFO pUserInfo;
	for( int i=0; i<(int)m_UserList.size(); i++ )
	{
        pUserInfo = m_UserList[i];
		if( memcmp( pUserInfo, &UserInfo, sizeof(UserInfo)-4 )==0 )
		{
			return FALSE;
		}
	}
	pUserInfo = new USER_INFO;
	*pUserInfo = UserInfo;
	pUserInfo->T = GetTickCount()+15000;
	m_UserList.push_back( pUserInfo );
	return TRUE;
}

BOOL CUserProcess::DelUser( DWORD W_IP, DWORD W_PORT )
{
	PUSER_INFO pUserInfo;
	USER_LIST::iterator iter;
	for( int i=0;i<(int)m_UserList.size();i++ )
	{
		pUserInfo = m_UserList[i];
		if( pUserInfo->POS == 0 )
		{
			if( pUserInfo->W_IP == W_IP && pUserInfo->W_PORT == W_PORT )
			{
				m_UserList.erase(find(m_UserList.begin(),m_UserList.end(),pUserInfo));
                delete pUserInfo;
				break;
			}
		}
		else
		{
			if( pUserInfo->N_IP == W_IP && pUserInfo->N_PORT == W_PORT )
			{
				m_UserList.erase(find(m_UserList.begin(),m_UserList.end(),pUserInfo));
                delete pUserInfo;
				break;
			}
		}
	}
	return true;
}

void CUserProcess::SaveSoundData( char* Buffer, int length )
{
	char* pData = new char[length+4];
	*PDWORD(pData) = length;
	memcpy( pData+4, Buffer, length );
	m_SoundList.push(pData);

	pData = m_SoundList.front();
	length = *PDWORD(pData);
	char tmp[MAX_PATH];
	int i = length / REGION_SIZE + ((length % REGION_SIZE) > 0) ;
	int len = m_MyPacket.GetPacket( tmp, MAX_PATH, PACKET_USER_SOUND_OK, i, length );
	Send( tmp, len );
}

void CUserProcess::SendSoundData()
{
	int length = 0;
	if( m_SoundList.empty() )
	{
		char tmp[MAX_PATH];
		length = m_MyPacket.GetPacket( tmp, MAX_PATH, PACKET_USER_SOUND_ERR, 1 );
		Send( tmp, length );
		return;
	}
	char* pData = m_SoundList.front();
	if( pData )
	{
		length = *PDWORD(pData);
		pData += 4;

		int sendlen=0, max = REGION_SIZE, i = 0, len = 0;
		
		char tmp[REGION_SIZE+100];
		while( sendlen<(int)length )
		{
			if( sendlen+max>(int)length )
			{
				max = length - sendlen;
			}
			len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_SOUND, i, max, &pData[sendlen] );
			sendlen += max;
			i++;
		}
		len = m_MyPacket.GetPacket( tmp, REGION_SIZE+100, PACKET_USER_SOUND_OK, i, length );
		Send( tmp, len );
	}
}

void CUserProcess::Send( char* Buf, int len, char* lpTo, unsigned int Port )
{
	if( lpTo == NULL )
	{
		lpTo = m_IP;
	}
	if( Port == 0 )
	{
        Port = m_Port;
	}
	CUserManage* p = (CUserManage*)g_pVideoClient->GetSocket( m_WindowPos );
	if(p)
	{
#ifdef _DEBUG
		CString Debug;
		Debug.Format( "SEND %x %d", *PWORD(Buf), m_WindowPos );
        g_pVideoClient->SendDebugMsg(Debug.GetBuffer());
#endif
		p->Send( Buf, len, lpTo, Port );
	}
}

BOOL CUserProcess::Check( DWORD Time )
{
	char Tmp[2];
	ZeroMemory( Tmp, sizeof(Tmp) );
	Send( Tmp, 2 );
	return Time>CheckTime?TRUE:FALSE;
}