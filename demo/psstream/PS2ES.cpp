#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PS2ES.h"

#ifdef WIN32
typedef __int64 LONGLONG;
#else
typedef long long LONGLONG;
#endif


#define STREAM_MAPPING     0xbc
#define PRIVATE_STREAM_1   0xbd
#define PADDING_STREAM     0xbe
#define PRIVATE_STREAM_2   0xbf

#define FIRST_AUDIO_STREAM 0xc0
#define LAST_AUDIO_STREAM  0xdf

#define FIRST_VIDEO_STREAM 0xe0
#define LAST_VIDEO_STREAM  0xef

#define STREAM_DIRECTORY   0xff

unsigned char packet_start_code_prefix [3] = { 0x00, 0x00, 0x01 };
unsigned char end_code [4] =                 { 0x00, 0x00, 0x01, 0xb9 };
unsigned char pack_start_code [4] =          { 0x00, 0x00, 0x01, 0xba };
unsigned char system_header_start_code [4] = { 0x00, 0x00, 0x01, 0xbb };
/////////////////////////////////////////////////////////////////////////////
//   PES structure

//ESCR_flag
struct ps_escr{
	LONGLONG escr;		//ESCR_base
	short extension;	//ESCR_extension
};
struct ps_esrate{
	int rate;			//ES_rate
};
struct ps_trick{
	short mode;			//trick_mode_control
	short content;		//
};
struct ps_add{
	char info;			//additional_copy_info
};
struct ps_crc{
	short crc;			//previous_PES_packet_CRC
};
struct ps_ext_pridata{
	char pridata[16];	//PES_private_data
};
struct ps_ext_header{
	unsigned short length;		//pack_field_length    //原来是short， 但根据协议似乎应该是unsigned short，否则出现0x8xxx的长度时，就变成负值了 20141223
};
struct ps_ext_counter{
	short counter;		//program_packet_sequence_counter
	bool identifier;	//MPEG1_MPEG2_identifier
	unsigned short length;		//original_stuff_length //原来是short， 但根据协议似乎应该是unsigned short，否则出现0x8xxx的长度时，就变成负值了 20141223
};
struct ps_ext_buffer{
	bool scale;			//P-STD_buffer_scale
	short size;			//P-STD_buffer_size
};
struct ps_ext_ext{
	unsigned short length;		//PES_extension_field_length	//原来是short， 但根据协议似乎应该是unsigned short，否则出现0x8xxx的长度时，就变成负值了 20141223
};
struct ps_ext{
	bool pridata;		//PES_private_data_flag
	bool header;		//pack_header_field_flag
	bool counter;		//program_packet_sequence_counter_flag
	bool buffer;		//P-STD_buffer_flag
	bool extension;		//PES_extension_flag_2
	ps_ext_pridata prif;		//PES_private_data_field
	ps_ext_header headerf;		//pack_header_field_field
	ps_ext_counter countf;		//program_packet_sequence_counter_field
	ps_ext_buffer buff;			//P-STD_buffer_field
	ps_ext_ext extf;			//PES_extension_field

};
//PES packet
struct ps_packet{
	unsigned char id;  //stream_id
	unsigned short length;      //PES_packet_length //原来是short， 但根据协议似乎应该是unsigned short，否则出现0x8xxx的长度时，就变成负值了 20141223
	short flag;        //'10'
	short scramble;    //PES_scrambling_control
	bool priority;     //PES_priority
	bool align;        //data_alignment_indicator
	bool copyright;    //copyright
	bool origin;       //original_or_copy
	short ptsdts;      //PTS_DTS_flags
	bool escr;         //ESCR_flag
	bool rate;		   //ES_rate_flag
	bool trick;        //DSM_trick_mode_flag
	bool add;		   //additional_copy_info_flag
	bool crc;		   //PES_CRC_flag
	bool extension;	   //PES_extension_flag
	short hlength;	   //PES_header_data_length
	LONGLONG pts;	   	
	LONGLONG dts;
	ps_escr escrf;     //ESCR_field
	ps_esrate ratef;   //ES_rate_field
	ps_trick trickf;   //DSM_trick_mode_field
	ps_add addf;	   //additional_copy_info_field	
	ps_crc crcf;	   //PES_CRC_field
	ps_ext extf;	   //PES_extension_field	

};
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Pack structure
struct ps_pack{
	LONGLONG scr;		//system_clock_reference_base
	short scre;			//system_clock_reference_extension
	int rate;			//program_mux_rate
	unsigned short stufflength;	//pack_stuffing_length //原来是short， 但根据协议似乎应该是unsigned short，否则出现0x8xxx的长度时，就变成负值了 20141223
};

