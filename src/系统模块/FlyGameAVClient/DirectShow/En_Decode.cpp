// En_Decode.cpp: implementation of the En_Decode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "En_Decode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

En_Decode::En_Decode(const char *fin_name, const char *fout_name)
{
	BitContainer = 0;
	BitCounter = 0;
	NumOfRootNode = 0;
	for (int i = 0; i < 511; i++)
		OurTree[i].freq = 0;

	StrCopy(fout_name, outfile_buf);
	StrCopy(fin_name, infile_buf);
}

En_Decode::En_Decode(const char *fin_name)
{
//	in_file.open(fin_name, std::ios::binary|std::ios::in);
//	temp_file.open(fin_name, std::ios::binary|std::ios::in);
	BitContainer = 0;
	BitCounter = 0;
	NumOfRootNode = 0;
	for (int i = 0; i < 511; i++)
		OurTree[i].freq = 0;

	StrCopy(fin_name, infile_buf);
}

En_Decode::En_Decode()
{
	BitContainer = 0;
	BitCounter = 0;
	NumOfRootNode = 0;
	for (int i = 0; i < 511; i++)
		OurTree[i].freq = 0;
}

void En_Decode::StrCopy(const char *from, char *to)
{
	if (!*from)
		return;
	while (*from)
	{
		*to = *from;
		to++;
		from++;
	}
	*to = '\0';
}

unsigned char En_Decode::GetStrLength(const char *str)
{
	unsigned char length = 0;
	while (*str)
	{
		length++;
		str++;
	}
	return length;
}

En_Decode::~En_Decode()
{
}

void En_Decode::BuildHufTree()
{
	int NodeCounter = 256;
	int i;

	for (i = 0; i < NodeCounter; i++)
	{
		OurTree[i].parent = -1;
		OurTree[i].right = -1;
		OurTree[i].left = -1;
	}

	while (1)
	{
		int MinFreq0 = -1;
		int MinFreq1 = -1;

		for (i = 0; i < NodeCounter; i++)
		{
			if (i != MinFreq0)
			{
				if (OurTree[i].freq > 0 && OurTree[i].parent == -1)
				{
					if (MinFreq0 == -1 || OurTree[i].freq < OurTree[MinFreq0].freq)
					{
						if (MinFreq1 == -1 || OurTree[i].freq < OurTree[MinFreq1].freq)
							MinFreq1 = MinFreq0;
						MinFreq0 = i;
					}
					else if (MinFreq1 == -1 || OurTree[i].freq < OurTree[MinFreq1].freq)
						MinFreq1 = i;
				}
			}
		}
		if (MinFreq1 == -1)
		{
			NumOfRootNode = MinFreq0;
			break;
		}

		//Combine two nodes to form a parent node
		OurTree[MinFreq0].parent = NodeCounter;
		OurTree[MinFreq1].parent = NodeCounter;
		OurTree[NodeCounter].freq = OurTree[MinFreq0].freq + OurTree[MinFreq1].freq;
		OurTree[NodeCounter].right = MinFreq0;
		OurTree[NodeCounter].left = MinFreq1;
		OurTree[NodeCounter].parent = -1;
		NodeCounter++;
	}
}

void En_Decode::Output1Bit(int bit)
{
	if (BitCounter == 8 || bit == -1)
	{
		while (BitCounter < 8)
		{
			BitContainer <<= 1;
			BitCounter += 1;
		}
		DestLength++;
		out_file.put(BitContainer);
		BitCounter = 0;
	}
	BitContainer = (BitContainer << 1) | bit;
	BitCounter++;
}

void En_Decode::Compress1Byte(int node, int child)
{
	if (OurTree[node].parent != -1)
	{
		bit_count++;
		Compress1Byte(OurTree[node].parent, node);
	}
	if (child != -1)
	{
		if (child == OurTree[node].right)
			Output1Bit(0);
		else if(child == OurTree[node].left)
			Output1Bit(1);
	}
}

void En_Decode::Encode()
{
	char c;
	unsigned char uc;
	int idx = 0;

	DestLength = 0;

	std::ifstream in_file;
	std::ifstream temp_file;

	in_file.open(infile_buf, std::ios::binary|std::ios::in);
	if (out_file.is_open())
		out_file.close();
	out_file.open(outfile_buf, std::ios::binary|std::ios::out);
	temp_file.open(infile_buf, std::ios::binary|std::ios::in);

	//Out put the encoded file name string
	unsigned char f = 0;
	ext_length = GetStrLength(ext_name);
	out_file.put(ext_length);
	while (f < ext_length)
		out_file.put(ext_name[f++]);

	OrigBytes = 0;
	ActiveSymbs = 0;
	while (!in_file.eof())
	{
		in_file.get(c);
		uc = static_cast<unsigned char>(c);
		if (OurTree[uc].freq == 0)
		{
			ActiveSymbs++;
		}
		OurTree[uc].freq++;
		OrigBytes++;
	}

	out_file.write(reinterpret_cast<const char *>(&OrigBytes), sizeof(FREQCOUNTER));
	out_file.write(reinterpret_cast<const char *>(&ActiveSymbs), sizeof(ActiveSymbs));

	for (idx = 0; idx < 256; idx++)
	{
		if (OurTree[idx].freq > 0)
		{
			uc = static_cast<char>(idx);
			out_file.put(uc);
			out_file.write(reinterpret_cast<const char *>(&OurTree[idx].freq), sizeof(FREQCOUNTER));
		}
		EncodedCode[idx] = 0;
	}

	BuildHufTree();

	bit_count = 0;
	temp_file.seekg(0, std::ios::beg);
	while(1)
	{
		temp_file.get(c);
		if (temp_file.eof())
			break;
		uc = static_cast<unsigned char>(c);
		Compress1Byte(uc, -1);
		EncodedCode[uc] = bit_count;
		bit_count = 0;
	}
	Output1Bit(-1);
	in_file.close();
	temp_file.close();
	out_file.close();
}

