
//
#define SIZE_AUDIO_FRAME 960
#define SIZE_AUDIO_PACKED 60
#define SIZE_AUDIO_PACKED_MP3  590

#define SIZE_FILE_PACKED  1024   //1K文件传送量

typedef struct
{
	short flag;
	BYTE session;
	BYTE id;
	
	BYTE cmd;
	BYTE type;//0：始发 1：反馈
	BYTE ext;
	unsigned short data_size_extra;
}PACK_CMD;


//文件传送数据包（包含文件数据）
typedef struct
{
	short flag;//类型
	BYTE session;
	BYTE  id;//编号0--->终
	unsigned short data_size;//数据大小
	
	BYTE data[SIZE_FILE_PACKED];//文件数据
	
}PACK_FILE;

//音频数据包（包含音频数据）
typedef struct
{
	short flag;
	BYTE session;
	BYTE  id;
	//Add by 2005-10-22
	BYTE video_show_position;//视频显示位置0,1,2.....
	
	int roomID;//房间号
	int sendPlayerID;//发送人ID	
	int deskID;//发送人的DESK ID

	BYTE data[SIZE_AUDIO_PACKED];
	//	BYTE data[SIZE_AUDIO_PACKED_MP3];
	
}PACK_AUDIO;

//视频数据包（不包含视频数据）
typedef struct
{
	short flag;
	BYTE session;
	BYTE id;
	BYTE subid;
	BYTE key; //whether key frame
	unsigned int data_size_total;
	unsigned short data_size;
	//Add by 2005-10-22
	BYTE video_show_position;//视频显示位置0,1,2.....

	int roomID;//房间号
	int sendPlayerID;//发送人ID	
	int deskID;//发送人的DESK ID

	//一般不使用,只是转发时候才用
	char toIPStr[20];//目标IP
	int toPortInt;//目标端口
	
}PACK_VIDEO;

//MP3 DATA（不包含视频数据）
typedef struct
{
	short flag;
	BYTE session;
	BYTE id;
	BYTE subid;
	BYTE key; //whether key frame
	unsigned int data_size_total;
	unsigned short data_size;
	//Add by 2005-10-22
	//	BYTE video_show_position;//视频显示位置0,1,2.....
	
	
}PACK_MP3_DATA;


//设备支持列表
typedef struct
{
	bool bVideoIn;  //视频采集
	bool bVideoOut; //视频输出
	bool bAudioIn;  //声音采集
	bool bAudioOut;  //声音输出
	bool bVideoCodec;//视频编码
	bool bAudioCodec;//声音编码
}DEVICE_SUPPORT;

#define CPARAM_QCIF 0

#define CPARAM_INTER TRUE
#define CPARAM_INTRA FALSE

typedef struct compression_parameters 
{
/* Contains all the parameters that are needed for 
   encoding plus all the status between two encodings */
  int half_pixel_searchwindow; /* size of search window in half pixels
				  if this value is 0, no search is performed
				*/
  int format;			/*  */
  int pels;			/* Only used when format == CPARAM_OTHER */
  int lines;			/* Only used when format == CPARAM_OTHER */
  int inter;			/* TRUE of INTER frame encoded frames,
				   FALSE for INTRA frames */
  int search_method;		/* DEF_EXHAUSTIVE or DEF_LOGARITHMIC */
  int advanced_method;		/* TRUE : Use array to determine 
				          macroblocks in INTER frame
					  mode to be encoded */
  int Q_inter;			/* Quantization factor for INTER frames */
  int Q_intra;			/* Quantization factor for INTRA frames */
  unsigned int *data;		/* source data in qcif format */
  unsigned int *interpolated_lum;	/* intepolated recon luminance part */
  unsigned int *recon;		/* Reconstructed copy of compressed frame */
  int *EncodeThisBlock; 
                                /* Array when advanced_method is used */
} CParam;

/* Structure for counted bits */
typedef struct bits_counted
 {
  int Y;
  int C;
  int vec;
  int CBPY;
  int CBPCM;
  int MODB;
  int CBPB;
  int COD;
  int header;
  int DQUANT;
  int total;
  int no_inter;
  int no_inter4v;
  int no_intra;
/* NB: Remember to change AddBits(), ZeroBits() and AddBitsPicture() 
   when entries are added here */
} Bits;