/* Program stream map handling */
typedef struct ps_es_t
{
    int i_type;
    int i_id;

    int i_descriptor;
    unsigned char *p_descriptor;

    /* Language is iso639-2T */
    unsigned char lang[3];

} ps_es_t;

//在一段二进制数据中搜索一段二进制数据的起始位置。如果没找到，则返回NULL
//pData, DataLen是整个数据, 
//pModel, ModelLen是要搜索的数据段
static unsigned char *BinBin(unsigned char *pData, int DataLen, unsigned char *pModel, int ModelLen)
{
	int i, j;
	int MatchFlg;
	for (i=0; i<DataLen-ModelLen; i++)
	{
		MatchFlg = 1;
		for (j=0; j<ModelLen; j++)
		{
			if (pData[i+j] != pModel[j])
			{
				MatchFlg = 0;
				break;
			}
		}
		if (MatchFlg)
		{
			return pData+i;
		}
	}
	return NULL;
}

unsigned char * parse_system_header (unsigned char *p)
{
	int header_length;

	p += 4;  /* skip start code */
	header_length = (*(p++)) << 8;
	header_length += *(p++);

	p += header_length;
  
	return (p);
}


LONGLONG parse_time_stamp (unsigned char *p)
{
    int b;
    LONGLONG val;

    b = *p++;
    val = (b & 0x0e) << 29;

    b = (*(p++)) << 8;
    b += *(p++);
    val += ((b & 0xfffe) << 14);

    b = (*(p++)) << 8;
    b += *(p++);
    val += ((b & 0xfffe) >> 1);

    return (val);
}

LONGLONG parse_scr(unsigned char *p)
{
	int tmp;
	LONGLONG ltmp;

	tmp = *p++;
	if( ((tmp >> 6) & 3) != 1)
		return 0;
	//get scr and scr ext;
	ltmp = (tmp & 0x38) << 27;
	tmp <<= 8;
	tmp += *(p++);
	ltmp += (tmp & 0x03ff) <<20;
	tmp = *(p++);
	ltmp += (tmp & 0xf8) <<12;
	tmp <<= 8;
	tmp += *(p++);
	ltmp += (tmp & 0x03ff) << 5;
	tmp = *(p++);
	ltmp += (tmp & 0xf8);
	return (ltmp);
}

LONGLONG parse_scre(unsigned char *p)
{
	int tmp;
	LONGLONG ltmp;

	tmp = *p++;
	tmp <<= 8;
	tmp += *(p++);
	ltmp = (tmp & 0x03fe) >> 1;
	return (ltmp);
}

LONGLONG parse_muxrate(unsigned char *p)
{
	int tmp;
	LONGLONG ltmp;

	tmp = *p++;
	tmp <<= 8;
	tmp += *(p++);
	tmp <<= 8;
	tmp += *(p++);
	ltmp = (tmp & 0xfffffc) >> 1;
	return (ltmp);
}

