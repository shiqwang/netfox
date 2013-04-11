#include "StdAfx.h"
#include "md5.h"
#include <conio.h>
#include <stdexcept>
using namespace std;
const ULONG CMd5::Context[4]={  0x67452301,
                                0xefcdab89,
                                0x98badcfe,
                                0x10325476
       };//算法长量

const LPSTR CMd5::ErrInfo[5][2]={
        {"",""},
        {"The Memory is not enough !","内存分配失败!"},
        {"Have not assign a file !","未指定文件!"},
        {"Have not assign the text !","未指定文本!"},
        {"Have something wrong with MD5!","有错误发生!"},
        };
//==========================<内联函数实现>=========================================
//循环移位函数
ULONG CMd5::LRotate(ULONG Sdata,int nBit)
{
 return (Sdata<<nBit)|(Sdata>>(32-nBit));
}
//非线性变换函数
ULONG CMd5::F(ULONG x,ULONG y,ULONG z)
{
 return (x&y)|((~x)&z);
}
ULONG CMd5::G(ULONG x,ULONG y,ULONG z)
{
 return (x&z)|(y&(~z));
}
ULONG CMd5::H(ULONG x,ULONG y,ULONG z)
{
 return x^y^z;
}
ULONG CMd5::I(ULONG x,ULONG y,ULONG z)
{
 return y^(x|(~z));
}
///////////////////////////////////////////
//四轮循环中需要使用的函数
void CMd5::FF(ULONG &a,ULONG b,ULONG c,ULONG d,ULONG Msg,int nBit,ULONG Cnt)
{
 a+=F(b,c,d)+Msg+(ULONG)Cnt;
 a=LRotate(a,nBit);
 a+=b;
}
void CMd5::GG(ULONG &a,ULONG b,ULONG c,ULONG d,ULONG Msg,int nBit,ULONG Cnt)
{
 a+=G(b,c,d)+Msg+(ULONG)Cnt;
 a=LRotate(a,nBit);
 a+=b;
}
void CMd5::HH(ULONG &a,ULONG b,ULONG c,ULONG d,ULONG Msg,int nBit,ULONG Cnt)
{
 a+=H(b,c,d)+Msg+(ULONG)Cnt;
 a=LRotate(a,nBit);
 a+=b;
}
void CMd5::II(ULONG &a,ULONG b,ULONG c,ULONG d,ULONG Msg,int nBit,ULONG Cnt)
{
 a+=I(b,c,d)+Msg+(ULONG)Cnt;
 a=LRotate(a,nBit);
 a+=b;
}
//==========================<成员函数实现>======================================
CMd5::CMd5()
{
  IsSuc=false;
  Pointer=NULL;
  memset(strErr,0,50);
}

CMd5::~CMd5()
{

 if(Pointer){delete[]Pointer; Pointer=NULL;}
}

void CMd5::TargetStr(LPCSTR str)
{
 if(!Init())SetErr(1);
 CaculateStr(str);
}
bool CMd5::Init()
{
 IsSuc=true;
 *Digest='\0';
     Pointer=new BYTE[sizeof(ULONG)*16];

 pM=(ULONG*)Pointer;//pM指向由Pointer开辟的内存首地址
 for(int i=0;i<4;i++)
        T[i]=Context[i];//将四个特征数给T[i]
 return true;
}

