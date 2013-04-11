// En_Decode.h: interface for the En_Decode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EN_DECODE_H__EFEDB134_3A40_4855_8EA5_828E486E6B35__INCLUDED_)
#define AFX_EN_DECODE_H__EFEDB134_3A40_4855_8EA5_828E486E6B35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <cmath>

typedef unsigned int FREQCOUNTER;

struct HufNodeStruc
{
	FREQCOUNTER freq;
	int parent;
	int right;
	int left;
};

const int MaxFileNameLength = 256;
//
class En_Decode
{
public:
	En_Decode(const char *fin_name, const char *fout_name);
	En_Decode(const char *fin_name);
	En_Decode();
	virtual ~En_Decode();
	void Encode();
	void Decode();
	void SetInputFileName(const char *pathname, const char *extname);
	void SetOutputFileName(const char *pathname);
	void GetExtName(char *extname);
	FREQCOUNTER GetMinFreq();
	FREQCOUNTER GetMaxFreq();
	FREQCOUNTER GetFreq(const int idx);
	int GetActiveSymbols();
	FREQCOUNTER GetActualDataLength();
	FREQCOUNTER GetUncodedFileLength();
	double GetUncodedFileCodeLength();
	double GetSourceFileEntropy();
private:
	void BuildHufTree();
	void Output1Bit(int bit);
	void Compress1Byte(int node, int child);
private:
	std::ofstream out_file;
	HufNodeStruc OurTree[511];
	int NumOfRootNode;
	unsigned char BitContainer;
	unsigned char BitCounter;
	char infile_buf[MaxFileNameLength];
	char outfile_buf[MaxFileNameLength];
	unsigned char EncodedCode[256];
	char ext_name[10];
	unsigned char ext_length;
	int bit_count;

	FREQCOUNTER OrigBytes;
	int ActiveSymbs;
	FREQCOUNTER DestLength;

	void StrCopy(const char *from, char *to);
	unsigned char GetStrLength(const char *str);
};

#endif // !defined(AFX_EN_DECODE_H__EFEDB134_3A40_4855_8EA5_828E486E6B35__INCLUDED_)