unsigned char *parse_packet (unsigned char *pBuf, int len, unsigned char &found_pts, LONGLONG &pts, unsigned char &found_dts, LONGLONG &dts, unsigned char *pOut, int *pOutLen)
{
    int length;
    unsigned char *end;
    unsigned char *data_start;
	int i;
    int count;
	int tmp;				//32bit temperatory
	LONGLONG ltmp;			//64bit temperatory
    int flags, pes_header_data_length;
	short trick_flags,ext_flags,PES_ext_field_len;
	ps_packet psp;
	unsigned char stream;
	unsigned char *p = pBuf;

	PES_ext_field_len = 0; //added by hejia on 20150203

	stream = p[3];

    p += 3;  /* skip start code*/
	psp.id = *(p++);
    psp.length = (*(p++)) << 8;
    psp.length += *(p++);
	
	if (0 == psp.length)
	{
		//兼容吴翔以前的代码。以前的写法是：如果长度大于64K字节，就写0。这是不对的。
		return NULL;
//		psp.length = len-6;
	}

	end = p + psp.length;

	if (psp.id == PADDING_STREAM)  goto done;  /* ignore */
	else 
	{
		//flags contains PES_scrambling_control,PES_priority,data_alignment_indicator,copyright,...
		
		flags = (*(p++)) << 8;
		flags += *(p++);
		
		psp.flag = (flags >> 14) & 3;  
	
		if (psp.flag != 2)  return NULL;//if psp.flag is not equal to '10',this is a bad stream packet.
		psp.hlength = *(p++);
		//get the data start address
		data_start = p + psp.hlength;
		//get pts & dts
		psp.ptsdts = (flags >> 6) & 3;
		switch (psp.ptsdts)
		{
		case 2:
			if (! found_pts)
			{
				found_pts = 1;
				pts = parse_time_stamp (p);
				psp.pts = pts;
				psp.dts = 0;
	    
			}			
			p += 5;
			break;
		case 3:
			if (! found_pts )
			{
				found_pts = 1;
				pts  = parse_time_stamp (p);
				psp.pts = pts;

			}
			p += 5;
			if (! found_dts )
			{
				found_dts = 1;
				dts = parse_time_stamp (p);
				psp.dts = dts;
			}
			
			p += 5;
			break;
		case 0:
			break;
		default:
			return NULL;
		}

		psp.escr = (flags >> 5 ) & 1;
		if(psp.escr == 1)
		{
			
			psp.escrf.escr = parse_scr(p);
			p += 4;
			psp.escrf.extension = parse_scre(p);
			p +=2;

    	}

		psp.rate = (flags >> 4 ) & 1;
		if(psp.rate == 1)
		{
			tmp = *(p++);
			tmp <<= 8;
			tmp += *(p++);
			tmp <<= 8;
			tmp += *(p++);
			psp.ratef.rate = (tmp & 0x7ffffe) >> 1;
		}

		psp.trick = (flags >> 3) & 1;
		if(psp.trick == 1)
		{
			tmp = *(p++);
			psp.trickf.mode = (tmp & 0xe0) >> 5;
			psp.trickf.content = tmp & 0x1f;
			
		}

		psp.add = (flags >> 2 ) & 1;
		if(psp.add == 1)
		{
			psp.addf.info = *(p++) >> 1;
		}

		psp.crc = (flags >> 1 ) & 1;
		if(psp.crc == 1)
		{
			tmp = *(p++);
			tmp <<= 8;
			tmp += *(p++);
			psp.crcf.crc = tmp;
		}

		psp.extension = flags & 1;
		if(psp.extension == 1)
		{
			tmp = (*(p++));
			psp.extf.pridata = (tmp >> 7) & 1;
			psp.extf.header = (tmp >> 6) & 1;
			psp.extf.counter = (tmp >> 5) & 1;
			psp.extf.buffer = (tmp >> 4) & 1;
			psp.extf.extension = tmp & 1;
			if(psp.extf.pridata == 1)
			{
				for(i=0;i<16;i++)
				{
					psp.extf.prif.pridata[i] = *p++;
				}
			}
			if(psp.extf.header == 1)
			{
				psp.extf.headerf.length = *p++;
				
			}
			if(psp.extf.counter == 1)
			{
				psp.extf.countf.counter = *p++ & 0x7f;

			}
			if(psp.extf.buffer == 1)
			{
				tmp = *p++;
				tmp += *p++;
				psp.extf.buff.scale = (tmp & 0x2000) >> 13;
				psp.extf.buff.size = tmp & 0x1fff;

			}
			if(psp.extf.extension == 1)
			{			
				psp.extf.extf.length = *(p++) & 0x7f;
				p += PES_ext_field_len;			
			}
		}
		p = data_start;
		count = (end - p);
		//到这里p指向ES码流，count是长度
		memcpy(pOut, p, count);
		*pOutLen = count;
	}
    done:
    return (end);
}

//返回NULL说明分析失败，成功则返回新的起始位置
unsigned char *parse_pack (unsigned char *buf, int len)
{
    int stream;
    unsigned char *p = buf;//
	unsigned char *pstart = buf;
	int tmp; //32bit temperatory
	LONGLONG ltmp;//64bit temperatory
	ps_pack pp;
	//skip start code
	p += 4;
	//if next 2bit is not '01', an error occurs.
    tmp = *p;
	if( ((tmp >> 6) & 3) != 1)
		return NULL;
	//get scr;
	pp.scr = parse_scr(p);
	//get scr ext;
	p += 4;
	
	pp.scre = parse_scre(p);
	//get mux rate;
	p += 2;
	pp.rate = parse_muxrate(p);
	
    p = pstart + sizeof (pack_start_code) + 10;
	// parse system header;
    if (memcmp (p, system_header_start_code, sizeof (system_header_start_code)) == 0)
        p = parse_system_header (p);
	return p;
}

