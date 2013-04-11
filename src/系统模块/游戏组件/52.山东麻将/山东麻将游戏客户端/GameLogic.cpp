/*-------------------------------------------------------------------------------------------
|
|								Mj		CGameLogic.cpp
|	
|------------------------------------------------------------------------------------------*/

#include "stdafx.h"
#include "GameLogic.h"
#include <vector>
//#define NOVE_FLOWER			//有花
///////////////////////////////////////////////////////////////////////////////////////////////
//静态变量

//麻将数据
//#ifdef HAVE_FLOWER
//const BYTE CGameLogic::m_byMjs[ORG_MJ_MAX] = 
//{
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//	0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,						//花
//};
//#endif
////无花
//#ifdef NO_FLOWER
//const BYTE CGameLogic::m_byMjs[ORG_MJ_MAX] = 
//{
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,				//万
//	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,				//饼
//	0X21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,				//条
//	0x31, 0x32, 0x33, 0x34,												//风
//	0x41, 0x42, 0x43,													//字
//};
//#endif
//十三幺
const BYTE CGameLogic::mjThirteen[13] =
{
	0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43
};
const BYTE CGameLogic::mjNineLight[13] =
{
	1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 9 
};
/////////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//混乱麻将
void CGameLogic::RandMj( BYTE byMjs[], BYTE byMjCount )
{
	srand( (unsigned)time( NULL ) );
	BYTE index,mjTemp;

	//混乱次数
	//for( int i=0; i<2; i++ )
	{
		for( int j=0; j<byMjCount; j++ )
		{
			index = rand()%byMjCount;
			if( j!=index )
			{
				mjTemp = byMjs[j];
				byMjs[j] = byMjs[index];
				byMjs[index] = mjTemp;
			}
		}
	}
}

//排序麻将
void CGameLogic::SortMj( BYTE byMjs[], BYTE byMjCount, BYTE byMjMax )
{
	BYTE i, j, tag, mjTemp;
	for( i=0,tag=1; tag==1&&i<byMjCount; i++ )
	{
		tag = 0;
		for( j=0; j<byMjCount; j++ )
		{
			if( j+1<byMjMax && byMjs[j]>byMjs[j+1]  )
			{
				mjTemp = byMjs[j];
				byMjs[j] = byMjs[j+1];
				byMjs[j+1] = mjTemp;
				tag = 1;
			}
		}
	}
}

//类型判断
MJ_BLOCK CGameLogic::CheckBlock( BYTE byMj1, BYTE byMj2, BYTE byMj3 )
{
	MJ_BLOCK block;
	block.first = 0xff;
	block.style = BLOCK_NULL;
	
	//如果byMj1, byMj2, byMj3不是顺序结构,排序
	int i, tag, temp;
	for( i=0,tag=1; tag==1&&i<3; i++ )
	{
		tag = 0;
		if( byMj1 > byMj2 )
		{
			temp = byMj1;
			byMj1 = byMj2;
			byMj2 = temp;
			tag = 1;
		}
		if( byMj2 > byMj3 )
		{
			temp = byMj2;
			byMj2 = byMj3;
			byMj3 = temp;
			tag = 1;
		}
	}
	
	//碰形结构

	if( byMj1==byMj2 && byMj2==byMj3 )
	{
		block.first = byMj1;
		block.style = BLOCK_KE;
		return block;
	}

	//顺形结构	
	if( byMj3<0x31 )			//不为字, 花
	{
		if( byMj3==byMj2+1 && byMj2==byMj1+1 )
		{
			block.first = byMj1;
			block.style = BLOCK_SUN;
			return block;
		}
	}
	return block;
}

//是否可以做将牌
bool CGameLogic::IsJiang( BYTE byMj1, BYTE byMj2 )
{
	if( byMj1 == byMj2 )
	{
		if (GetMjValue(byMj1)==2 || GetMjValue(byMj1)==5 || GetMjValue(byMj1)==8)//山东麻将2，5，8做将牌
			if (GetMjType(byMj1)<0x30)
				return true;
	}
	return false;
}

//是否可以吃牌
int CGameLogic::IsChi( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj )
{
	//风,字不能上
	if( byMj > 0x30 )
	{
		return false;
	}
	int chiInfo = 0;			//吃牌信息, 0-无吃牌,1-**@型, 2-*@*, 4-@**型
	bool pos2,pos1,pos;
	pos2 = pos1 = pos = true;
	for( int i=HAND_MJ_MAX-byHandMjCount-1; i<HAND_MJ_MAX-1; i++ )
	{
		//**@
		if( byHandMj[i] == byMj-2 && pos2 )
		{
			for( int j=i; j<HAND_MJ_MAX-1; j++ )
			{
				if( byHandMj[j] == byMj-1 )
				{
					chiInfo += 1;			// 吃牌类型1
					break;
				}
			}
			pos2 = false;
		}
		//*@*
		else if( byHandMj[i] == byMj-1 && pos1 )
		{
			for( int j=i; j<HAND_MJ_MAX-1; j++ )
			{
				if( byHandMj[j] == byMj+1 )
				{
					chiInfo += 2;			//吃牌类型2
					break;
				}
			}
			pos1 = false;
		}
		//@**
		else if( byHandMj[i] == byMj+1 && pos )
		{
			for( int j=i; j<HAND_MJ_MAX-1; j++ )
			{
				if( byHandMj[j] == byMj+2 )
				{
					chiInfo += 4;			//吃牌类型2
					break;
				}
			}
			pos = false;
		}
	}
	return chiInfo;
}

//是否可以碰牌
bool CGameLogic::IsPeng( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj )
{
	for( int i=HAND_MJ_MAX-byHandMjCount-1; i<HAND_MJ_MAX-1-1; i++ )
	{
		if( byHandMj[i]==byMj && byHandMj[i+1]== byMj )
		{
			return true;
		}
	}
	return false;
}

