#pragma once

#define __LCC__
#include <mysql.h>
#pragma comment( lib, "libmysql.lib" )

typedef enum
{
	MYSQL_ERROR_NO_INIT,
	MYSQL_ERROR_CONNECT,
	MYSQL_ERROR_SELECT_DB,
	MYSQL_ERROR_QUERY,
};

class CWinMySql
{
protected:
	MYSQL* MySql;
public:
	CWinMySql(void);
	virtual ~CWinMySql(void);

	int Connect( const char *host=NULL, const char *user=NULL, const char *passwd=NULL, const char *db=NULL, unsigned int port=3306 );
	int SelectDB( const char *db );
	MYSQL_RES* Query( const char *query, unsigned long length=0 );
	int GetResNum( MYSQL_RES* MySqlRes );
	int FreeQuery( MYSQL_RES* MySqlRes );
};