unsigned char* parse_psm(unsigned char *buf, int len, unsigned char stream_type[256])
{
	int i_buffer = len;
	unsigned char *p_buffer = buf;
	int i_length, i_version, i_info_length, i_esm_length, i_es_base;

	if(p_buffer[3] != 0xbc ) return NULL;

	i_length = (unsigned short)(p_buffer[4] << 8) + p_buffer[5] + 6;
	if( i_length > i_buffer ) return NULL;

	//i_current_next_indicator = (p_buffer[6] && 0x01);
	i_version = (p_buffer[6] && 0xf8);

	i_info_length = (unsigned short)(p_buffer[8] << 8) + p_buffer[9];
	if( i_info_length + 10 > i_length ) return NULL;

	/* Elementary stream map */
	i_esm_length = (unsigned short)(p_buffer[ 10 + i_info_length ] << 8) +
		p_buffer[ 11 + i_info_length];
	i_es_base = 12 + i_info_length;

	while( i_es_base + 4 < i_length )
	{
		ps_es_t **tmp_es;
		ps_es_t es;
		es.lang[0] = es.lang[1] = es.lang[2] = 0;

		es.i_type = p_buffer[ i_es_base  ];
		es.i_id = p_buffer[ i_es_base + 1 ];
		stream_type[es.i_id] = es.i_type;

		i_info_length = (unsigned short)(p_buffer[ i_es_base + 2 ] << 8) + p_buffer[ i_es_base + 3 ];

		if( i_es_base + 4 + i_info_length > i_length ) break;

		i_es_base += 4 + i_info_length;
	}
	return p_buffer+i_length;
}
typedef struct {
	unsigned char found_pts;
	LONGLONG pts;
	unsigned char found_dts;
	LONGLONG dts;
	unsigned char *pFrm;
	int Pos;
	unsigned char StreamType; //目前仅支持H264, 0x1b； G711A, 0x90
}STREAM_INFO;

typedef struct 
{
	int MaxESFrmLen;
	STREAM_INFO StreamInfo[2]; //索引0代表音频, 索引1代表视频
	unsigned char stream_type[256];
	unsigned char *pLstPSStream;	//存放PS码流。因为某些情况下，传入的PS码流不能保证是完整的PS包，因此内部必须缓冲下来，确保真正处理时有完整PS包
	int LstPSPos;
	unsigned char *pLstESFrm;	//存放ES码流。 因为PS解包模块有可能解出半帧图像，为了保证输出的完整，因此要缓冲一帧。 知道确信新的一帧出现，才将上一帧输出。
	int LstESPos;
}PS2ES_OBJ;

void PS2ES_i_Free(PS2ES_OBJ *pObj)
{
	int i;
	for (i=0; i<2; i++)
	{
		if (NULL != pObj->StreamInfo[i].pFrm)
		{
			free(pObj->StreamInfo[i].pFrm);
			pObj->StreamInfo[i].pFrm = NULL;
		}
	}
	if (NULL != pObj->pLstESFrm)
	{
		free(pObj->pLstESFrm);
		pObj->pLstESFrm = NULL;
	}
	if (NULL != pObj->pLstPSStream)
	{
		free(pObj->pLstPSStream);
		pObj->pLstPSStream = NULL;
	}
}

