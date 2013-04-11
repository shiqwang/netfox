#pragma once

class CConvertRgbYuv
{
private:
	static int RGB2YUV_YR[256], RGB2YUV_YG[256], RGB2YUV_YB[256];
	static int RGB2YUV_UR[256], RGB2YUV_UG[256], RGB2YUV_UBVR[256];
	static int RGB2YUV_VG[256], RGB2YUV_VB[256];

	static long int crv_tab[256];
	static long int cbu_tab[256];
	static long int cgu_tab[256];
	static long int cgv_tab[256];
	static long int tab_76309[256];
	static unsigned char clp[1024];

	void InitLookupTable();
	void InitConvertTable();
public:
	CConvertRgbYuv(void);
	~CConvertRgbYuv(void);

	void RGB24_YUV( int width, int height, unsigned char *source,unsigned char *dest );
	void YUV_RGB24(unsigned char* pSource, unsigned char *pDest, int width, int height);
};