//是否明杠
MJ_GANGINFO CGameLogic::IsGang( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj )
{
	MJ_GANGINFO gangInfo;
	gangInfo.GangStyle= 0;				
	gangInfo.GangMj= -1;
	for( int i=HAND_MJ_MAX-byHandMjCount; i<HAND_MJ_MAX-3; i++ )
	{
		if( byHandMj[i]==byMj && byHandMj[i+1]==byMj && byHandMj[i+2]==byMj )
		{
			gangInfo.GangMj= byHandMj[i];
			gangInfo.GangStyle= 1;
		}
	}
	return gangInfo;
}

//自摸明杠 返回,
MJ_GANGINFO CGameLogic::IsZMGang( BYTE byHandMj[], BYTE byHandMjCount, HU hu,BYTE jPos1, BYTE jPos2 )
{
	MJ_GANGINFO gangInfo;
	gangInfo.GangStyle= 0;				
	gangInfo.GangMj= 0xff;
	for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
	{
		if( hu.block[i].style = BLOCK_NULL )
			break;
		else if( hu.block[i].style != BLOCK_PENG )
			continue;
		else
		{
			for( int j=HAND_MJ_MAX-byHandMjCount; j<HAND_MJ_MAX-1; j++ )
			{
				if( byHandMj[i]==jPos1 || byHandMj[i]==jPos2 )
					continue;
				if( hu.block[i].first == byHandMj[j] )
				{
					gangInfo.GangStyle= 2;					//自摸明杠
					gangInfo.GangMj= byHandMj[i];			//明杠胡牌块位
					return gangInfo;
				}
			}
		}
	}
	return gangInfo;

}
//是否暗杠
MJ_GANGINFO CGameLogic::IsAnGang( BYTE byHandMj[], BYTE byHandMjCount,BYTE jPos1, BYTE jPos2 )
{
	MJ_GANGINFO gangInfo;
	gangInfo.GangStyle= 0;				
	gangInfo.GangMj= 0xff;
	for( int i=HAND_MJ_MAX-byHandMjCount; i<HAND_MJ_MAX-3; i++ )
	{
		//跳过
		if( byHandMj[i]== jPos1 || byHandMj[i]==jPos2 )
			continue;
		if( byHandMj[i]==byHandMj[i+1] && byHandMj[i]==byHandMj[i+2] )
		{
			//手上有杠牌
			if( byHandMj[i]==byHandMj[i+3] )
			{
				gangInfo.GangStyle= 4;
				gangInfo.GangMj= byHandMj[i];
			}
			//刚摸到杠牌
			else if( byHandMj[i] == byHandMj[HAND_MJ_MAX-1])
			{
				gangInfo.GangStyle= 4;
				gangInfo.GangMj= byHandMj[i];
			}
		}
	}
	return gangInfo;
}

