// STLMap.h: interface for the CSTLMap class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _STL_MAP
#define _STL_MAP

#pragma warning(disable : 4786)

#include <map>
#include <string>
using namespace std;

//map模板
template <class T1,class T2> class CSTLMap  
{
public:
	typedef typename map < T1, T2* >::iterator		Iterator; //值类型定义
	typedef typename map < T1, T2* >::value_type	ValueType; //值类型

	map < T1, T2* >		m_UserTypeMap; //集合
	
	int GetSize() { return (int)m_UserTypeMap.size(); }; //得到大小
	
	bool PutData( T1 key_value, T2* pData) //压入数据
	//Iterator PutData( T1 key_value, T2* pData)
	{
		if( !pData ) return false;
		pair<Iterator, bool> temp_pair = m_UserTypeMap.insert( ValueType( key_value, pData ) );
		//Iterator temp_pair = m_UserTypeMap.insert( m_UserTypeMap.end(),  ValueType( key_value, pData ) );
		return temp_pair.second;
		//return temp_pair;
	};

	T2* GetData( T1 key_value ) //取数据
	{
		T2* pData = NULL;
		Iterator iter = m_UserTypeMap.find( key_value );
		if( iter == m_UserTypeMap.end() )
			return NULL;
		else
			pData = (*iter).second;
		
		return pData;
	};

	T2* GetIData( T1 key_value ) //取值数据
	{
		T2* pData = NULL;
		Iterator iter = m_UserTypeMap.begin();
		for( ; iter != m_UserTypeMap.end(); iter++ )
		{
			if( !stricmp( key_value.data(), (*iter).first.data()) )
			{
				pData = (*iter).second;
				break;
			}
		}
		return pData;
	};

	Iterator DeleteData( T1 key_value ) //删除数据
	{
		if( m_UserTypeMap.empty() )
			return m_UserTypeMap.end();
		
		Iterator iter = m_UserTypeMap.find( key_value );
		if( iter == m_UserTypeMap.end() )
			return m_UserTypeMap.end();
		else {
			T2* pData = NULL;
			pData = (*iter).second;

			iter = m_UserTypeMap.erase(iter);

			delete pData;
			pData = NULL;

			return iter;
		}
	};

	Iterator PopData( T1 key_value ) //压出数据，不删除
	{
		if( m_UserTypeMap.empty() )
			return m_UserTypeMap.end();
		
		Iterator iter = m_UserTypeMap.find( key_value );
		if( iter == m_UserTypeMap.end() )
			return m_UserTypeMap.end();
		else {
			iter = m_UserTypeMap.erase(iter);
			return iter;
		}
	};

	void DeleteAllData() //删除所有数据
	{
		Iterator iter1 = m_UserTypeMap.begin(), iter2 = m_UserTypeMap.end();
		for(; iter1 != iter2; iter1++ )
			delete (*iter1).second;

		m_UserTypeMap.clear();
	};

	void Clear() //清空，但不删除
	{
        m_UserTypeMap.clear();
	};

	bool IsExist( T1 key_value ) //判断是否有此数据
	{
		Iterator iter = m_UserTypeMap.find( key_value );
		if( iter == m_UserTypeMap.end() )
			return false;
		else
			return true;
	};

	bool IsEmpty() { return m_UserTypeMap.empty(); }; //是否为空

	CSTLMap() { };
	virtual ~CSTLMap() {
		DeleteAllData();
	};
};

#endif