int PS2ES_Create(PS2ES_HANDLE *phPS2ES, int MaxESFrmLen)
{
	int rv;
	int i;
	PS2ES_OBJ *pObj;
	pObj = (PS2ES_OBJ *)malloc(sizeof(PS2ES_OBJ));
	if (NULL == pObj)
	{
		*phPS2ES = NULL;
		return -1;
	}
	memset(pObj, 0, sizeof(PS2ES_OBJ));
	pObj->MaxESFrmLen = MaxESFrmLen;
	for (i=0; i<2; i++)
	{
		pObj->StreamInfo[i].pFrm = (unsigned char *)malloc(MaxESFrmLen);
		if (NULL == pObj->StreamInfo[i].pFrm)
		{
			rv = -2;
			goto ErrExt;
		}
	}
	pObj->pLstESFrm = (unsigned char *)malloc(MaxESFrmLen);
	if (NULL == pObj->pLstESFrm)
	{
		rv = -3;
		goto ErrExt;
	}
	pObj->LstESPos = 0;
	pObj->pLstPSStream = (unsigned char *)malloc(MaxESFrmLen);
	if (NULL == pObj->pLstPSStream)
	{
		rv = -4;
		goto ErrExt;
	}
	pObj->LstPSPos = 0;
	
	pObj->stream_type[0xe0] = 0x1b;		//赋个常用的对应表。这样可以避免在影射表来之前返回Unknown Type
	pObj->stream_type[0xc0] = 0x90;
	*phPS2ES = (PS2ES_HANDLE)pObj;
	return 0;
ErrExt:
	PS2ES_i_Free(pObj);
	free(pObj);		//到这里pObj一定不为NULL;
	*phPS2ES = NULL;
	return rv;
}

void PS2ES_Delete(PS2ES_HANDLE hPS2ES)
{
	PS2ES_OBJ *pObj = (PS2ES_OBJ *)hPS2ES;
	if (NULL == pObj)
	{
		return;
	}
	PS2ES_i_Free(pObj);
	free(pObj);
}

static int PS2ES_PushIn_i(PS2ES_HANDLE hPS2ES, unsigned char *pPSFrm, int PSFrmLen)
{
	PS2ES_OBJ *pObj = (PS2ES_OBJ *)hPS2ES;
	unsigned char *p;
	unsigned char *pEnd;
	int tmpLen;
	int tmpOffset;
	int Pos;
	unsigned char tmpCode;
	unsigned char stream;

	p = pPSFrm;
	pEnd = pPSFrm + PSFrmLen;
	while (p < pEnd)
	{
		p = BinBin(p, pEnd-p, packet_start_code_prefix, 3);
		if (NULL == p)
		{
			return -1;
		}
		tmpCode = p[3];
		if (0xba == tmpCode)
		{
			p = parse_pack(p, pEnd-p);
			if (NULL == p)
			{
				return -2;
			}
		}else if (0xBC == tmpCode) // Program stream map 
		{
			p = parse_psm(p, pEnd-p, pObj->stream_type);
			if (NULL == p)
			{
				return -3;
			}
		}
		else if ((0xBE == tmpCode)|| // Padding 
			(0xBB == tmpCode)|| // Partial System header format  //20150123 //HK设备中有这个头
			(0xBD == tmpCode)|| // Private stream 1			//20150214	HJ
			(0xBF == tmpCode)|| // Private stream 2 
			(0xB0 == tmpCode)|| // ECM 
			(0xB1 == tmpCode)|| // EMM 
			(0xFF == tmpCode)||  // Program stream directory 
			(0xF2 == tmpCode)|| // DSMCC stream 
			(0xF8 == tmpCode)) // ITU-T H.222.1 type E stream 
		{
			//不分析，跳过即可
			unsigned char *p1;
			p1 = p+4;
			tmpLen = (*(p1++)) << 8;
			tmpLen += *(p1++);
			p = p1 + tmpLen;
			if (NULL == p)
			{
				return -4;
			}
		}else
		{
			int tmpID;
			if ((tmpCode >= FIRST_AUDIO_STREAM) && (tmpCode <= LAST_AUDIO_STREAM))
			{
				tmpID = 0;
			}else if (( tmpCode >= FIRST_VIDEO_STREAM) && (tmpCode <= LAST_VIDEO_STREAM))
			{
				tmpID = 1;
			}else
			{
				return -2;
			}
			p = parse_packet (p, pEnd-p, pObj->StreamInfo[tmpID].found_pts, pObj->StreamInfo[tmpID].pts, pObj->StreamInfo[tmpID].found_dts, pObj->StreamInfo[tmpID].dts, pObj->StreamInfo[tmpID].pFrm+pObj->StreamInfo[tmpID].Pos, &tmpLen);
			if (NULL == p)
			{
				return -5;
			}
			pObj->StreamInfo[tmpID].Pos += tmpLen;
			pObj->StreamInfo[tmpID].StreamType = pObj->stream_type[tmpCode];
		}
	}

	return 0;
}


