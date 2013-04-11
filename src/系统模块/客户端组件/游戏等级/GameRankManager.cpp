
#include "StdAfx.h"
#include "GameRankManager.h"


typedef LPCTSTR (*PRANK)(LONG lScore);


CGameRankManager::CGameRankManager(void)
{
}

CGameRankManager::~CGameRankManager(void)
{
}

//
//接口查询
//
void * __cdecl CGameRankManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameRankManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameRankManager,Guid,dwQueryVer);
	return NULL;
}


//kindid
//1			3		梭哈游戏	ShowHand.exe	196609	100	1
//2			1		拱猪游戏	UncoverPig.exe	196610	100	1
//3			3		升级游戏	Upgrade.exe		196610	100	1
//4			3		斗地主游戏	Lord.exe		196610	100	1
//5			3		挖坑游戏	Cavern.exe		196610	100	1
//6			3		锄大地游戏	Hoe.exe	1		96610	100	1
//7			4		中国象棋	CChess.exe		196610	100	1
//8			3		色盅游戏	SeZhong.exe		199610	100	1
//9			4		四国军棋	FourEnsign.exe	196610	100	1
//10		3		跑得快游戏	RunFast.exe		199610	100	1
//22        4		围棋		Go.exe			

//1 	4	港式五张牌	ShowHand.exe	196611
//2	    2	拱猪游戏	UncoverPig.exe	196614
//3	    2	升级游戏	Upgrade.exe	196614
//4	    2	斗地主游戏	Lord.exe	196614
//5	    2	挖坑游戏	Cavern.exe	196611
//6	    2	锄大地游戏	Hoe.exe	196614
//7	    1	中国象棋	CChess.exe	196613
//9	    1	四国军棋	FourEnsign.exe	196614
//10	2	跑得快游戏	RunFast.exe	196610
//11	1	麻将游戏	Mjsx.exe	196617
//12	1	国际象棋	Chess.exe	196609
//13	1	五子棋游戏	GoBang.exe	196610
//15	1	斗兽棋	        DouShouQi.exe	196610
//16	2	21点游戏	BJ.exe	196610
//17	4	色蛊游戏	SeZhong.exe	196612
//20	4	飘三叶游戏	ThreeLeaf.exe	196610
//105	6	挖坑游戏	Cavern.exe	196611
//111	4	麻将游戏	Mjsx.exe	196617
				
//
//获取等级
//

//
//金币类型
//

LPCTSTR __cdecl CGameRankManager::GetGameRank(WORD wKindID, 
											  LONG lScore, 
											  WORD wGameGenre)
{

	if (wGameGenre==GAME_GENRE_GOLD)
	{
		return GetCurrencyRank(lScore);
	}
	else 
	{
		switch (wKindID)
		{
		case 2:
			{
				return GetCChessRank(lScore);
			}
		case 3:
			{
				return GetFourEnsignRank(lScore);
			}
		case 4:
			{
				return GetCChessRank(lScore);
			}
		case 5:
			{
				return GetDoShouQiRank(lScore);
			}
		case 6:
			{
				return GetLanddRank(lScore);

			}
		case 7:
			{
				return GetGoRank(lScore);

			}
		case 8:
			{
				return GetSparrowCSRank(lScore);

			}
		case 9:
			{
				return GetSanDaHaRank(lScore);
			}
		case 10:
			{
				return GetTFZGameRank(lScore);
			}
		default:
			{
				return GetGoRank(lScore);
			}
		}
	}

	return NULL;
}