//是否可以胡牌
int CGameLogic::IsHu( BYTE byHandMj[], BYTE byHandMjCount, BYTE byMj, HU hu, HU *tempHu )
{
	//特殊胡
	if( byHandMjCount == HAND_MJ_MAX-1 )
	{
		if( IsNineLight( byHandMj, byMj ) ) return 20+9;		//九连灯,九番
		//else return false;
	}
	
	//排序牌面
	BYTE tempMj[HAND_MJ_MAX];
	for( int tag=1, i= HAND_MJ_MAX-1-byHandMjCount; i< HAND_MJ_MAX-1; i++ )
	{
		if( tag )
		{
			tempMj[i] = byHandMj[i];
			if( tag==1 && tempMj[i]>byMj )
			{
				tempMj[i+1] = tempMj[i];
				tempMj[i] = byMj;
				tag = 0;
			}
			else
			{
				if( i == HAND_MJ_MAX-1-1 )
				{
					tempMj[HAND_MJ_MAX-1] = byMj;
				}
			}
		}
		else
		{
			tempMj[i+1] = byHandMj[i];
		}
	}
	//特殊胡
	if( byHandMjCount == HAND_MJ_MAX-1 )
	{
		if( IsSpecialHu( tempMj ) ) return 20+13;				//十三幺,九番
		//else return false;
	}
	//将牌位在整数块( 0, 3, 6, 9, 12 )处
	//设置变量
	MJ_BLOCK block1;
	for( int i=HAND_MJ_MAX-1-byHandMjCount; i<HAND_MJ_MAX-1; i+=3 )
	{
		if( IsJiang( tempMj[i], tempMj[i+1] ) )
		{
			//胡牌结构处理 
			CleanHu( tempHu );
			CopyHu( tempHu, &hu );
			tempHu->jiangMj = tempMj[i];
			//单钓
			if( tempHu->block[(HAND_MJ_MAX-2)/3-1].style != BLOCK_NULL )
			{
				return 1;
			}
			for( int j=HAND_MJ_MAX-1-byHandMjCount; j<HAND_MJ_MAX-1-1; j+=3 )
			{
				if(  j==i )
				{
					if( i== HAND_MJ_MAX-1-1 )
					{
						return 1;
					}
					else
					{
						j += 2;
					}
				}

				block1 = CheckBlock( tempMj[j], tempMj[j+1], tempMj[j+2] );
				if( block1.style != BLOCK_NULL )
				{
					AddBlock( tempHu, &block1 );
				}
				else			
				{
					//有相嵌的两个BLOCK结构,或三个BLOCK结构
					if( j>i&&j+5<=HAND_MJ_MAX-1 || j<i-5 )
					{
						MJ_BLOCK block2;
						//两种情况  223344 或 233334
						//情况1   223344 
						block1 = CheckBlock( tempMj[j], tempMj[j+2], tempMj[j+4] );
						block2 = CheckBlock( tempMj[j+1], tempMj[j+3], tempMj[j+5] );
						if( block1.style != BLOCK_NULL  && block2.style != BLOCK_NULL )
						{
							AddBlock( tempHu, &block1 );
							AddBlock( tempHu, &block2 );
							j += 3;
						}
						//情况2 233334
						else
						{
							block1 = CheckBlock( tempMj[j], tempMj[j+1], tempMj[j+5] );
							block2 = CheckBlock( tempMj[j+2], tempMj[j+3], tempMj[j+4] );
							if( block1.style != BLOCK_NULL && block2.style != BLOCK_NULL )
							{
								AddBlock( tempHu, &block1 );
								AddBlock( tempHu, &block2 );
								j += 3;
							}
						}
						
					}	
					else
					{
						break;
					}
				}
				if( tempHu->block[(HAND_MJ_MAX-2)/3-1].style != BLOCK_NULL )
				{
					return 1;
				}
			}
		}
	}

	//将牌在其它位置( 1,4, 7, 10 ),( 2, 5, 8, 11 ), 两种算法相同
	//只算一种, 如：12223 将牌位置为1, 2
	//				12333
	for( int i=HAND_MJ_MAX-1-byHandMjCount+2; i< HAND_MJ_MAX-1; i+=3 )
	{
		if( IsJiang( tempMj[i], tempMj[i+1] ) )
		{
			CleanHu( tempHu );
			CopyHu( tempHu, &hu );
			block1 = CheckBlock( tempMj[i-2], tempMj[i-1], tempMj[i+2] );
			if( block1.style != BLOCK_NULL )
			{
				AddBlock( tempHu, &block1 );
				for( int j=HAND_MJ_MAX-1-byHandMjCount; j<HAND_MJ_MAX-1-1; j+=3 )
				{
					if( j == i-2 )
					{
						j+=5;				//跳过将牌区，5张牌
					}
					block1 = CheckBlock( tempMj[j], tempMj[j+1], tempMj[j+2] );
					if( block1.style != BLOCK_NULL )
					{
						AddBlock( tempHu, &block1 );
					}
					else
					{
						if( j>i&&j+5<=HAND_MJ_MAX-1 || j+5<i )
						{
							MJ_BLOCK block2;
							//两种情况
							block1 = CheckBlock( tempMj[j], tempMj[j+2], tempMj[j+4] );
							if( block1.style != BLOCK_NULL )
							{
								block2 = CheckBlock( tempMj[j+1], tempMj[j+3],tempMj[j+5] );
								if( block2.style != BLOCK_NULL )
								{
									AddBlock( tempHu, &block1 );
									AddBlock( tempHu, &block2 );
								}
							}

							else
							{
								block1 = CheckBlock( tempMj[j], tempMj[j+1], tempMj[j+5] );
								if( block1.style != BLOCK_NULL )
								{
									block2 = CheckBlock( tempMj[j+2], tempMj[j+3], tempMj[j+4] );
									if( block2.style != BLOCK_NULL )
									{
										AddBlock( tempHu, &block1 );
										AddBlock( tempHu, &block2 );
									}
								}
							}
						}
						else
						{
							break;
						}
					}
					if( tempHu->block[(HAND_MJ_MAX-2)/3-1].style != BLOCK_NULL )
					{
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

//特殊胡牌
bool CGameLogic::IsSpecialHu( BYTE tempMj[] )
{
	if( IsThirteenOne( tempMj ) ) return true;
	if( IsSevenPairs( tempMj ) )  return true;
	return false;
}

//十三幺
bool CGameLogic::IsThirteenOne( BYTE tempMj[] )
{
	//十三幺
	int thirteenJpos = 0;
	for( int i=0; i<13; i++ )
	{
		if( tempMj[i]==tempMj[i+1] )
		{
			thirteenJpos = i;
			for( int j=0; j<13; j++ )
			{
				if( j >= thirteenJpos )
				{
					if( tempMj[j] != mjThirteen[j] )
					{
						break;
					}
				}
				else
				{
					if( tempMj[j] != mjThirteen[j] )
					{
						break;
					}
				}
				if( j == 12 )
				{
					MessageBox( NULL, "十三幺", 0, 0 );
					return true;
				}
			}
			break;
		}
	}
	return false;
}

//七对子
bool CGameLogic::IsSevenPairs( BYTE tempMj[] )
{
	//七对
	int pair=0;
	for( int i=0; i<HAND_MJ_MAX-1; i+=2 )
	{
		if( tempMj[i]== tempMj[i+1] )
		{
			pair ++;
		}
		else
		{
			break;
		}
		if( pair == 7 )
		{
			//MessageBox( NULL, "七对子", "kk", MB_OK );
			return true;
		}
	}
	return false;
}

//九莲灯 +9番
bool CGameLogic::IsNineLight( BYTE tempMj[], BYTE byMj )
{
	BYTE mjType = GetMjType( tempMj[0]);
	for( int i=0; i<HAND_MJ_MAX-1; i++ )
	{
		if( GetMjType(tempMj[i]) != mjType)
			return false;
		if( GetMjValue(tempMj[i]) != mjNineLight[i] )
			return false;
	}
	return true;
}

/////////////////辅助函数/////////////////////////////////////////////////////////////////

//投骰子,随机数据
int CGameLogic::ActionDice()
{
	srand( GetTickCount() );
	int pos=rand()%6;
	return pos;
}
//左移麻将,0值向右,设摸牌区为0
bool CGameLogic::LMove( BYTE byMjs[], BYTE byRestCount )
{
	int start;
	if( byMjs[HAND_MJ_MAX-1] == 0 )
	{
		start = HAND_MJ_MAX-byRestCount-1;
	}
	else
	{
		start = HAND_MJ_MAX-byRestCount;
	}
	for( int i=start; i<HAND_MJ_MAX; i++ )
	{
		if( byMjs[i] == 0 )
		{
			//无无效牌,最后一张,不用移
			if( i == HAND_MJ_MAX-1 )
			{
				return true;
			}
			else
			{
				for( int j=i; j<HAND_MJ_MAX; j++ )
				{
					byMjs[j] = byMjs[j+1];
				}
				//末值设为0
				byMjs[HAND_MJ_MAX-1] = 0;
				return true;
			}
		}
	}
	return false;
}

//右移麻将, 0值左传
bool CGameLogic::RMove( BYTE byMjs[], BYTE byRestCount, BYTE byMoveCount )
{
	std::vector< BYTE > temp;
	int pos = 0;
	int start;
	if( byMjs[HAND_MJ_MAX-1] == 0 )
	{
		start = HAND_MJ_MAX-byRestCount-1;
	}
	else
	{
		start = HAND_MJ_MAX-byRestCount;
	}
	for( int i=start; i<HAND_MJ_MAX; i++ )
	{
		if( byMjs[i] != 0 )
		{
			temp.push_back( byMjs[i] );
		}
	}
	//清0
	for( int j=start; j<start+byMoveCount; j++ )
	{
		byMjs[j] = 0;
	}
	for( int k=start+byMoveCount; k<HAND_MJ_MAX; k++ )
	{
		if((pos+1) >(int)temp.size())
			return true;
		byMjs[k] = temp[pos++];
		if( k == HAND_MJ_MAX-1 )
			return true;
	}
	return false;
	//for( int i=HAND_MJ_MAX; i>byRestCount; i-- )
	//{
	//	if( byMjs[i] == 0 )
	//	{
	//		for( int j=i; j>pos; j-- )
	//		{
	//			byMjs[j] = byMjs[j-byMoveCount];
	//			if( byMjs[j] == 0 )
	//				break;
	//			if( j ==
	//			//
	//		}
	//	}
	//}
}

//插入麻将
void CGameLogic::InsertMjList( BYTE byMjList[], BYTE byRestCount  )
{
	int start;
	if( byRestCount == HAND_MJ_MAX )
	{
		start = HAND_MJ_MAX-byRestCount;
	}
	else
	{
		start = HAND_MJ_MAX-byRestCount-1;
	}
	//检测到倒数第二张牌,出牌后才有插入操作
	for( int i=start; i<HAND_MJ_MAX-2; i++ )
	{
		//倒数第二张牌不为最大
		if( byMjList[i] > byMjList[HAND_MJ_MAX-2] )
		{
			BYTE temp = byMjList[HAND_MJ_MAX-2];
			for( int j=HAND_MJ_MAX-2; j>i; j-- )
			{
				byMjList[j] = byMjList[j-1];
			}
			byMjList[i] = temp;
			break;
		}
	}
	//LMove( byMjList, HAND_MJ_MAX, byRestCount );
}

//麻将值与图位转换
bool CGameLogic::TransMjDToI( BYTE srcMjData, BYTE *desMjData )
{
	BYTE type,buffer;
	type = srcMjData&LOGIC_MASK_TYPE;
	switch( type )
	{
	case 0:				//万
		{
			buffer = srcMjData;
			CopyMemory( desMjData, &buffer, sizeof( buffer ) );

			return true;
		}		
	case 16:			//饼
		{
			buffer = 9+( srcMjData&LOGIC_MASK_VALUE );
			CopyMemory( desMjData, &buffer, sizeof( buffer ) );
			return true;
		}
	case 32:			//条
		{
			buffer = 18+( srcMjData&LOGIC_MASK_VALUE );
			CopyMemory( desMjData, &buffer, sizeof( buffer ) );
			return true;
		}		
	case 48:			//风
		{
			buffer = 27+( srcMjData&LOGIC_MASK_VALUE );
			CopyMemory( desMjData, &buffer, sizeof( buffer ) );
			return true;
		}
	case 64:			//字
		{
			buffer = 31+( srcMjData&LOGIC_MASK_VALUE );
			CopyMemory( desMjData, &buffer, sizeof( buffer ) );
			return true;
		}
	case 80:			//花
		{
			buffer = 34+( srcMjData&LOGIC_MASK_VALUE );
			CopyMemory( desMjData, &buffer, sizeof( buffer ) );
			//MessageBox( "补花" );
			return true;
		}
	}
	return false;
}

bool CGameLogic::TransMjDToI( BYTE srcMjData[], BYTE *desMjData[], BYTE desCount )
{
	for( int i=0; i<desCount; i++)
	{
		if( !TransMjDToI( srcMjData[i], desMjData[i] ) )
			return false;
	}
	return true;
}
//麻将值与图位转换
bool CGameLogic::TransMjIToD( BYTE srcMjData, BYTE *desMjData )
{
	BYTE buffer;
	if( srcMjData <= 9 )
	{
		buffer = srcMjData;
		CopyMemory( desMjData, &buffer, sizeof( buffer ) );
		return true;
	}
	else if( srcMjData <= 18 )
	{
		buffer = srcMjData-9+16;
		CopyMemory( desMjData, &buffer, sizeof( buffer ) );
		return true;
	}
	else if( srcMjData <= 27 )
	{
		buffer = srcMjData-18+32;
		CopyMemory( desMjData, &buffer, sizeof( buffer ) );
		return true;
	}
	else if( srcMjData <= 31 )
	{
		buffer = srcMjData-27+48;
		CopyMemory( desMjData, &buffer, sizeof( buffer ) );
		return true;
	}
	else if( srcMjData <= 34 )
	{
		buffer = srcMjData-31+64;
		CopyMemory( desMjData, &buffer, sizeof( buffer ) );
		return true;
	}
	else
	{
		buffer = srcMjData-34+80;
		CopyMemory( desMjData, &buffer, sizeof( buffer ) );
		return true;
	}
	return false;
}

//番计算
long CGameLogic::Pow( long x, long y )
{
	long r = 1;
	for( int i=0; i<y; i++ )
	{ 
		r *= x;
	}
	return r;
}
//门风计算
BYTE CGameLogic::TransIDToFeng( WORD wChairID, WORD wBanker )
{
	BYTE feng= (BYTE)(wChairID+wBanker);
	if( feng>GAME_PLAYER-1)
	{
		feng -= 4; 
	}
	return feng;
}
//加入块到胡牌结构
void CGameLogic::AddBlock( HU *hu, PTR_MJ_BLOCK block )
{
	for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
	{
		if( hu->block[i].style == BLOCK_NULL )
		{
			hu->block[i].first = block->first;
			hu->block[i].style = block->style;
			break;
		}
	}
}

//清除胡牌数据
void CGameLogic::CleanHu( HU *hu )
{
	hu->jiangMj = 0;
	for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
	{
		hu->block[i].first = 0;
		hu->block[i].style = BLOCK_NULL;
	}
}

//拷贝胡牌数据
void CGameLogic::CopyHu( HU *destHu, HU *srcHu )
{
	destHu->jiangMj = srcHu->jiangMj;
	for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
	{
		destHu->block[i].first = srcHu->block[i].first;
		destHu->block[i].style = srcHu->block[i].style;
	}
}

//分析胡牌类型,返回番数
long CGameLogic::CheckGDHu( HU hu, BYTE byFengWei, BYTE byQuanWei, char *strFang)
{
	memset( strFang, 0, strlen(strFang));
	long nFangCount = 0;
	int nSanYuanCount ;			//三元牌数
	int nFengPaiCount ;			//风牌数
	int nKePaiCount;			//刻子数， 值为四，为对对碰
	bool bSameMjType = true;	//清一色可能
	BYTE byMjType = -1;			//同色麻将类型

	//for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
	{
		//将牌为风字
		if( GetMjType( hu.jiangMj )== 3 )
		{
			nFengPaiCount = 0;		//风牌数
			nSanYuanCount = 0;
			nKePaiCount = 0;
			//字一色,小四喜、大三元、混一色,鸡胡、平和、对对碰,混碰可能
			for( int j=0; j<(HAND_MJ_MAX-2)/3; j++ )
			{	
				//字牌
				if( hu.block[j].first>0x30 )
				{
					nKePaiCount += 1;
					//三元牌+1番
					if( hu.block[j].first > 0x34 )
					{
						nSanYuanCount += 1;
						nFangCount += 1;
						strcat( strFang, "三元牌  +1番     ");
					}
					//风位、圈位，+1番
					else if( GetMjValue(hu.block[j].first)==byFengWei+1)
					{
						nFengPaiCount += 1;
						nFangCount += 1;
						strcat( strFang, "门风牌 +1番     ");
						if( GetMjValue(hu.block[j].first)== byQuanWei+1)
						{
							nFangCount += 1;
							strcat( strFang, "圈风牌 +1番     ");
						}
					}
					else if( GetMjValue(hu.block[j].first)==byQuanWei+1)
					{
						nFengPaiCount += 1;
						nFangCount += 1;
						strcat( strFang, "圈风牌 +1番     ");
						if( hu.block[j].first==byFengWei+1)
						{
							nFangCount += 1;
							strcat( strFang, "门风牌 +1番     ");
						}
					}
					else
					{
						nFengPaiCount += 1;
					}
				}
				//混一色,鸡胡可能,平和可能
				else if( hu.block[j].style<BLOCK_PENG )
				{
					if( byMjType == -1)
					{
						byMjType = GetMjType(hu.block[j].first);
					}
					else
					{
						if( GetMjType(hu.block[j].first)!=byMjType)
						{
							//清一色/混一色否定
							bSameMjType = false;
						}
					}
				}
				//对对碰,混碰可能
				else
				{
					nKePaiCount += 1;
					if( byMjType == -1)
					{
						byMjType = GetMjType(hu.block[j].first);
					}
					else
					{
						if( GetMjType(hu.block[j].first)!=byMjType)
						{
							//清一色/混一色否定
							bSameMjType = false;
						}
					}
				}
			}	
			//大三元, 9番
			if( nSanYuanCount == 3)
			{
				nFangCount = 9;
				strcpy( strFang, "大三元  +9番     ");
				return nFangCount;
			}
			//字一色,	8番
			if( nSanYuanCount+nFengPaiCount == 4 )
			{
				nFangCount = 8;
				strcpy( strFang, "字一色 +8番      ");
				return nFangCount;
			}
			//小四喜, 7番
			if( nFengPaiCount== 3 )
			{
				//小四喜牌, 7番
				nFangCount = 7;
				strcpy( strFang, "小四喜 +7番     ");
				return nFangCount;
			}
			//其它小胡类型：混碰、对碰、混一色、平和、鸡胡
			//碰优先
			if( nKePaiCount==4 )
			{
				//混碰, +5番
				if( bSameMjType==true )
				{
					nFangCount += 5;
					strcat( strFang, "混一色对对碰 +5番     ");
				}
				//对对碰
				else
				{
					nFangCount += 3;
					strcat( strFang, "对对碰 +3番     ");
				}
			}
			else
			{	
				//平和
				if( nKePaiCount==0 )
				{
					nFangCount += 1;
					strcat( strFang, "平和  +1番     " );
				}
				//鸡胡
				else
				{
					if( nFangCount== 0 )
						strcpy( strFang, "鸡胡 +0番     " );
				}
			}
		}
		//将牌为三元牌
		if( GetMjType( hu.jiangMj ) == 4 )
		{
			nSanYuanCount = 0;	//三元牌数
			nFengPaiCount = 0;	//风牌
			nKePaiCount   = 0;
			bSameMjType   = true;
			byMjType	  = -1;
			//字一色, 小三元可能
			 for( int j=0; j<(HAND_MJ_MAX-2)/3; j++)
			 {
				 //字牌
				 if( hu.block[j].first>0x30 )
				 {
					 nKePaiCount += 1;
					 //三元牌+1番
					 if( hu.block[j].first > 0x34 )
					 {
						 nSanYuanCount += 1;
						 nFangCount += 1;
						 strcat( strFang, "三元牌 +1番     " );
					 }
					 //风位、圈位，+1番
					 else if( GetMjValue(hu.block[j].first)==byFengWei+1)
					 {
						 nFengPaiCount += 1;
						 nFangCount += 1;
						 strcat( strFang, "门风牌 +1番     ");
						 if( GetMjValue(hu.block[j].first)== byQuanWei+1)
						 {
							 nFangCount += 1;
							 strcat( strFang, "圈风牌 +1番     ");
						 }
					 }
					 else if( GetMjValue(hu.block[j].first)==byQuanWei+1)
					 {
						 nFengPaiCount += 1;
						 nFangCount += 1;
						 strcat( strFang, "圈风牌 +1番     ");
						 if( GetMjValue(hu.block[j].first)==byFengWei+1)
						 {
							 nFangCount += 1;
							 strcat( strFang, "门风牌 +1番     ");
						 }
					 }
					 else
					 {
						 nFengPaiCount += 1;
					 }
					 ////风位、圈位，+1番
					 //if( hu.block[j].first==byFengWei || hu.block[j].first==byQuanWei )
					 //{
						// nFangCount += 1;
						// //既是圈位，又是风位，再加一番
						// if( hu.block[j].first==byFengWei&&hu.block[j].first==byQuanWei)
						// {
						//	 nFangCount += 1;
						// }
					 //}
				 }
				 //混一色,鸡胡可能,平和可能
				 else if( hu.block[j].style<BLOCK_PENG )
				 {
					 if( byMjType == -1)
					 {
						 byMjType = GetMjType(hu.block[j].first);
					 }
					 else
					 {
						 if( GetMjType(hu.block[j].first)!=byMjType)
						 {
							 //清一色/混一色否定
							 bSameMjType = false;
						 }
					 }
				 }
				 //对对碰,混碰可能
				 else
				 {
					 nKePaiCount += 1;
					 if( byMjType == -1)
					 {
						 byMjType = GetMjType(hu.block[j].first);
					 }
					 else
					 {
						 if( GetMjType(hu.block[j].first)!=byMjType)
						 {
							 //清一色/混一色否定
							 bSameMjType = false;
						 }
					 }
				 }
			 }
			 //大四喜, 9番
			 if( nFengPaiCount==4 )
			 {
				 nFangCount = 9;
				 strcpy( strFang, "大四喜 +9番     " );
				 return nFangCount;
			 }
			 //字一色, 8番
			 if( nFengPaiCount+nSanYuanCount==4)
			 {
				 nFangCount = 8;
				 strcpy( strFang, "字一色 +8番     " );
				 return nFangCount;
			 }
			 //小三元, 7番
			 if( nSanYuanCount==2)
			 {
				 nFangCount = 7;
				 strcpy( strFang, "小三元 +7番     " );
				 return nFangCount;
			 }
			 //其它小胡类型：混碰、对碰、混一色、平和、鸡胡
			 //碰优先
			 if( nKePaiCount==4 )
			 {
				 //混碰, +5番
				 if( bSameMjType==true )
				 {
					 nFangCount += 5;
					 strcat( strFang, "混一色对对碰 +5番     " );
				 }
				 //对对碰, +3番
				 else
				 {
					 nFangCount += 3;
					 strcat( strFang, "对对碰 +3番     " );
				 }
			 }
			 else
			 {
				if( nKePaiCount==0 )
				{
					nFangCount += 1;
					strcat( strFang, "平和 +1番     " );
				}
				//鸡胡
				else
				{
					if( nFangCount== 0 )
						strcpy( strFang, "鸡胡 +0番     " );
				}
			 }
			 //平和, +1番

			 ////混一色
			 //for( int k=0; k<(HAND_MJ_MAX-2)/3-1; k++ )
			 //{
				// if( GetMjType(hu.block[k].first) != GetMjType(hu.block[k].first) )
				//	 break;
				// else
				// {
				//	 //混一色
				//	 if( k == (HAND_MJ_MAX-2)/3-2)
				//	 {
				//		 //是否混碰
				//		 for( int l=0; l<(HAND_MJ_MAX-2)/3; l++)
				//		 {
				//			 if( hu.block[l].style<BLOCK_PENG ) 
				//				 break;
				//			 else
				//			 {
				//				 //混一色对对碰
				//				 if( l == (HAND_MJ_MAX-2)/3-2)
				//				 {
				//					 nFangCount = 5;
				//					 return nFangCount;
				//				 }
				//			 }
				//		 }
				//		 //混一色牌  3番
				//		 nFangCount += 3;
				//	 }
				// }
			 //}
			 ////对对碰
			 //for( int j=0; j<(HAND_MJ_MAX-2)/3; j++ )
			 //{
				// if( hu.block[j].style<BLOCK_PENG )
				// {
				//	 break;
				// }
				// else
				// {
				//	 //对对碰
				//	 if( j==(HAND_MJ_MAX-2)/3-1 )
				//	 {
				//		 nFangCount += 3;
				//	 }
				// }
			 //}
		}
		////幺九牌
		//if( GetMjValue(hu.jiangMj)==1 || GetMjValue(hu.jiangMj)==9 )
		//{
		//	//
		//	for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
		//	{
		//		//不为碰,杠,跳出
		//		if(hu.block[i].style<BLOCK_PENG)
		//		{
		//			break;
		//		}
		//		//不为幺九,跳出
		//		else if( GetMjValue(hu.block[i].first)!=1 && hu.block[i].first!=9 )
		//		{
		//			break;
		//		}
		//		else
		//		{
		//			//幺九牌
		//			if( i == (HAND_MJ_MAX-2)/3-1)
		//			{
		//				for( int j=0; j<(HAND_MJ_MAX-2)/3-1; j++ )
		//				{
		//					//同一花色
		//					if( GetMjType(hu.block[j].first)!=GetMjType(hu.block[j].first) )
		//					{
		//						break;
		//					}
		//					//清幺九牌,8番
		//					if( j==(HAND_MJ_MAX-2)/3-2)
		//					{
		//						nFangCount = 8;
		//						return nFangCount;
		//					}
		//				}
		//				//混幺九牌, 6番
		//				nFangCount = 6;
		//				return nFangCount;
		//			}
		//		}
		//	}
		//}
		//其它类型	
		else
		{	
			//重新始化变量
			nSanYuanCount = 0;	//三元牌数
			nFengPaiCount = 0;	//风牌
			nKePaiCount   = 0;	//刻牌
			byMjType = GetMjType( hu.jiangMj );
			bSameMjType = true;
			//幺九牌
			if( GetMjValue(hu.jiangMj)==1 || GetMjValue(hu.jiangMj)==9 )
			{
				//
				for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
				{
					//不为碰,杠,跳出
					if(hu.block[i].style<BLOCK_PENG)
					{
						break;
					}
					//不为幺九,跳出
					else if( GetMjValue(hu.block[i].first)!=1 && hu.block[i].first!=9 )
					{
						break;
					}
					else
					{
						//幺九牌
						if( i == (HAND_MJ_MAX-2)/3-1)
						{
							for( int j=0; j<(HAND_MJ_MAX-2)/3; j++ )
							{
								//同一花色
								if( GetMjType(hu.block[j].first)!=byMjType )
								{
									break;
								}
								//清幺九牌,8番
								if( j==(HAND_MJ_MAX-2)/3-1)
								{
									nFangCount = 8;
									strcpy( strFang, "清幺九 +8番     " );
									return nFangCount;
								}
							}
							//混幺九牌, 6番
							nFangCount = 6;
							strcpy( strFang, "混幺九 +6番     " );
							return nFangCount;
						}
					}
				}
			}

			//大四喜、大三元、清一色，清碰、混一色、混碰、鸡胡、平和、对对碰,混碰可能
			for( int j=0; j<(HAND_MJ_MAX-2)/3; j++ )
			{	
				//字牌
				if( hu.block[j].first>0x30 )
				{
					nKePaiCount += 1;
					//三元牌+1番
					if( hu.block[j].first > 0x34 )
					{
						nSanYuanCount += 1;
						nFangCount += 1;
						strcat( strFang, "三元牌 +1番     " );
					}
					//风位、圈位，+1番
					else if( GetMjValue(hu.block[j].first)==byFengWei+1)
					{
						nFengPaiCount += 1;
						nFangCount += 1;
						strcat( strFang, "门风牌 +1番     ");
						if( GetMjValue(hu.block[j].first)== byQuanWei+1)
						{
							nFangCount += 1;
							strcat( strFang, "圈风牌 +1番     ");
						}
					}
					else if( GetMjValue(hu.block[j].first)==byQuanWei+1)
					{
						nFengPaiCount += 1;
						nFangCount += 1;
						strcat( strFang, "圈风牌 +1番     ");
						if( GetMjValue(hu.block[j].first)==byFengWei+1)
						{
							nFangCount += 1;
							strcat( strFang, "门风牌 +1番     ");
						}
					}
					else
					{
						nFengPaiCount += 1;
					}
					////风位、圈位，+1番
					//if( hu.block[j].first==byFengWei || hu.block[j].first==byQuanWei )
					//{
					//	nFangCount += 1;
					//	//既是圈位，又是风位，再加一番
					//	if( hu.block[j].first==byFengWei&&hu.block[j].first==byQuanWei)
					//	{
					//		nFangCount += 1;
					//	}
					//}
				}
				//混一色,鸡胡可能,平和可能
				else if( hu.block[j].style<BLOCK_PENG )
				{
					if( GetMjType(hu.block[j].first)!=byMjType)
					{
						//清一色/混一色否定
						bSameMjType = false;
					}
				}
				//对对碰,混碰可能
				else
				{
					nKePaiCount += 1;
					if( GetMjType(hu.block[j].first)!=byMjType)
					{
						//清一色/混一色否定
						bSameMjType = false;
					}
				}
			}	
			//大三元, 9番
			if( nSanYuanCount == 3)
			{
				nFangCount = 9;
				strcpy( strFang, "大三元 +9番     " );
				return nFangCount;
			}
			//大四喜, 9番
			if( nFengPaiCount==4)
			{
				nFangCount = 9;
				strcpy( strFang, "大四喜 +9番     " );
				return nFangCount;
			}
			//清一色，清碰、混一色、混碰
			if( bSameMjType== true)
			{
				//清一
				if( nFengPaiCount==0 && nSanYuanCount==0)
				{
					//清碰， 6番
					if( nKePaiCount==4)
					{
						nFangCount = 6;
						strcpy( strFang, "清一色对对碰 +6番     " );
						return nFangCount;
					}
					//清一色，+5番
					else
					{
						nFangCount += 5;
						strcat( strFang, "清一色 +5番     " );
					}
				}
				//混一
				else
				{
					//混碰,+5番
					if( nKePaiCount== 4 )
					{
						nFangCount += 5;
						strcat( strFang, "混一色对对碰 +5番     " );
					}
					//混一色, +3番
					else
					{
						nFangCount += 3;
						strcat( strFang, "混一色 +3番     " );
					}
				}
			}
			else 
			{
				//碰和， +3番
				if( nKePaiCount==4 )
				{
					nFangCount += 3;
					strcat( strFang, "对对碰 +3番     " );
				}
				else
				{
					//平和，+1番
					if( nKePaiCount==0 )
					{
						nFangCount += 1;
						strcat( strFang, "平和 +1番     " );
					}
					//鸡胡，0
					else
					{
						if( nFangCount== 0 )
							strcpy( strFang, "鸡胡 +0番     " );
					}
				}
			}

		//	//全碰：大四喜、大三元、清碰、混碰、碰和	
		//	for( int i=0; i<(HAND_MJ_MAX-2)/3; i++ )
		//	{
		//		if( hu.block[i].style < BLOCK_PENG )
		//		{
		//			break;
		//		}
		//		else
		//		{
		//			//字牌
		//			if( GetMjValue( hu.block[i].first ) > 0x30 )
		//			{
		//				if( GetMjValue(hu.block[i].first)>0x40 )
		//				{
		//					nSanYuanCount += 1;
		//				}
		//				else
		//				{
		//					nFengPaiCount += 1;
		//				}
		//			}
		//			else
		//			{
		//				//普通对对碰
		//				if( bSameMjType && GetMjType( hu.block[i].first) != byMjType)
		//				{
		//					bSameMjType = false;
		//				}
		//			}
		//		}
		//		if( i == (HAND_MJ_MAX-2)/3 )
		//		{
		//			//大四喜, 九番
		//			if( nFengPaiCount == 4)
		//			{
		//				nFangCount = 9;
		//				return nFangCount;
		//			}
		//			//大三元， 9番
		//			if( nSanYuanCount == 3 )
		//			{
		//				nFangCount = 9; 
		//				return nFangCount;
		//			}
		//			//清碰, 混碰
		//			if( bSameMjType )
		//			{
		//				//清一色对对碰, 6番
		//				if( nFengPaiCount== 0 && nSanYuanCount==0 )
		//				{
		//					nFangCount = 6;
		//					return nFangCount;
		//				}
		//				//混一色对对碰， 5番
		//				else
		//				{
		//					nFangCount +=5;
		//				}
		//			}
		//			//对对碰， 3番
		//			else
		//			{
		//				nFangCount += 3;
		//			}
		//		}
		//	}
		//}
		}
	}
	
	if( nFangCount >5 )
	{
		strcat( strFang, "     暴胡   ");
		nFangCount = 5;
	}
	return nFangCount;
}
////////////////////////////////操作
BLOCK CGameLogic::Peng( BYTE byMjs[], BYTE byRestCount, BYTE byMj )
{
	MJ_BLOCK block;
	for( int i=HAND_MJ_MAX-1-byRestCount; i<HAND_MJ_MAX-1; i++ )
	{
		if( byMjs[i] == byMj )
		{
			byMjs[i] = 0;
			byMjs[i+1] = 0;
			//右移3格
			break;
		}
	}
	block.first = byMj;
	block.style = BLOCK_PENG;
	return block;
}

//杠操作
BLOCK CGameLogic::MGang( BYTE byMjs[], BYTE byRestCount, BYTE byMj )
{
	BLOCK block;
	block.style= BLOCK_NULL;
	block.first= -1;
	for( int i=HAND_MJ_MAX-1-byRestCount; i<HAND_MJ_MAX-3; i++ )
	{
		if( byMjs[i] == byMj )
		{
			byMjs[i] = 0;
			byMjs[i+1] = 0;
			byMjs[i+2] = 0;
			block.style= BLOCK_GANG;
			block.first= byMj;
			//右移4格
			return block;
		}
	}
	return block;
}

//自摸明杠-补杠
void CGameLogic::ZMGang( BYTE byMjs[], BYTE byRestCount, HU *hu, BYTE GangMj )
{
	for( int i=0; i<(HAND_MJ_MAX-2)/3; i++)
	{
		if( hu->block[i].first== GangMj)
		{
			hu->block[i].style= BLOCK_GANG;
			break;
			//右移一格
		}
	}
	for( int j=0; j<HAND_MJ_MAX-1; j++ )
	{
		if( byMjs[j] == GangMj)
		{
			byMjs[j] = 0;
			break;
		}
	}
}

//暗杠
BLOCK CGameLogic::AnGang( BYTE byMjs[], BYTE byRestCount, BYTE GangMj )
{
	BLOCK block;
	for( int i=HAND_MJ_MAX-byRestCount; i<HAND_MJ_MAX-1; i++ )
	{
		if( byMjs[i] == GangMj)
		{
			byMjs[i] = 0;
		}
	}
	block.style= BLOCK_AN_GANG;
	block.first= GangMj;
	return block;
}
//吃牌操作
BLOCK CGameLogic::Chi( BYTE byMjs[], BYTE byRestCount, BYTE byMj, int ChiMode )
{
	MJ_BLOCK block;
	block.first = 0;
	block.style = BLOCK_NULL;
	switch( ChiMode )
	{
	case 1:				//@@*
		{
			block.first = byMj-2;
			block.style = BLOCK_CHI;
			//清零麻将
			for( int i=HAND_MJ_MAX-1-byRestCount; i<HAND_MJ_MAX-1; i++ )
			{
				if( byMjs[i] == byMj-2 )
				{
					byMjs[i] = 0;
					for( int j=i; j<HAND_MJ_MAX-1; j++)
					{
						if( byMjs[j]==byMj-1)
						{
							byMjs[j] = 0;
							return block;
						}
					}
					break;
				}
			}
		}
		break;
	case 2:					//@*@
		{
			block.first = byMj-1;
			block.style = BLOCK_CHI;
			//清零麻将
			for( int i=HAND_MJ_MAX-1-byRestCount; i<HAND_MJ_MAX-1; i++ )
			{
				if( byMjs[i] == byMj-1 )
				{
					byMjs[i] = 0;
					for( int j=i; j<HAND_MJ_MAX-1; j++)
					{
						if( byMjs[j]==byMj+1)
						{
							byMjs[j] = 0;
							return block;
						}
					}
					break;
				}
			}
		}
		break;
	case 4:				//*@@
		{
			block.first = byMj;
			block.style = BLOCK_CHI;
			//清零麻将
			for( int i=HAND_MJ_MAX-1-byRestCount; i<HAND_MJ_MAX-1; i++ )
			{
				if( byMjs[i] == byMj+1 )
				{
					byMjs[i] = 0;
					for( int j=i; j<HAND_MJ_MAX-1; j++)
					{
						if( byMjs[j]==byMj+2 )
						{
							byMjs[j] = 0;
							return block;
						}
					}
					break;
				}
			}
		}
		break;
	}
	return block;
}