int PS2ES_PushIn(PS2ES_HANDLE hPS2ES, unsigned char *pPSFrm, int PSFrmLen)
{
	PS2ES_OBJ *pObj = (PS2ES_OBJ *)hPS2ES;
	int rv;

	if ((pPSFrm[0]==0)&&(pPSFrm[1]==0)&&(pPSFrm[2]==1))
	{
		//新来了一帧
		if (pObj->LstPSPos > 0)
		{
			rv = PS2ES_PushIn_i(hPS2ES, pObj->pLstPSStream, pObj->LstPSPos);
		}else
		{
			rv = -1;
		}
		memcpy(pObj->pLstPSStream, pPSFrm, PSFrmLen);
		pObj->LstPSPos = PSFrmLen;
		return rv;
	}else
	{
		//没有新来一帧
		memcpy(pObj->pLstPSStream+pObj->LstPSPos, pPSFrm, PSFrmLen);
		pObj->LstPSPos += PSFrmLen;
		return -2;
	}
}


//在一段二进制数据中搜索一段二进制数据的起始位置。如果没找到，则返回NULL
//pData, DataLen是整个数据, 
//pModel, ModelLen是要搜索的数据段
static unsigned char *MyBinBin(unsigned char *pData, int DataLen, unsigned char *pModel, int ModelLen)
{
	int i, j;
	int MatchFlg;
	for (i=0; i<DataLen-ModelLen; i++)
	{
		MatchFlg = 1;
		for (j=0; j<ModelLen; j++)
		{
			if (pData[i+j] != pModel[j])
			{
				MatchFlg = 0;
				break;
			}
		}
		if (MatchFlg)
		{
			return pData+i;
		}
	}
	return NULL;
}

int PS2ES_PopOut_i(PS2ES_HANDLE hPS2ES, unsigned char *pESFrm, int *pESFrmLen, unsigned int *pTmStamp, int *pFrmType, unsigned char *pStreamType)
{
	PS2ES_OBJ *pObj = (PS2ES_OBJ *)hPS2ES;
	if (0 != pObj->StreamInfo[0].Pos)
	{
		memcpy(pESFrm, pObj->StreamInfo[0].pFrm, pObj->StreamInfo[0].Pos);
		*pESFrmLen = pObj->StreamInfo[0].Pos;
		pObj->StreamInfo[0].Pos = 0;
		*pTmStamp = 0; //暂时不使用PS流中的时间戳
		*pFrmType = ES_FRM_AUDIO;
		*pStreamType = pObj->StreamInfo[0].StreamType;
		return 0;
	}
	if (0 != pObj->StreamInfo[1].Pos)
	{
		//发现个别摄像头码流有问题, 相邻两个帧的RTP时间戳会相同，因此到这里的Frm中可能包含不止一个帧，这里要把它分开
#define HEAD_H264_LEN    4
		unsigned char Head264[HEAD_H264_LEN];
		int tmpLen;
		int tmpHeadLen;

		Head264[0] = 0;
		Head264[1] = 0;
		Head264[2] = 0;
		Head264[3] = 0x01;

#define MAX_264HEAD_LEN		300  //我们认为，帧头开始的码流中偏移这么多字节后，就不可能再有SPS和PPS
		unsigned char *pEnd;
		pEnd = NULL;
		if (pObj->StreamInfo[1].Pos > 1024)
		{
			pEnd = MyBinBin(pObj->StreamInfo[1].pFrm+MAX_264HEAD_LEN, pObj->StreamInfo[1].Pos-MAX_264HEAD_LEN, Head264, HEAD_H264_LEN);
		}
		if (NULL == pEnd)
		{
			//如果不存在个别摄像机相邻两帧共用时间戳的情况，则只会走这里。
			memcpy(pESFrm, pObj->StreamInfo[1].pFrm, pObj->StreamInfo[1].Pos);
			*pESFrmLen = pObj->StreamInfo[1].Pos;
			pObj->StreamInfo[1].Pos = 0;
			*pTmStamp = 0; //暂时不使用PS流中的时间戳
			*pFrmType = ES_FRM_VIDEO;
			*pStreamType = pObj->StreamInfo[1].StreamType;
		}else
		{
			int tmpL1;
			tmpL1 = pEnd-pObj->StreamInfo[1].pFrm;
			memcpy(pESFrm, pObj->StreamInfo[1].pFrm, tmpL1);
			*pESFrmLen = tmpL1;
			memmove(pObj->StreamInfo[1].pFrm, pEnd, pObj->StreamInfo[1].Pos-tmpL1);
			pObj->StreamInfo[1].Pos -= tmpL1;
			*pTmStamp = 0; //暂时不使用PS流中的时间戳
			*pFrmType = ES_FRM_VIDEO;
			*pStreamType = pObj->StreamInfo[1].StreamType;
		}
		/*
FILE *fp;
fp = fopen("d:\\tmp\\ps1.bin", "ab");
fwrite(pESFrm, 1, *pESFrmLen, fp);
fclose(fp);
*/
		return 0;
	}
	return -1;
}