LPCTSTR __cdecl CGameRankManager::GetCurrencyRank(LONG lScore)
{
/*
	LONG lScoreInfo[] = 	{	99L,		199L,		499L,		1499L,	3499L,		7499L,		15499L,		47499L,	111499L,	239499L,	495499L,	1007499L,	2031499L,	4063499L,	6563499L,	9565499L,	13563499L,	23563499L,	38563499L,	58563499L,	83563499L,	93563499L,	103563499L	};	LPCTSTR pszOrderName[] =	{		_T("蓄储员"),		_T("蓄储所副组长"),	_T("蓄储所组长"),	_T("蓄储所副所长"),		_T("蓄储所长"),		_T("支行副科长"),	_T("支行科长"),		_T("支行副行长"),		_T("支行行长"),		_T("分行副处长"),	_T("分行处长"),		_T("分行副行长"),		_T("分行行长"),		_T("省行副处长"),	_T("省行处长"),		_T("省行财务总监"),		_T("省行副行长"),	_T("省行行长"),		_T("总行副部长"),	_T("总行部长"),		_T("总行财务总监"),	_T("总行副行长"),	_T("总行行长")	};	for(int i=0; i<23; i++)		{		if(lScore<lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return _T("财政部长");
*/
/*
梭哈
分数

<=2000
2000		-	3999
4000		-	7999
8000		-	19999

20000		-	39999
40000		-	79999
80000		-	149999
150000		-	299999

300000		-	499999
500000		-	999999
1000000		-	1999999
2000000		-	4999999

5000000		-	9999999
10000000	-	49999999
50000000	-	99999999
100000000	-	499999999

500000000	-	999999999
10亿		-	4999999999 
50亿		-	9999999999
100亿以上
*/
	static __int64 lScoreInfo[19] = 	{	2000L,		4000,		8000,		20000,		
	40000L,		80000L,		150000L,		300000L,		
	500000L,		1000000L,	2000000L,	5000000L,	
	10000000L,	50000000L,	100000000L,	500000000L,	
	1000000000L,	5000000000L,	10000000000L
	};	LPCTSTR pszOrderName[20] = 	{	_T("务农"),		_T("佃户"),		_T("雇工"),		_T("作坊主"),	_T("农场主"),	_T("地主"),		_T("大地主"),	_T("土豪"),	_T("财主"),		_T("大财主"),	_T("富翁"),		_T("大富翁"),	_T("小财神"),	_T("大财神"),	_T("赌棍"),		_T("赌侠"),	_T("赌王"),		_T("赌圣"),		_T("赌神"),		_T("职业赌神")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

LPCTSTR __cdecl CGameRankManager::GetShowHandRank(LONG lScore)
{
	static __int64 lScoreInfo[19] = 	{	2000L,		4000,		8000,		20000,		
	40000L,		80000L,		150000L,		300000L,		
	500000L,		1000000L,	2000000L,	5000000L,	
	10000000L,	50000000L,	100000000L,	500000000L,	
	1000000000L,	5000000000L,	10000000000L
	};	LPCTSTR pszOrderName[20] = 	{	_T("务农"),		_T("佃户"),		_T("雇工"),		_T("作坊主"),	_T("农场主"),	_T("地主"),		_T("大地主"),	_T("土豪"),	_T("财主"),		_T("大财主"),	_T("富翁"),		_T("大富翁"),	_T("小财神"),	_T("大财神"),	_T("赌棍"),		_T("赌侠"),	_T("赌王"),		_T("赌圣"),		_T("赌神"),		_T("职业赌神")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

//
//240 分
//
LPCTSTR __cdecl CGameRankManager::GetTFZGameRank(LONG lScore)
{
	static __int64 lScoreInfo[27] = 	{	51L,		101L,		151L,		251,		
	401,		651,		1151,		1801,		
	2951,		4751,		7701,		12451,	
	20151,		32601L,		52751L,		85351L,	
	138101L,		223451L,		361551L,		585001L,
	946551L,		1431551L,	2378101L,	3809651L,
	6187751L,	9997401L,	16185151L
	};	LPCTSTR pszOrderName[28] = 	{	_T("待业"),		_T("科员"),		_T("科长助理"),	_T("副科长"),	_T("第一副科"),	_T("正科长"),	_T("处长助理"),	_T("副处长"),	_T("第一副处"),	_T("正处长"),	_T("局长助理"),	_T("副局长"),	_T("第一副局"),	_T("正局长"),	_T("厅长助理"),	_T("副厅长"),	_T("第一副厅"),	_T("正厅长"),	_T("部长助理"),	_T("副部长"),	_T("第一副部"),	_T("正部长"),	_T("总理助理"),_T("副总理"),	_T("第一副总理"),_T("总理"),	_T("副主席"),	_T("主席")	};	for(int i=0; i<27; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[27];
}

//
//三打哈
//

LPCTSTR __cdecl CGameRankManager::GetSanDaHaRank(LONG lScore)
{
	static __int64 lScoreInfo[19] = 	{	500	,		501,		1001,		2001,		
	3001,		4001,		5001,		7501,		
	10001,		15001,		20001,		25001,	
	30001,		40001L,		60001L,		80001L,	
	100001L,		150001L,		200001L
	};	LPCTSTR pszOrderName[20] = 	{	_T("平民"),		_T("从九品巡检"),		_T("九品主簿"),		_T("从八品训导"),	_T("八品县丞"),	_T("从七品州判"),		_T("七品知县"),		_T("从六品州同"),	_T("六品通判"),	_T("从五品知州"),		_T("五品同知"),		_T("从四品知府"),	_T("四品道员"),	_T("从三品盐运使"),		_T("三品按察使"),	_T("从二品巡抚"),	_T("二品总督"),	_T("从一品都御史"),		_T("一品大学士"),	_T("三朝元老")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}


//
//长沙麻将
//
LPCTSTR __cdecl CGameRankManager::GetSparrowCSRank(LONG lScore)
{
	static __int64 lScoreInfo[16] = 	{	100L,		200,		500,		1000,		
	2000,		5000,		10000,		20000,		
	30000,		70000L,		100000L,		200000L,	
	500000L,		700000L,		1000000L,	2000000L,	
	};	LPCTSTR pszOrderName[17] = 	{	_T("初品豆子"),	_T("良品豆子"),		_T("上品豆子"),		_T("初品豆浆"),	_T("良品豆浆"),	_T("上品豆浆"),		_T("初品豆渣"),		_T("良品豆渣"),	_T("上品豆渣"),	_T("良品豆花"),		_T("上品豆花"),		_T("初品豆腐"),	_T("良品豆腐"),	_T("上品豆腐"),		_T("臭豆腐"),		_T("火宫殿臭豆腐"),	_T("长沙臭豆腐")	};	for(int i=0; i<16; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}


//
//拱猪
//
LPCTSTR __cdecl CGameRankManager::GetUncoverPigRank(LONG lScore)
{
/*
拱猪
分数
<=-5000000
-4999999	-	-1000000
-999999		-	-200000
-199999		-	-100000
-99999		-	0
1			-	4999
5000		-	19999
20000		-	59999
60000		-	99999
100000		-	149999
150000		-	239999
240000		-	379999
380000		-	549999
550000		-	699999
700000		-	999999
1000000		-	2799999
2800000		-	4999999
5000000		-	9999999
10000000	-	19999999
20000000以上
*/
	LONG lScoreInfo[19] = 	{	-5000000,	-1000000,	-200000,	-100000,
	0,			5000,		20000,		60000,
	100000,		150000,		240000,		380000,
	550000,		700000,		1000000,	2800000,
	5000000,	10000000,	20000000	};	LPCTSTR pszOrderName[20] = 	{	_T("世界猪圣"),		_T("国家猪神"),	_T("地区猪王"),		_T("农场猪王"),	_T("猪小弟"),		_T("小猪倌"),	_T("业余饲养员"),	_T("优秀饲养员"),	_T("养猪专业户"),	_T("养猪标兵"),	_T("养猪状元"),		_T("养猪专家"),	_T("养猪大王"),		_T("农场组长"),	_T("农场经理"),		_T("小农场主"),	_T("大农场主"),		_T("小资本家"),	_T("大资本家"),		_T("富甲一方")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

//
//升级
//
LPCTSTR __cdecl CGameRankManager::GetUpgradeRank(LONG lScore)
{
/*
升级
分数

<=0
1		-	29
30		-	69
70		-	119

120		-	179
180		-	259
260		-	319
320		-	399

400		-	599
600		-	799
800		-	999
1000	-	1199

1200	-	1499
1500	-	1999
2000	-	2799
2800	-	3999

4000	-	7999
8000	-	14999
15000	-	29999
3万分以上
*/
	LONG lScoreInfo[19] = 	{	0,			30,			70,			120,
	180,		260,		320,		400,
	600,		800,		1000,		1200,
	1500,		2000,		2800,		4000,
	8000,		15000,		30000	};	LPCTSTR pszOrderName[20] = 	{	_T("平民"),			_T("童生"),		_T("秀才"),		_T("举人"),	_T("进士"),			_T("探花"),		_T("榜眼"),		_T("状元"),	_T("九品芝麻官"),	_T("八品县丞"),	_T("七品知县"),	_T("六品通判"),	_T("五品知州"),		_T("四品知府"),	_T("三品御史"),	_T("二品尚书"),	_T("一品大学士"),	_T("宰相"),		_T("王爷"),		_T("皇帝")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

//
//七龙珠
//
LPCTSTR __cdecl CGameRankManager::GetSDragonBallRank(LONG lScore)
{
/*
七龙珠
1 0 龙蛋 
2 50 初生龙 
3 100 雏龙 
4 200 幼龙 
5 500 鸭嘴龙 
6 800 双棘龙 
7 1200 迅龙 
8 2000 雷龙 
9 3000 甲龙 
10 5000 角龙 
11 8000 剑龙 
12 12000 黄金龙 
13 15000 霸王龙 
14 18000 北海冥龙 
15 23000 南海狂龙 
16 30000 西海魔龙 
17 40000 东海神龙 
18 60000 中华圣龙  
*/
	static __int64 lScoreInfo[18] = 	{	0,		50,		100,	200,
	500,	800,	1200,	2000,
	3000,	5000,	8000,	12000,
	15000,	18000,	23000,	30000,
	40000,	60000
	};	LPCTSTR pszOrderName[18] = 	{	_T("龙蛋"),			_T("初生龙"),		_T("雏龙"),		_T("幼龙"),	_T("鸭嘴龙"),		_T("双棘龙"),		_T("迅龙"),		_T("雷龙"),	_T("甲龙"),			_T("角龙"),			_T("剑龙"),		_T("黄金龙"),	_T("霸王龙"),		_T("北海冥龙"),		_T("南海狂龙"),	_T("西海魔龙"),	_T("东海神龙"),		_T("中华圣龙")	};	for(int i=0; i<18; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[17];
}

//
//挖坑
//
LPCTSTR __cdecl CGameRankManager::GetCavernRank(LONG lScore)
{
/*
挖坑
分数
<=0
1		-	29
30		-	59
60		-	99

100		-	179
180		-	339
340		-	659
660		-	1299

1300	-	2579
2580	-	5139
5140	-	10259
10260	-	20499

20500	-	40979
40980	-	81939
81940	-	163859
163860	-	327699

327700	-	655379
655380	-	1310739
1310740	-	2621459
2621460以上
*/
	LONG lScoreInfo[19] = 	{	0,		30,		60,		100,	
	180,	340,	660,	1300,	
	2580,	5140,	10260,	20500,	
	40980,	81940,	163860,	327700,	
	655380,	1310740,2621460
	};	LPCTSTR pszOrderName[20] = 	{	_T("地鼠"),		_T("硕鼠"),			_T("鼹鼠"),		_T("开拓者"),	_T("掘地者"),	_T("挖掘者"),		_T("淘金者"),	_T("铁锹工"),	_T("榔头工"),	_T("勘探工"),		_T("推土机"),	_T("重型推土机"),	_T("挖掘机"),	_T("重型挖掘机"),	_T("钻井机"),	_T("重型钻井机"),	_T("挖坑玩家"),	_T("挖坑专家"),		_T("挖坑精英"),	_T("挖坑大师")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

//
//锄大地
//
LPCTSTR __cdecl CGameRankManager::GetHoeRank(LONG lScore)
{
/*
锄大地
分数
<=0 
1		-	99
100		-	399
400		-	899

900		-	1599
1600	-	2499
2500	-	3699
3700	-	5499

5500	-	7699
7700	-	10499
10500	-	15499
15500	-	22499

22500	-	31499
31500	-	46499
46500	-	66499
66500	-	106499

106500	-	186499
186500	-	286499
286500	-	572999
573000以上
*/
	LONG lScoreInfo[19] = 	{	0,		100,	400,	900,	
	1600,	2500,	3700,	5500,	
	7700,	10500,	15500,	22500,	
	31500,	46500,	66500,	106500,	
	186500,	286500,	573000
	};	LPCTSTR pszOrderName[20] = 	{	_T("游民"),		_T("游侠"),		_T("侠客"),		_T("侠圣"),	_T("骑士"),		_T("百眼巨人"),	_T("精灵王"),	_T("天使"),	_T("月亮神"),	_T("战神"),		_T("胜利神"),	_T("太阳神"),	_T("爱神"),		_T("大地神"),	_T("火神"),		_T("海神"),	_T("冥神"),		_T("天王"),		_T("终极天王"),	_T("究极天王")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

//
//中国象棋
//
LPCTSTR __cdecl CGameRankManager::GetCChessRank(LONG lScore)
{
/*
象棋
分数
<=0
1		-	9
10		-	29
30		-	59

60		-	99
100		-	199
200		-	349
350		-	549

550		-	749
750		-	999
1000	-	1299
1300	-	1599

1600	-	1999
2000	-	2499
2500	-	2999
3000	-	3999

4000	-	4999
5000	-	5999
6000	-	7999
8000以上
*/

	LONG lScoreInfo[19] = 	{	0,		10,		30,		60,
	100,	200,	350,	550,
	750,	1000,	1300,	1600,
	2000,	2500,	3000,	4000,	
	5000,	6000,	8000
	};	LPCTSTR pszOrderName[20] = 	{	_T("菜鸟"),		_T("入门学徒"),	_T("中级学徒"),	_T("高级学徒"),	_T("学成出师"),	_T("九级棋士"),	_T("八级棋士"),	_T("七级棋士"),	_T("六级棋士"),	_T("五级棋士"),	_T("四级棋士"),	_T("三级棋士"),	_T("二级棋士"),	_T("一级棋士"),	_T("三级大师"),	_T("二级大师"),	_T("一级大师"),	_T("特级大师"),	_T("国家大师"),	_T("国际大师")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

//
//色盅
//
LPCTSTR __cdecl CGameRankManager::GetSeZhongRank(LONG lScore)	
{
	__int64 lScoreInfo[20] = 	{	1000,		2000,		4000,		8000,
	20000,		40000,		80000,		150000,
	300000,		500000,		1000000,	2000000,
	5000000,	10000000,	50000000,	100000000,
	500000000,	1000000000,	5000000000,	10000000000
	};	LPCTSTR pszOrderName[20] = 	{	_T("初学者"),	_T("散户"),		_T("投机者"),	_T("幸运者"),	_T("操盘手"),	_T("行家"),		_T("专家"),		_T("精英"),	_T("大师"),		_T("技巧大师"),	_T("欺诈大师"),	_T("幻术大师"),	_T("盅霸"),		_T("盅王"),		_T("盅神"),		_T("赌侠"),	_T("赌王"),		_T("赌圣"),		_T("赌神"),		_T("至尊")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

LPCTSTR __cdecl CGameRankManager::GetFourEnsignRank(LONG lScore)
{
	
	LONG lScoreInfo[21] = 	{	30,		120,	270,
	480,	750,	1080,	1470,
	1920,	2430,	3000,	3630,
	4320,	5070,	5880,	6750,	
	7680,	8670,	9720,	10830,
	12000,	13230
	};	LPCTSTR pszOrderName[22] = 	{	_T("工兵"),		_T("副班长"),	_T("正班长"),	_T("副排长"),	_T("正排长"),	_T("副连长"),	_T("正连长"),	_T("副营长"),	_T("正营长"),	_T("副团长"),	_T("正团长"),	_T("副旅长"),	_T("正旅长"),	_T("副师长"),	_T("正师长"),	_T("副军长"),	_T("正军长"),	_T("副司令"),	_T("正司令"),	_T("副元帅"),	_T("正元帅"),	_T("统帅")	};	for(int i=0; i<21; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[21];
}
//
//斗兽棋
//

LPCTSTR __cdecl CGameRankManager::GetDoShouQiRank(LONG lScore)
{
	
	LONG lScoreInfo[16] = 	{	10,		50,		80,		110,
	150,	200,	300,	400,
	500,	700,	800,    900,	
	1000,	1500,	2000,	3000
	};	LPCTSTR pszOrderName[17] = 	{	_T("虫宝宝"),	   _T("幼虫"),	     _T("小飞虫"),	    _T("成虫"),	_T("花壳虫"),	   _T("两翼虫"),	 _T("四翼虫"),      _T("轻翼虫"),	_T("战虫"),  	   _T("天虫"),	     _T("右道神虫"),	_T("左道神虫"),		_T("天虫王子"),    _T("天虫王后"),	 _T("天虫头领"),	_T("无敌天虫"),	_T("无敌天虫")		};	for(int i=0; i<16; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[16];
}

//围棋
LPCTSTR __cdecl CGameRankManager::GetGoRank(LONG lScore)
{
	
	LONG lScoreInfo[18] = 	{	-800,	-700,		-600,		-500,
    -400,	-300,	-200,	-100,
	0,	100,	200,	300,
	400,	500,	600,	700,	
	800,900
	};	LPCTSTR pszOrderName[19] = 	{	_T("九级"),	_T("八级"),	_T("七级"),	_T("六级"),	_T("五级"),	 _T("四级"),	_T("三级"),	_T("二级"),	_T("一级"),	_T("一段"),	_T("二段"),	_T("三段"),	_T("四段"),	_T("五段"),	_T("六段"),	_T("七段"),	_T("八段"),	_T("九段"),_T("棋圣")	};	for(int i=0; i<18; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[18];
}

//
//斗地主
//
LPCTSTR __cdecl CGameRankManager::GetLanddRank(LONG lScore)
{
/*
斗地主
分数

<=0
1		-	49
50		-	99
100		-	149

150		-	199
200		-	299
300		-	499
500		-	799

800		-	1199
1200	-	1699
1700	-	2299
2300	-	2999

3000	-	3999
4000	-	5999
6000	-	8999
9000	-	11999

12000	-	19999
20000	-	49999
50000	-	99999
10万分以上
*/
	static __int64 lScoreInfo[19] = 	{	0,		50,		100,	150,
	200,	300,	500,	800,
	1200,	1700,	2300,	3000,
	4000,	6000,	9000,	12000,
	20000,	50000L,	100000L
	};	LPCTSTR pszOrderName[20] = 	{	_T("乞丐"),		_T("小工"),		_T("短工"),		_T("长工"),	_T("佃户"),		_T("贫农"),		_T("下农"),		_T("中农"),	_T("富农"),		_T("小地主"),	_T("大地主"),	_T("乡绅"),	_T("富绅"),		_T("小资本家"),	_T("大资本家"),	_T("金融家"),	_T("银行家"),	_T("富豪"),		_T("超级富豪"),	_T("首富")	};	for(int i=0; i<19; i++)		{		if(lScore < lScoreInfo[i]) 		{			return pszOrderName[i];		}	}	return pszOrderName[19];
}

LPCTSTR __cdecl CGameRankManager::GetRunFastRank(LONG lScore)
{
	return _T("");
}

LPCTSTR __cdecl CGameRankManager::GetDefaultRank(LONG lScore)
{
	return _T("");
}


//
//建立对象函数
//
extern "C" __declspec(dllexport) 
void * __cdecl CreateGameRankManager(const GUID & Guid, 
									 DWORD dwInterfaceVer)
{
	//建立对象
	CGameRankManager *pGameRankManager=NULL;
	try
	{
		pGameRankManager = new CGameRankManager();
		if(pGameRankManager==NULL) 
		{
			throw _T("创建失败");
		}

		void *pObject = pGameRankManager->QueryInterface(Guid,dwInterfaceVer);
		if(pObject==NULL) 
		{
			throw _T("接口查询失败");
		}
		return pObject;
	}
	catch (...) 
	{
	}

	//清理对象
	SafeDelete(pGameRankManager);
	return NULL;
}


//-----------------------------------------------
//					the end
//-----------------------------------------------