void En_Decode::Decode()
{
	std::ifstream in_file;
	in_file.open(infile_buf, std::ios::binary|std::ios::in);

	DestLength = 0;

	in_file.get(ext_name[0]);
	ext_length = static_cast<unsigned char>(ext_name[0]);
	for (int i = 0; i < ext_length; i++)
		in_file.get(ext_name[i]);
	ext_name[i] = '\0';
	if (out_file.is_open())
		out_file.close();

	out_file.open(outfile_buf, std::ios::binary|std::ios::out);
	in_file.read(reinterpret_cast<char *>(&OrigBytes), sizeof(FREQCOUNTER));
	in_file.read(reinterpret_cast<char *>(&ActiveSymbs), sizeof(int));

	FREQCOUNTER temp1 = OrigBytes;
	int temp2 = ActiveSymbs;
	char c;
	unsigned char uc;
	while (temp2--)
	{
		in_file.get(c);
		uc = static_cast<unsigned char>(c);
		in_file.read(reinterpret_cast<char *>(&OurTree[uc].freq), sizeof(FREQCOUNTER));
	}

	BuildHufTree();

	temp1--;
	while (temp1--)
	{
		int NumOfTgtSymb;
		NumOfTgtSymb = NumOfRootNode;

		while (OurTree[NumOfTgtSymb].right != -1)
		{
			if (BitCounter == 0)
			{
				if (in_file.eof())
				{
					goto CloseAllFiles;
					return;
				}
				DestLength++;
				in_file.get(c);
				BitContainer = static_cast<unsigned char>(c);
				BitCounter = 8;
			}
			if (BitContainer & 0x80)
				NumOfTgtSymb = OurTree[NumOfTgtSymb].left;
			else
				NumOfTgtSymb = OurTree[NumOfTgtSymb].right;
			BitContainer <<= 1;
			BitCounter--;
		}
		out_file.write(reinterpret_cast<const char *>(&NumOfTgtSymb), sizeof(char));
	}
CloseAllFiles:
	in_file.close();
	out_file.close();
}

void En_Decode::SetInputFileName(const char *pathname, const char *extname)
{
	StrCopy(pathname, infile_buf);
	StrCopy(extname, ext_name);
}

void En_Decode::SetOutputFileName(const char *pathname)
{
	StrCopy(pathname, outfile_buf);
}

FREQCOUNTER En_Decode::GetFreq(const int idx)
{
	return OurTree[idx].freq;
}

void En_Decode::GetExtName(char *extname)
{
	std::ifstream in_file;
	in_file.open(infile_buf, std::ios::binary|std::ios::in);

	in_file.get(*extname);
	ext_length = static_cast<unsigned char>(*extname);
	for (int i = 0; i < ext_length; i++)
	{
		in_file.get(*extname);
		extname++;
	}
	*extname = '\0';

	in_file.close();
}

FREQCOUNTER En_Decode::GetMinFreq()
{
	FREQCOUNTER temp = OurTree[0].freq;
	for (int i = 0; i < 255; i++)
		if (temp > OurTree[i].freq)
			temp = OurTree[i].freq;
	return temp;
}

FREQCOUNTER En_Decode::GetMaxFreq()
{
	FREQCOUNTER temp = OurTree[0].freq;
	for (int i = 0; i < 255; i++)
		if (temp < OurTree[i].freq)
			temp = OurTree[i].freq;
	return temp;
}

int En_Decode::GetActiveSymbols()
{
	return ActiveSymbs;
}

FREQCOUNTER En_Decode::GetActualDataLength()
{
	return DestLength;
}

FREQCOUNTER En_Decode::GetUncodedFileLength()
{
	return OrigBytes;
}

double En_Decode::GetUncodedFileCodeLength()
{
	const double BASE = log10(2.0);
	double temp1, temp2, res = 0.0;
	for (int i = 0; i < 256; i++)
	{
		if (OurTree[i].freq != 0)
		{
			temp1 = static_cast<double>(OurTree[i].freq)/static_cast<double>(OrigBytes);
			temp2 = log10(temp1);
			temp2 *= temp1;
			res += temp2;
		}
	}
	return -res/BASE;
}

double En_Decode::GetSourceFileEntropy()
{
	double temp = 0;

	for (int i = 0; i < 256; i++)
	{
		temp += static_cast<double>(EncodedCode[i])*OurTree[i].freq/OrigBytes;
	}
	return temp;
}