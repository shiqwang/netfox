#include "StdAfx.h"
#include ".\packet.h"

CPacket::CPacket(void)
{
}

CPacket::~CPacket(void)
{
	m_PacketCmd.DeleteAllData();
}

void CPacket::AddPacket( WORD Packet, ... )
{
	string* pFormat = new string;
	va_list marker;
	va_start( marker, Packet );//开始
	BYTE b;
	while( (b = va_arg( marker, BYTE )) != 0 )
	{
		*pFormat += b;
	}
	va_end( marker );//结束

	m_PacketCmd.PutData( Packet, pFormat );
}

int CPacket::GetPacket( char* pBuffer, int length, WORD Packet, ... )
{
	string* pFormat = m_PacketCmd.GetData( Packet );
	if( pFormat )
	{
		int index = 2;
		*PWORD(pBuffer) = Packet;

		va_list marker;
		va_start( marker, Packet );//开始

		BYTE byte;
		WORD word;
		DWORD dword;
		char* p;

		for( int i=0;i<(int)pFormat->size();i++ )
		{
			switch( BYTE((*pFormat)[i]) )
			{
			case T_BYTE:
				if( index>=length )
				{
					va_end( marker );//结束
					return 0;
				}
				byte = va_arg( marker, BYTE );
				pBuffer[index++] = byte;
				break;
			case T_WORD:
				if( index>=length-1 )
				{
					va_end( marker );//结束
					return 0;
				}
				word = va_arg( marker, WORD );
				*(PWORD)&pBuffer[index] = word;
				index += 2;
				break;
			case T_DWORD:
				if( index>=length-3 )
				{
					va_end( marker );//结束
					return 0;
				}
				dword = va_arg( marker, DWORD );
				*(PDWORD)&pBuffer[index] = dword;
				index += 4;
				break;
			case T_LEN_DATA:
				if( index>=length-1 )
				{
					va_end( marker );//结束
					return 0;
				}
				word = va_arg( marker, WORD );
				*PWORD(&pBuffer[index]) = word;
				index += 2;
				if( word == 0 )
				{
					va_end( marker );//结束
					return index;
				}
				if( index>length-word )
				{
					va_end( marker );//结束
					return 0;
				}
				p = va_arg( marker, char* );
				memcpy( &pBuffer[index], p, word );
				index += word;					
				break;
			default:
				pBuffer[index++] = BYTE((*pFormat)[i]);
				break;
			}
		}

		va_end( marker );//结束
		return index;
	}
	return -1;
}

int CPacket::GetValue( const char* pBuffer, int length, WORD Packet, ... )
{
	string* pFormat = m_PacketCmd.GetData( Packet );
	if( pFormat )
	{
		int index = 2;
		*PWORD(pBuffer) = Packet;

		va_list marker;
		va_start( marker, Packet );//开始

		PBYTE pbyte;
		PWORD pword;
		PDWORD pdword;
		WORD len;
		char* p;

		for( int i=0;i<(int)pFormat->size();i++ )
		{
			switch( BYTE((*pFormat)[i]) )
			{
			case T_BYTE:
				if( index>=length )
				{
					va_end( marker );//结束
					return 0;
				}
				pbyte = va_arg( marker, PBYTE );
				if( pbyte != NULL )
				{
					*pbyte = pBuffer[index];
				}
				index++;
				break;
			case T_WORD:
				if( index>=length-1 )
				{
					va_end( marker );//结束
					return 0;
				}
				pword = va_arg( marker, PWORD );
				if( pword != NULL )
				{
					*pword = *(PWORD)&pBuffer[index];
				}
				index += 2;
				break;
			case T_DWORD:
				if( index>=length-3 )
				{
					va_end( marker );//结束
					return 0;
				}
				pdword = va_arg( marker, PDWORD );
				if( pdword != NULL )
				{
					*pdword = *(PDWORD)&pBuffer[index];
				}
				index += 4;
				break;
			case T_LEN_DATA:
				if( index>length-2 )
				{
					va_end( marker );//结束
					return 0;
				}
				pword = va_arg( marker, PWORD );
				if( pword == NULL )
				{
					va_end( marker );//结束
					return 0;
				}
				len = va_arg( marker, WORD );
				*pword = *PWORD(&pBuffer[index]);
				index += 2;
				if( len < *pword )
				{
					va_end( marker );//结束
					return 0;
				}
				if( index>length-*pword )
				{
					va_end( marker );//结束
					return 0;
				}
				p = va_arg( marker, char* );
				memcpy( p, &pBuffer[index], *pword );
				index += *pword;					
				break;
			default:
				if( index>=length )
				{
					va_end( marker );//结束
					return 0;
				}
//				pBuffer[index++] = BYTE((*pFormat)[i]);
				index++;
				break;
			}
		}

		va_end( marker );//结束
		return index;
	}
	return -1;
}