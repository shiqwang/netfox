/*-------------------------------------------------------------
|
|					Âé½«Àà   Transend_Mj_struct.cpp
|	
---------------------------------------------------------------*/

#include "Mj_struct.h"

tMj::tMj()
{
	m_value = 0;
	//m_style = MJ_NULL;
	//m_state = MJS_TABLE;
	//m_toward = MJT_BACK;
	//m_x = 0;
	//m_y = 0;
	//m_width = 45;
	//m_height = 69;
}

//tMj::tMj( BYTE value, BYTE style )
//{
//	m_value = value;
//	//m_style = style;
//	//m_state = MJS_TABLE;
//	//m_toward = MJT_BACK;
//	//m_x = 0;
//	//m_y = 0;
//	//m_width = 45;
//	//m_height = 69;
//}
tMj::~tMj()
{
}

bool tMj::isUpperthan(tMj &tdc )
{
	//if( m_style == tdc.m_style )
	//{
	//	if( m_value > tdc.m_value )
	//		return true;
	//	else
	//		return false;
	//}
	//else
	//{
	//	if( m_style > tdc.m_style )
	//		return true;
	//	else
			return false;
	//}
}

bool tMj::isUpperthan( tMj *ptdc )
{
	//if( m_style == ptdc->m_style )
	//{
	//	if( m_value > ptdc->m_value )
	//		return true;
	//	else
	//		return false;
	//}
	//else
	//{
	//	if( m_style > ptdc->m_style )
	//		return true;
	//	else
			return false;
	//}
}

tMj tMj::operator +( int value )
{
	m_value += value ;
	return *this;
}

void tMj::operator =( tMj &tdc )
{
	m_value = tdc.m_value;
	//m_style = tdc.m_style;

	//m_height = tdc.m_height;
	//m_width = tdc.m_width;
}

bool tMj::operator ==( tMj &ptdc )
{
	//if( m_value==ptdc.m_value && m_style==ptdc.m_style )
	//	return true;
	//else
		return false;
}

void tMj::newtMj()
{
	m_value = 0;
	//m_style = MJ_NULL;
	//m_state = MJS_TABLE;
	//m_toward = MJT_BACK;
	//m_x = 0;
	//m_y = 0;
	//m_width = 45;
	//m_height = 69;
}

//void tMj::SetState( BYTE state, BYTE toward )
//{
//	m_state = state;
//	m_toward = toward;
//}
//void tMj::SetPos( int x, int y )
//{
//	m_x = x;
//	m_y = y;
//}

void tMj::SetValue( BYTE value )
{
	m_value = value;
}

BYTE tMj::GetValue()
{
	return m_value;
}
//
//void tMj::SetStyle( BYTE style )
//{
//	m_style = style;
//}
//
//BYTE tMj::GetStyle()
//{
//	return m_style;
//}
//void tMj::SetMjSize( int width, int height )
//{
//	m_width = width;
//	m_height = height;
//}
