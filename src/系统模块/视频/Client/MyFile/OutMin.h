#pragma once

#include "..\DirectShow\GraphBuilder.h"
#include "..\DirectShow\MyBaseFilter.h"
#include "..\DirectShow\OutputPin.h"

class COutMin
{
private:
	CGraphBuilder* m_pOutGraphBuilder;
	CMyBaseFilter* m_pOutMyBaseFilter;
public:
	COutMin(void);
	virtual ~COutMin(void);

	void Init(void);
	void OutSound( char* pData, int len );
	void Release(void);
};