static int SearchXPS(unsigned char *pData, int DataLen, unsigned char XPSVal)
{
	int i, j;
	int MatchFlg;
	unsigned char tmpModel[3];

	tmpModel[0] = 0;
	tmpModel[1] = 0;
	tmpModel[2] = 1;
	for (i=0; i<DataLen-4; i++)
	{
		MatchFlg = 1;
		for (j=0; j<3; j++)
		{
			if (pData[i+j] != tmpModel[j])
			{
				MatchFlg = 0;
				break;
			}
		}
		if (MatchFlg)
		{
			if ((pData[i+3]&0x1f)==XPSVal)
			{
				return 1;
			}
		}
	}
	return 0;
}

int PS2ES_PopOut(PS2ES_HANDLE hPS2ES, unsigned char *pESFrm, int *pESFrmLen, unsigned int *pTmStamp, int *pFrmType, unsigned char *pStreamType)
{
	int rv;
	int tmpSPS, tmpPPS, tmpI, tmpP;
	int tmpLen;
	int tmpNewLen;
	unsigned char *pNewESFrm;

	PS2ES_OBJ *pObj = (PS2ES_OBJ *)hPS2ES;

	pNewESFrm = pObj->pLstESFrm+pObj->LstESPos;
	rv = PS2ES_PopOut_i(hPS2ES, pNewESFrm, &tmpNewLen, pTmStamp, pFrmType, pStreamType);
	if (0 != rv)
	{
		return rv;
	}
	if (*pFrmType == ES_FRM_AUDIO)
	{
		memcpy(pESFrm, pNewESFrm, tmpNewLen); //如果是音频，就把刚取到的数据原封不动的拷出去就行了
		*pESFrmLen = tmpNewLen;
		return 0;
	}
	//到这里说明视频。但最新数据还存放在输出缓冲区中

	tmpLen = tmpNewLen;
#define TMP_MAX_XPS_LEN		512
	if (tmpLen > TMP_MAX_XPS_LEN)
	{
		tmpLen = TMP_MAX_XPS_LEN; //没必要向后找的太多，节省些时间
	}

#if 0
	printf("\n");
	for (int i = 0; i < 50; i++) {
		printf("%02x ", (unsigned char)pNewESFrm[i]);
	}
	printf("\n");
#endif

	tmpP = SearchXPS(pNewESFrm, tmpLen, 1);
	tmpSPS = SearchXPS(pNewESFrm, tmpLen, 7);
//	tmpPPS = SearchXPS(pNewESFrm, tmpLen, 8);    //PPS和I帧头似乎不用找。出现这两种情况应该继续缓冲。
//	tmpI = SearchXPS(pNewESFrm, tmpLen, 5);

	if (tmpP||tmpSPS) //来了一个P帧或者
	{
		//说明是一个新的P帧来了，则缓冲的数据全输出出去
		if (0 == pObj->LstESPos)
		{
			pObj->LstESPos = tmpNewLen; //前面没有任何数据，则数据不用移动了
			return -2;
		}else
		{
			//前面的数据要输出出去
			memcpy(pESFrm, pObj->pLstESFrm, pObj->LstESPos);
			*pESFrmLen = pObj->LstESPos;
			memmove(pObj->pLstESFrm, pNewESFrm, tmpNewLen);
			pObj->LstESPos = tmpNewLen; 
			return 0;
		}
	}else
	{
		//不是一帧的开始，要继续缓冲起来
		pObj->LstESPos += tmpNewLen; 
		return -3;
	}
}