void CMd5::TransForm()
{
 /* MSG msg; 
  while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
  {
   TranslateMessage(&msg);
   DispatchMessage(&msg);
  }*/
  ULONG a=T[0],b=T[1],c=T[2],d=T[3];
 //第一轮-------------------------------->
  FF (a, b, c, d, pM[ 0], S11, 0xd76aa478); //- 1 -
  FF (d, a, b, c, pM[ 1], S12, 0xe8c7b756); //- 2 -
  FF (c, d, a, b, pM[ 2], S13, 0x242070db); //- 3 -
  FF (b, c, d, a, pM[ 3], S14, 0xc1bdceee); //- 4 -
  FF (a, b, c, d, pM[ 4], S11, 0xf57c0faf); //- 5 -
  FF (d, a, b, c, pM[ 5], S12, 0x4787c62a); //- 6 -
  FF (c, d, a, b, pM[ 6], S13, 0xa8304613); //- 7 -
  FF (b, c, d, a, pM[ 7], S14, 0xfd469501); //- 8 -
  FF (a, b, c, d, pM[ 8], S11, 0x698098d8); //- 9 -
  FF (d, a, b, c, pM[ 9], S12, 0x8b44f7af); //- 10 -
  FF (c, d, a, b, pM[10], S13, 0xffff5bb1); //- 11 -
  FF (b, c, d, a, pM[11], S14, 0x895cd7be); //- 12 -
  FF (a, b, c, d, pM[12], S11, 0x6b901122); //- 13 -
  FF (d, a, b, c, pM[13], S12, 0xfd987193); //- 14 -
  FF (c, d, a, b, pM[14], S13, 0xa679438e); //- 15 -
  FF (b, c, d, a, pM[15], S14, 0x49b40821); //- 16 -
 //第二轮--------------------------------->
  GG (a, b, c, d, pM[ 1], S21, 0xf61e2562); //- 17 -
  GG (d, a, b, c, pM[ 6], S22, 0xc040b340); //- 18 -
  GG (c, d, a, b, pM[11], S23, 0x265e5a51); //- 19 -
  GG (b, c, d, a, pM[ 0], S24, 0xe9b6c7aa); //- 20 -
  GG (a, b, c, d, pM[ 5], S21, 0xd62f105d); //- 21 -
  GG (d, a, b, c, pM[10], S22,  0x2441453); //- 22 -
  GG (c, d, a, b, pM[15], S23, 0xd8a1e681); //- 23 -
  GG (b, c, d, a, pM[ 4], S24, 0xe7d3fbc8); //- 24 -
  GG (a, b, c, d, pM[ 9], S21, 0x21e1cde6); //- 25 -
  GG (d, a, b, c, pM[14], S22, 0xc33707d6); //- 26 -
  GG (c, d, a, b, pM[ 3], S23, 0xf4d50d87); //- 27 -
  GG (b, c, d, a, pM[ 8], S24, 0x455a14ed); //- 28 -
  GG (a, b, c, d, pM[13], S21, 0xa9e3e905); //- 29 -
  GG (d, a, b, c, pM[ 2], S22, 0xfcefa3f8); //- 30 -
  GG (c, d, a, b, pM[ 7], S23, 0x676f02d9); //- 31 -
  GG (b, c, d, a, pM[12], S24, 0x8d2a4c8a); //- 32 -
 //第三轮--------------------------------->
  HH (a, b, c, d, pM[ 5], S31, 0xfffa3942); //- 33 -
  HH (d, a, b, c, pM[ 8], S32, 0x8771f681); //- 34 -
  HH (c, d, a, b, pM[11], S33, 0x6d9d6122); //- 35 -
  HH (b, c, d, a, pM[14], S34, 0xfde5380c); //- 36 -
  HH (a, b, c, d, pM[ 1], S31, 0xa4beea44); //- 37 -
  HH (d, a, b, c, pM[ 4], S32, 0x4bdecfa9); //- 38 -
  HH (c, d, a, b, pM[ 7], S33, 0xf6bb4b60); //- 39 -
  HH (b, c, d, a, pM[10], S34, 0xbebfbc70); //- 40 -
  HH (a, b, c, d, pM[13], S31, 0x289b7ec6); //- 41 -
  HH (d, a, b, c, pM[ 0], S32, 0xeaa127fa); //- 42 -
  HH (c, d, a, b, pM[ 3], S33, 0xd4ef3085); //- 43 -
  HH (b, c, d, a, pM[ 6], S34,  0x4881d05); //- 44 -
  HH (a, b, c, d, pM[ 9], S31, 0xd9d4d039); //- 45 -
  HH (d, a, b, c, pM[12], S32, 0xe6db99e5); //- 46 -
  HH (c, d, a, b, pM[15], S33, 0x1fa27cf8); //- 47 -
  HH (b, c, d, a, pM[ 2], S34, 0xc4ac5665); //- 48 -
  //第四轮-------------------------------->
  II (a, b, c, d, pM[ 0], S41, 0xf4292244); //- 49 -
  II (d, a, b, c, pM[ 7], S42, 0x432aff97); //- 50 -
  II (c, d, a, b, pM[14], S43, 0xab9423a7); //- 51 -
  II (b, c, d, a, pM[ 5], S44, 0xfc93a039); //- 52 -
  II (a, b, c, d, pM[12], S41, 0x655b59c3); //- 53 -
  II (d, a, b, c, pM[ 3], S42, 0x8f0ccc92); //- 54 -
  II (c, d, a, b, pM[10], S43, 0xffeff47d); //- 55 -
  II (b, c, d, a, pM[ 1], S44, 0x85845dd1); //- 56 -
  II (a, b, c, d, pM[ 8], S41, 0x6fa87e4f); //- 57 -
  II (d, a, b, c, pM[15], S42, 0xfe2ce6e0); //- 58 -
  II (c, d, a, b, pM[ 6], S43, 0xa3014314); //- 59 -
  II (b, c, d, a, pM[13], S44, 0x4e0811a1); //- 60 -
  II (a, b, c, d, pM[ 4], S41, 0xf7537e82); //- 61 -
  II (d, a, b, c, pM[11], S42, 0xbd3af235); //- 62 -
  II (c, d, a, b, pM[ 2], S43, 0x2ad7d2bb); //- 63 -
  II (b, c, d, a, pM[ 9], S44, 0xeb86d391); //- 64 -
  T[0]+=a;
  T[1]+=b;
  T[2]+=c;
  T[3]+=d;
  return ;
}//经过四轮变换，生四组32位数，最后要将他们级连
LPCSTR CMd5::GetDigestKey()
{
    if(IsSuc)
 {  
  return Digest;
 }
   SetErr(4);
   return NULL;
}
bool CMd5::CaculateStr(LPCSTR str)
{
   ULONG slen=(ULONG)strlen(str);
   ULONG n=slen/64,ntail=slen%64;
   for(int i=0;i<(int)n;i++)
   {
 ULONG *p=(ULONG *)str;
 TransForm();
 pM+=16;
   }
   memset(Pointer,0,64);
   memcpy(Pointer,str,ntail);
   UCHAR *pt=(UCHAR *)Pointer+ntail;
   *pt=0x80;
   LONG64 *pl=(LONG64 *)Pointer+7;
   *pl=(LONG64)(slen*8);
   TransForm();
   JustifytoStr();
   return true;
}
void CMd5::SetErr(int nErr)
{
	if(*strErr)return;
	IsSuc=false;
	if(nErr==0)
	{
		#ifndef ENGLISH
		strcpy(strErr,ErrInfo[nErr][1]);   
		#else
		strcpy(strErr,ErrInfo[nErr][0]);
		#endif
	}
}
LPCSTR CMd5::GetErr()
{
 if(!*strErr)
 {
#ifndef ENGLISH
  return "未发生错误 !";
#else
  return "No error !";
#endif
 }
 else
  return strErr;
}
void  CMd5::JustifytoStr()
{
 char temp[10];
 for(int i=0;i<4;i++)
 {
  T[i]=(T[i]<<24)|((T[i]&0xff00)<<8)|((T[i]&0xff0000)>>8)|(T[i]>>24);
  sprintf(temp,"%08x",T[i]);
  strcat(Digest,temp);
 }
}//级联。