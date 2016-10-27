/*======================================================
	Copyright(c) BesoVideo Ltd. 2011
	changjin.liu@besovideo.com
========================================================*/
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#ifdef _DEBUG
#include <stdio.h>
#endif
#if defined(_MSC_VER) & !defined(__cplusplus)
#ifndef inline
#    define inline __inline
#endif
#endif
#include "config.h"
#include "libavformat/avformat.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _MSC_VER
#define TIME_T __time32_t
#else
#define TIME_T time_t
#endif

#if !FF_API_TIMESTAMP
#undef _TM_DEFINED
#include <time.h>
#ifndef localtime_r
#define localtime_r(_Time, _Tm) ({ struct tm *___tmp_tm =       \
                        localtime((_Time)); \
                        if (___tmp_tm) {    \
                          *(_Tm) = *___tmp_tm;  \
                          ___tmp_tm = (_Tm);    \
                        }           \
                        ___tmp_tm;  })
#endif
#endif

#include "SAVContainer.h"
#include "libavutil/pixdesc.h"
#include "SAVPriv.h"

#ifndef NULL
#define NULL 0
#endif

#define CHECK_ARG(p) if(!(p)) return SAV_RESULT_E_INVALIDARG; \
				if(sizeof(*(p)) != (p)->iSize) \
					return SAV_RESULT_E_INVALIDARG 

struct _SAVContainer{	
	SAVContainer_Context* pContext;
	AVFormatContext* pAVFC;
	AVPacket avPacket;
#ifdef _WIN32
	char szFileNameUTF8[512];
	wchar_t wzFileName[256];
#endif	
};
typedef struct _SAVContainer SAVContainer;

static int avformat_inited = 0;

static SAV_Result ErrNum_FFMPEG2SAV(int iErrAV)
{
	SAV_Result iResult = SAV_RESULT_E_FAILED;
	switch(iErrAV){
		case AVERROR(ENOMEM):
			iResult = SAV_RESULT_E_ALLOCMEMFAILED;
		break;
		case AVERROR(EIO):
			iResult = SAV_RESULT_E_IO;
		break;
		case AVERROR(ENOENT):
		case AVERROR_STREAM_NOT_FOUND:
			iResult = SAV_RESULT_E_NOTFOUND;
		break;
		case AVERROR(EPERM):
		case AVERROR(EACCES):
			iResult = SAV_RESULT_E_NOTALLOWED;
		case AVERROR_EOF:
			iResult = SAV_RESULT_E_EOF;
		break;
		case AVERROR(EINVAL):
			iResult = SAV_RESULT_E_INVALIDDATA;
		break;
		case AVERROR_BSF_NOT_FOUND:
		case AVERROR_DECODER_NOT_FOUND:
		case AVERROR_DEMUXER_NOT_FOUND:
		case AVERROR_ENCODER_NOT_FOUND:
		case AVERROR_FILTER_NOT_FOUND:
		case AVERROR_MUXER_NOT_FOUND:
		case AVERROR_OPTION_NOT_FOUND:
		case AVERROR_PROTOCOL_NOT_FOUND:
			iResult = SAV_RESULT_E_UNSUPPORTED;
			break;
	}
	return iResult;
}


#if !FF_API_TIMESTAMP
//year: from  0001/1/1  
//mon: 1~12  
//http://blog.csdn.net/axx1611/article/details/1792827
static unsigned long utc_mktime (unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec)
{
	if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
		mon += 12;      /* Puts Feb last since it has leap day */
		year -= 1;
	}

	return (((
		(unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +  
			year*365 - 719499  
			)*24 + hour /* now have hours */  
		)*60 + min /* now have minutes */  
	)*60 + sec; /* finally seconds */  
}

static unsigned long utc_mktime_tm (struct tm* ptm)  
{
	return utc_mktime(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,  
				ptm->tm_hour,ptm->tm_min,ptm->tm_sec);    
}
#endif

static SAV_Result ParseExtra_H264(AVCodecContext* pAVC, SAVCodec_Context* pSAVC)
{
	SAV_TYPE_INT32 iWidth, iHeight;
	SAV_Result Result = SAVPriv_ParseH264Head(pSAVC->pExtraData, pSAVC->iExtraDataSize, &iWidth, &iHeight);
	if(SAV_Result_SUCCEEDED(Result)){
		pSAVC->stVideoParam.iWidth = pAVC->width = iWidth;
		pSAVC->stVideoParam.iHeight = pAVC->height = iHeight;
	}
	return Result;
}

static SAV_Result ParseExtra_AAC(AVCodecContext* pAVC, SAVCodec_Context* pSAVC)
{
	SAVPriv_AACExtra aacExtra;
	SAV_Result Result;
	memset(&aacExtra, 0, sizeof(aacExtra));
	Result = SAVPriv_ParseAACHead(pSAVC->pExtraData, pSAVC->iExtraDataSize, &aacExtra);
	if(SAV_Result_SUCCEEDED(Result)){
		pAVC->sample_rate = pSAVC->stAudioParam.iSampleRate = aacExtra.iSampleRate;
		pAVC->channels = pSAVC->stAudioParam.iChannelCount = aacExtra.iChannelCount;
		pAVC->extradata_size = aacExtra.iExtraDataSize;
		pAVC->extradata = aacExtra.pExtraData;
	}
	return Result;
}

/*
#ifndef _WIN32
typedef struct { 
  SAV_TYPE_INT16  wFormatTag; 
  SAV_TYPE_INT16  nChannels; 
  SAV_TYPE_INT32  nSamplesPerSec; 
  SAV_TYPE_INT32  nAvgBytesPerSec; 
  SAV_TYPE_INT16  nBlockAlign; 
  SAV_TYPE_INT16  wBitsPerSample; 
  SAV_TYPE_INT16  cbSize; 
} WAVEFORMATEX; 
#else
#include "windows.h"
#include "mmreg.h"
#endif
*/
#define GetInt16FromStream(p) (*(p) | (*((p)+1)<<8))
#define GetInt32FromStream(p) (GetInt16FromStream(p) | (GetInt16FromStream(p+2) << 16))

static SAV_Result ParseExtra_G726(AVCodecContext* pAVC, SAVCodec_Context* pSAVC)
{
	SAV_TYPE_UINT8* p = pSAVC->pExtraData;
	if(pSAVC->iExtraDataSize < 18){//sizeof(WAVEFORMATEX)
		return SAV_RESULT_E_INVALIDARG;
	}
	p += 2 ;//skip wFormatTag(0x0045)
	pAVC->channels = GetInt16FromStream(p);
	p += 2;
	pAVC->sample_rate = GetInt32FromStream(p);
	p += 4;
	pAVC->bit_rate = GetInt32FromStream(p) * 8;
	p += 4;
	pAVC->block_align = GetInt16FromStream(p);
	pAVC->bits_per_coded_sample = GetInt16FromStream(p);
	if(!pAVC->bits_per_coded_sample)
		pAVC->bits_per_coded_sample = (pAVC->bit_rate + pAVC->sample_rate/2)/pAVC->sample_rate;
	pAVC->extradata = NULL;
	pAVC->extradata_size = 0;
	return SAV_RESULT_S_OK;
}

typedef struct _CParseExtra{
	SAVCodec_ID  eCodeID;
	SAV_Result (*ParseExtra)(AVCodecContext* pAVC, SAVCodec_Context* pSAVC);
}CParseExtra;

static CParseExtra gParseExtra[] = {
	//video
	{SAVCODEC_ID_H264, ParseExtra_H264},

	//audio
	{SAVCODEC_ID_G726, ParseExtra_G726},
	{SAVCODEC_ID_AAC,  ParseExtra_AAC},

	{SAVCODEC_ID_NONE, NULL }
};


static SAV_Result ParseCodecExtra(AVCodecContext* pAVC, SAVCodec_Context* pSAVC)
{
	int i = 0;
	
	if(AVMEDIA_TYPE_VIDEO == pAVC->codec_type){
		if(pAVC->width > 0 && pAVC->height > 0)
			return SAV_RESULT_S_OK; //no need to parse
			
	}else if(AVMEDIA_TYPE_AUDIO == pAVC->codec_type){
		//bit_rate = pSAVC->iBitRate;
		if(pAVC->sample_rate > 0 && pAVC->channels > 0){
			if(AV_CODEC_ID_AAC == pAVC->codec_id){//check if ADTS			
				if( !((pAVC->extradata[0] == 0xFF && (pAVC->extradata[1] & 0xF0) == 0xF0) &&
					 pAVC->extradata_size >= 7) ){
				 	//NOT ADTS.do nothing
				 	return SAV_RESULT_S_OK;
				}
			}else if(AV_CODEC_ID_ADPCM_G726 == pAVC->codec_id){
				if(pAVC->bit_rate |  pAVC->bits_per_coded_sample){
					if(!pAVC->bits_per_coded_sample)
						pAVC->bits_per_coded_sample = (pAVC->bit_rate + pAVC->sample_rate/2)/ pAVC->sample_rate;
					else if(!pAVC->bit_rate){
						pAVC->bit_rate = pAVC->bits_per_coded_sample * pAVC->sample_rate;
					}
					//do not need extra
					pAVC->extradata = NULL;
					pAVC->extradata_size = 0;
					return SAV_RESULT_S_OK;
				}
			}else{
				//unkown codec. we can do nothing
				return SAV_RESULT_S_OK;
			}
		}
	}else{
		return SAV_RESULT_S_IGNORE;
	}
	if(!pAVC->extradata || pAVC->extradata_size <= 0){
		return SAV_RESULT_E_INVALIDARG; //can not parse
	}
	while(gParseExtra[i].ParseExtra){
		if(gParseExtra[i].eCodeID == pSAVC->eCodecID)
			return gParseExtra[i].ParseExtra(pAVC, pSAVC);
		i++;
	}
	
	return SAV_RESULT_E_UNSUPPORTED;
}

#ifdef _WIN32
static void MBCS2UTF8(SAVContainer_Context* pContext)
{
	int iWide;	
	wchar_t* sWide = pContext->pContainer->wzFileName; 
	char* szUTF8 = pContext->pContainer->szFileNameUTF8;
	const char* szMBCS = pContext->sFileName;
	int iUTF8Bytes;
	int bOK = 0;
	do{
		/* convert MBCS to wide chars */
		iWide = MultiByteToWideChar(CP_ACP, 0, szMBCS, -1, NULL, 0);
		if (iWide <= 0){
			break;
		}
		if(iWide > _countof(pContext->pContainer->wzFileName))
			iWide = _countof(pContext->pContainer->wzFileName);
		MultiByteToWideChar(CP_ACP, 0, szMBCS, -1, sWide, iWide);
		/* convert wide char to UTF-8 */
		iUTF8Bytes = WideCharToMultiByte(CP_UTF8, 0, sWide, -1, NULL, 0, NULL, NULL);
		if(iUTF8Bytes <= 0){
			break;
		}
		if(iUTF8Bytes > sizeof(pContext->pContainer->szFileNameUTF8))
			iUTF8Bytes = sizeof(pContext->pContainer->szFileNameUTF8);
		WideCharToMultiByte(CP_UTF8, 0, sWide, -1, szUTF8, iUTF8Bytes, NULL, NULL);
		bOK = 1;
	}while(0);
	if(!bOK){
#ifdef _MSC_VER
		strncpy_s(szUTF8, sizeof(pContext->pContainer->szFileNameUTF8)-1,szMBCS, iUTF8Bytes -1);
#else
		strncpy(szUTF8, szMBCS, iUTF8Bytes -1);
#endif
	}
}
#endif

LIBSAV_API SAV_Result SAVContainer_Open(SAVContainer_Context* pContext)
{
	SAV_Result Result = SAV_RESULT_E_FAILED;
	int i;
	//check arguments
	CHECK_ARG(pContext);
	if(!pContext->sFileName)
		return SAV_RESULT_E_INVALIDARG;
	if(pContext->bMux){
		if(pContext->iStreamCount <= 0 || !pContext->ppStreams){
			return SAV_RESULT_E_INVALIDARG;
		}
		for(i = 0; i <	pContext->iStreamCount; i++){
			if(!pContext->ppStreams[i]){
				return SAV_RESULT_E_INVALIDARG;
			}
		}
	}else{
		//TODO:check args
	}
	if(!avformat_inited){
		avformat_inited = 1;
		av_register_all();
	}
	pContext->pContainer = NULL;
	pContext->iFileSize = -1;
	pContext->iDuration = SAV_PTS_VALUE_NOSET;
	do{
		int iErrAV;		
		SAVContainer* pObj =  (SAVContainer*)av_mallocz(sizeof(SAVContainer));
		AVFormatContext* pAVFC = NULL;
		char* szFileName;
		if(!pObj){
			Result = SAV_RESULT_E_ALLOCMEMFAILED;
			break;
		}
		pContext->pContainer = pObj;
//#ifdef _WIN32
#if 0
		MBCS2UTF8(pContext);
		szFileName = pContext->pContainer->szFileNameUTF8;
#else
		szFileName = pContext->sFileName;
#endif
		if(pContext->bMux){//mux
			AVStream *pAVStream = NULL;
			SAVCodec_Context* pSAVC = NULL;
			const char* pFormatName = pContext->sContainerName[0] ? pContext->sContainerName:NULL;

			if((iErrAV = avformat_alloc_output_context2(&pAVFC,NULL,pFormatName,szFileName)) < 0){
				Result = ErrNum_FFMPEG2SAV(iErrAV);
				break;
			}
			if(!pContext->sContainerName[0]){
#ifdef _MSC_VER
				strncpy_s(pContext->sContainerName,sizeof(pContext->sContainerName)-1,pAVFC->oformat->name,sizeof(pContext->sContainerName)-1);
#else
				strncpy(pContext->sContainerName,pAVFC->oformat->name,sizeof(pContext->sContainerName)-1);
#endif
			}	
			for(i = 0; i < pContext->iStreamCount; i++){
				pAVStream = avformat_new_stream(pAVFC,NULL);
				if(!pAVStream){
					Result = SAV_RESULT_E_ALLOCMEMFAILED;
					break;
				}
				pSAVC = pContext->ppStreams[i];
				pAVStream->codec->extradata = pSAVC->pExtraData;
				pAVStream->codec->extradata_size = pSAVC->iExtraDataSize;
				pAVStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
				pAVStream->codec->codec_id = pSAVC->eCodecID;
			#if 1
				pAVStream->codec->codec_type = pSAVC->eMediaType;
			#else
				pAVCodec = avcodec_find_encoder(pSAVC->eCodecID);
				if(!pAVCodec)
					pAVCodec = avcodec_find_decoder(pSAVC->eCodecID);
				if(!pAVCodec){
					//the codec is not built into libavcodec. hack by codec id
					if(pSAVC->eCodecID < (SAVCodec_ID)CODEC_ID_FIRST_AUDIO)
						pAVStream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
					else if(pSAVC->eCodecID < (SAVCodec_ID)CODEC_ID_FIRST_SUBTITLE)
						pAVStream->codec->codec_type = AVMEDIA_TYPE_AUDIO;
					else if(pSAVC->eCodecID < (SAVCodec_ID)CODEC_ID_FIRST_UNKNOWN)
						pAVStream->codec->codec_type = AVMEDIA_TYPE_SUBTITLE;
					else{
						Result = SAV_RESULT_E_INVALIDARG;
						break;
					}
				}else{
					pAVStream->codec->codec_type = pAVCodec->type;
				}
			#endif
				pAVStream->codec->codec_id = pSAVC->eCodecID;
				pAVStream->codec->time_base= *((AVRational*)&(pSAVC->TimeBase));
				if(AVMEDIA_TYPE_VIDEO == pAVStream->codec->codec_type){
					pAVStream->codec->width = pSAVC->stVideoParam.iWidth;
					pAVStream->codec->height = pSAVC->stVideoParam.iHeight;
					//pAVStream->codec->pix_fmt = (enum PixelFormat)pSAVC->stVideoParam.ePixelFmt;
				}else if(AVMEDIA_TYPE_AUDIO == pAVStream->codec->codec_type){					
					pAVStream->codec->bit_rate = pSAVC->iBitRate;
					pAVStream->codec->sample_rate = pSAVC->stAudioParam.iSampleRate;
					pAVStream->codec->channels = pSAVC->stAudioParam.iChannelCount;
					if(pAVStream->codec->sample_rate > 0)
						pAVStream->codec->bits_per_coded_sample = pAVStream->codec->bit_rate/pAVStream->codec->sample_rate;
				}else{
					//subtitle.do nothing
				}
				if(SAV_Result_FAILED(ParseCodecExtra(pAVStream->codec, pSAVC))){
					Result = SAV_RESULT_E_INVALIDDATA;
					break;
				}
				
			}
		#if FF_API_TIMESTAMP
			pAVFC->timestamp = pContext->iCreationTime;
		#else
			{
				char sTimeBuf[64];
				TIME_T tSec, uSec;		
				struct tm tmLocal;
				tSec = (TIME_T)(pContext->iCreationTime / SAV_TIME_BASE);
				uSec = (TIME_T)(pContext->iCreationTime % SAV_TIME_BASE);				
		#ifdef _MSC_VER
				_localtime32_s(&tmLocal,&tSec);
				sprintf_s(sTimeBuf,sizeof(sTimeBuf)-1,"%d-%02d-%02d %02d:%02d:%02d.%06d",
					tmLocal.tm_year+1900,tmLocal.tm_mon+1,tmLocal.tm_mday,
					tmLocal.tm_hour,tmLocal.tm_min,tmLocal.tm_sec,uSec); 				
		#else
				localtime_r(&tSec,&tmLocal);
				sprintf(sTimeBuf,"%d-%02d-%02d %02d:%02d:%02d.%06d",
					tmLocal.tm_year+1900,tmLocal.tm_mon+1,tmLocal.tm_mday,
					tmLocal.tm_hour,tmLocal.tm_min,tmLocal.tm_sec,uSec); 				
		#endif				
				av_dict_set(&pAVFC->metadata, "creation_time", sTimeBuf, 0);
			}
		#endif
			//set metadata
			if(pContext->pMetaData){
				SAVDict_Entry* pEntry = NULL;
				while(pEntry = SAVDict_Enum(pContext->pMetaData,pEntry))
					av_dict_set(&pAVFC->metadata, pEntry->pKey, pEntry->pValue, 0);
			}
			if ((iErrAV = avio_open(&pAVFC->pb, szFileName, AVIO_FLAG_WRITE)) < 0) {
				Result = ErrNum_FFMPEG2SAV(iErrAV);
				break;
			}
			if ((iErrAV = avformat_write_header(pAVFC, NULL)) < 0 ){
				Result = ErrNum_FFMPEG2SAV(iErrAV);
				break;
			}
			//don't free pAVStream->codec->extradata since it's allocated by user
			for(i = 0; i <	pContext->iStreamCount; i++){
				if(pAVFC->streams[i]->codec->extradata == pContext->ppStreams[i]->pExtraData){
					pAVFC->streams[i]->codec->extradata = NULL;
					pAVFC->streams[i]->codec->extradata_size = 0;
				}
			}
			
			Result = SAV_RESULT_S_OK;
			
		}else{//demux
			
			AVInputFormat* pAVIF = NULL;
			SAVCodec_Context** ppStreams = NULL;
			SAV_TYPE_INT32*  piStreamSeekIndexCount = NULL;
		
			pContext->iFileSize = 0;
			pContext->iStreamCount = 0;
			pContext->ppStreams = NULL;
			pContext->piStreamSeekIndexCount = NULL;
			pContext->iCreationTime = SAV_PTS_VALUE_NOSET;
			if(pContext->sContainerName[0]){
				pAVIF = av_find_input_format(pContext->sContainerName);
			}			
			if((iErrAV = avformat_open_input(&pAVFC,szFileName,pAVIF,(AVDictionary**)(&pContext->pMetaData))) < 0){
				Result = ErrNum_FFMPEG2SAV(iErrAV);
				break;
			}
			if(pAVFC->nb_streams <= 0){
				Result = SAV_RESULT_E_FAILED;
				break;
			}
			
			//avformat_find_stream_info?
			pContext->iStreamCount = pAVFC->nb_streams;
			ppStreams = (SAVCodec_Context**)av_mallocz(sizeof(SAVCodec_Context*) * pAVFC->nb_streams);
			if(!ppStreams){
				Result = SAV_RESULT_E_ALLOCMEMFAILED;
				break;
			}
			
			piStreamSeekIndexCount = (SAV_TYPE_INT32*)av_mallocz(sizeof(SAV_TYPE_INT32) * pAVFC->nb_streams);
			if(!piStreamSeekIndexCount){
				Result = SAV_RESULT_E_ALLOCMEMFAILED;
				break;
			}
			pContext->ppStreams = ppStreams;
			pContext->piStreamSeekIndexCount = piStreamSeekIndexCount;
			for(i = 0; i < (int)pAVFC->nb_streams; i++){
				ppStreams[i] = (SAVCodec_Context*)av_mallocz(sizeof(SAVCodec_Context));
				if(!ppStreams[i]){
					Result = SAV_RESULT_E_ALLOCMEMFAILED;
					break;
				}
				piStreamSeekIndexCount[i] = pAVFC->streams[i]->nb_index_entries;
				ppStreams[i]->iSize = sizeof(*ppStreams[i]);
				if(pAVFC->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
					ppStreams[i]->stVideoParam.iWidth = pAVFC->streams[i]->codec->width;
					ppStreams[i]->stVideoParam.iHeight = pAVFC->streams[i]->codec->height;
						if(!pAVFC->streams[i]->codec->time_base.num)
						pAVFC->streams[i]->codec->time_base = pAVFC->streams[i]->time_base;
				}else if(pAVFC->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
					ppStreams[i]->iBitRate = pAVFC->streams[i]->codec->bit_rate;
					ppStreams[i]->stAudioParam.iSampleRate = pAVFC->streams[i]->codec->sample_rate;
					ppStreams[i]->stAudioParam.iChannelCount = pAVFC->streams[i]->codec->channels;
					ppStreams[i]->stAudioParam.eSampleFormat = pAVFC->streams[i]->codec->sample_fmt;
				}else if(pAVFC->streams[i]->codec->codec_type == AVMEDIA_TYPE_SUBTITLE){
					if(!pAVFC->streams[i]->codec->time_base.num)
						pAVFC->streams[i]->codec->time_base = pAVFC->streams[i]->time_base;
				}
				ppStreams[i]->eCodecID = pAVFC->streams[i]->codec->codec_id;
				ppStreams[i]->eMediaType = pAVFC->streams[i]->codec->codec_type;
		 		ppStreams[i]->TimeBase = *(SAV_Rational*)&pAVFC->streams[i]->codec->time_base;
		 		ppStreams[i]->pExtraData = pAVFC->streams[i]->codec->extradata;
				ppStreams[i]->iExtraDataSize = pAVFC->streams[i]->codec->extradata_size;
				//TODO: convert H.264 extradata to NAL
			}
			if(i < (int)pAVFC->nb_streams){
				break;
			}
			if(!pContext->sContainerName[0]){
#ifdef _MSC_VER
				strcpy_s(pContext->sContainerName,sizeof(pContext->sContainerName)-1,pAVFC->iformat->name);
#else
				strcpy(pContext->sContainerName,pAVFC->iformat->name);
#endif
			}
			pContext->pMetaData = (SAVDictionary*)pAVFC->metadata;
			pContext->iDuration = pAVFC->duration;
		#if FF_API_FILESIZE
			pContext->iFileSize = pAVFC->file_size;
		#endif
			if(!pContext->iFileSize)
				pContext->iFileSize = avio_size(pAVFC->pb);
		
		#if FF_API_TIMESTAMP
			pContext->iCreationTime = pAVFC->timestamp;
		#else
			if(pAVFC->metadata){
				AVDictionaryEntry *t = av_dict_get(pAVFC->metadata, "creation_time", NULL, 0);
				if(t){
					struct tm tm1={0};
					int iUsec = 0;
		#if defined(_XOPEN_SOURCE)
					strptime(t->value,"%Y - %m - %d %T",&tm1);
		#else
			#ifdef _MSC_VER
					sscanf_s
			#else
					sscanf
			#endif
					(t->value,"%d-%d-%d %d:%d:%d.%06d",&tm1.tm_year,&tm1.tm_mon,
					&tm1.tm_mday,&tm1.tm_hour,&tm1.tm_min,&tm1.tm_sec,&iUsec);
					tm1.tm_year -= 1900;
					tm1.tm_mon -= 1;
		#endif
			 		//mktime will consider timezone to return localtime. But we need UTC
					pContext->iCreationTime = ((SAV_TYPE_INT64)utc_mktime_tm(&tm1)) * 1000000 + iUsec;
				}
			}
		#endif
			av_init_packet(&pContext->pContainer->avPacket);
		}
		pContext->pContainer->pAVFC = pAVFC;
		Result = SAV_RESULT_S_OK;
	}while(0);
	
	if(Result != SAV_RESULT_S_OK){
		SAVContainer_Close(pContext);
	}
	return Result;
}

LIBSAV_API SAV_Result SAVContainer_Process(SAVContainer_Context* pContext, SAV_Packet* pPacket)
{
	SAV_Result Result = SAV_RESULT_E_FAILED;
	SAVContainer* pObj;
	AVPacket* pAVPkt;
	AVStream* pAVStream;
	int iErrAV;
	CHECK_ARG(pContext);
	CHECK_ARG(pPacket);
	
	pObj = pContext->pContainer;
	if(!pObj)
		return SAV_RESULT_E_NOTOPENED;
	pAVPkt = &pObj->avPacket;
	if(pContext->bMux){
		int iOffset = 0;
		if(pPacket->iStreamIndex < 0 || pPacket->iStreamIndex >= pContext->iStreamCount){
			return SAV_RESULT_E_INVALIDARG;
		}
		pAVStream = pObj->pAVFC->streams[pPacket->iStreamIndex];
		av_init_packet(pAVPkt);
		if(pPacket->iFlags & SAV_PKT_FLAG_KEY){
			pAVPkt->flags |= AV_PKT_FLAG_KEY;
			if(pObj->pAVFC->oformat->flags & AVFMT_GLOBALHEADER){
				if(pAVStream->codec->codec_id == AV_CODEC_ID_H264){
					//remove the SPS/PPS
					int iSearchCount = (pPacket->iDataSize - 4) > 256 ? 256 : (pPacket->iDataSize - 4);
					int i = 0;
					SAV_TYPE_UINT8 iType = 0;
					SAV_TYPE_UINT8* p = pPacket->pData;
					while(i < iSearchCount){
						if(!(p[i] | p[i+1]) && p[i+2] == 1){//start prefix found
							iType = p[i+3] & 0x1F;
							if(iType < 6){
								break;
							}else{
								i += 3;
							}
						}else{
							i++;
						}	
					}
					if(iType){
						if(i > 0 && p[i-1] == 0)
							i--;
						iOffset = i;
					}
				}
			}
		}
		pAVPkt->data = pPacket->pData + iOffset;
		pAVPkt->size = pPacket->iDataSize - iOffset;
		if(pAVStream->codec->codec_id == AV_CODEC_ID_H264){
			SAV_TYPE_UINT32 iAVCLen = pAVPkt->size - 4;
			pAVPkt->data[0] = (SAV_TYPE_UINT8)(iAVCLen >> 24);
			pAVPkt->data[1] = (SAV_TYPE_UINT8)(iAVCLen >> 16);
			pAVPkt->data[2] = (SAV_TYPE_UINT8)(iAVCLen >>  8);
			pAVPkt->data[3] = (SAV_TYPE_UINT8)(iAVCLen >>  0);
		}
		pAVPkt->stream_index = pPacket->iStreamIndex;
		if(pAVStream->time_base.den > 0 && pAVStream->time_base.num > 0 && pPacket->iPTS >= 0){
			pAVPkt->pts = pAVPkt->dts = av_rescale(pPacket->iPTS, pAVStream->time_base.den, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVStream->time_base.num);
			pAVPkt->duration = (int)av_rescale(pPacket->iDuration, pAVStream->time_base.den, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVStream->time_base.num);
		}
		if(pAVStream->codec->codec_id == AV_CODEC_ID_TEXT)//do not care pAVStream->time_base?
			pAVPkt->convergence_duration = pAVPkt->duration;
			
		if((iErrAV = av_interleaved_write_frame(pObj->pAVFC,pAVPkt)) < 0){
			Result = ErrNum_FFMPEG2SAV(iErrAV);
		}else
			Result = SAV_RESULT_S_OK;
	}else{//demux
		
		pAVPkt->pts = SAV_PTS_VALUE_NOSET;
		if(pAVPkt->data){
			av_free_packet(pAVPkt);
			av_init_packet(pAVPkt);
		}
		
		if((iErrAV = av_read_frame(pObj->pAVFC,pAVPkt)) < 0){
			Result = ErrNum_FFMPEG2SAV(iErrAV);			
		}else{
			pAVStream = pObj->pAVFC->streams[pPacket->iStreamIndex];
			if(pAVStream->time_base.den > 0 && pAVStream->time_base.num > 0 && pAVPkt->pts >= 0){
				pPacket->iPTS = av_rescale(pAVPkt->pts, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVStream->time_base.num, pAVStream->time_base.den);
				pPacket->iDuration = av_rescale(pAVPkt->duration, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVStream->time_base.num, pAVStream->time_base.den);
			}
			pPacket->pData = pAVPkt->data;
			pPacket->iDataSize = pAVPkt->size;
			pPacket->iFlags =  pAVPkt->flags;
			pPacket->iStreamIndex = pAVPkt->stream_index;
			//TODO:should we convert H.264 extradata to NAL?
			Result = SAV_RESULT_S_OK;
		}
	}

	return Result;
}

LIBSAV_API SAV_Result SAVContainer_Control(SAVContainer_Context* pContext, SAVContainer_ControlCmd cmd, SAVContainer_ControlParam* pParam)
{
	SAV_Result Result = SAV_RESULT_E_FAILED;
	if(!pContext || !pContext->pContainer || !pContext->pContainer->pAVFC)
		return SAV_RESULT_E_INVALIDARG;
	switch(cmd){
		case SAVCONTAINER_CONTROLCMD_SEEK:
		{
			SAV_TYPE_INT64 iTargetTs, iMinTs, iMaxTs;
			int iErrAV;
			SAVContainer_SeekOpt* pOpt = (SAVContainer_SeekOpt*)pParam;
			AVFormatContext* pAVFC = pContext->pContainer->pAVFC;
			if(!pOpt || pContext->bMux
			|| pOpt->cp.iSize != sizeof(*pOpt)
			|| pOpt->iStreamIndex >= pContext->iStreamCount || pOpt->iStreamIndex < SAV_STREAM_INDEX_DEFAULT){
				Result = SAV_RESULT_E_INVALIDARG;
				break;
			}
			if(pOpt->iFlags & (SAVCONTAINER_SEEK_FLAG_BYTE | SAVCONTAINER_SEEK_FLAG_ANY)){
				Result = SAV_RESULT_E_UNSUPPORTED;
				break;
			}
			iMinTs = pOpt->iMinTs;
			iMaxTs = pOpt->iMaxTs;
			iTargetTs = pOpt->iTargetTs;
			if(!(pOpt->iFlags & SAVCONTAINER_SEEK_FLAG_FRAME)){
				if(iMinTs > iTargetTs || iTargetTs > iMaxTs){
					Result = SAV_RESULT_E_INVALIDARG;
					break;
				}
				if(pOpt->iStreamIndex != SAV_STREAM_INDEX_DEFAULT){
					//convert the timestamp in SAV_TIME_BASE to stream
					AVStream* pAVStream = pAVFC->streams[pOpt->iStreamIndex];
					iTargetTs = av_rescale(iTargetTs, pAVStream->time_base.den, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVStream->time_base.num);
					if(iMinTs > 0)
						iMinTs = av_rescale(iMinTs, pAVStream->time_base.den, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVStream->time_base.num);
					iMaxTs = av_rescale(iMaxTs, pAVStream->time_base.den, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVStream->time_base.num);
				}
			}
			if(iMinTs < 0)
				iMinTs = 0;
			if((iErrAV = avformat_seek_file(pAVFC, pOpt->iStreamIndex, iMinTs, iTargetTs, iMaxTs, pOpt->iFlags)) < 0){
				Result = ErrNum_FFMPEG2SAV(iErrAV);
			}else{
				Result = SAV_RESULT_S_OK;
			}
			if(1){//update index count. Some demuxer such as mkv defer the index retrieval to seek.
				int iDefaultIndex = pOpt->iStreamIndex;
				if(iDefaultIndex == SAV_STREAM_INDEX_DEFAULT){
					iDefaultIndex = av_find_default_stream_index(pAVFC);
				}
				if(iDefaultIndex >= 0){
					if(pContext->piStreamSeekIndexCount[iDefaultIndex] != pAVFC->streams[iDefaultIndex]->nb_index_entries){
						unsigned int i;
						for(i = 0; i < pAVFC->nb_streams; i++){
							pContext->piStreamSeekIndexCount[i] = pAVFC->streams[i]->nb_index_entries;
						}
					}
				}
			}
		}
		break;
		
		default:
		Result = SAV_RESULT_E_UNSUPPORTED;
		break;
	}
	return Result;
}

LIBSAV_API SAV_Result SAVContainer_Close(SAVContainer_Context* pContext)
{	
	SAVContainer* pObj;
	CHECK_ARG(pContext);
	pObj = pContext->pContainer;
	if(!pObj)
		return SAV_RESULT_S_OK;
	if(!pObj->pAVFC)
		return SAV_RESULT_S_OK;
	if(pContext->bMux){
		int Result = SAV_RESULT_S_OK;
		int iErrAV = av_write_trailer(pObj->pAVFC);
		if(iErrAV < 0){
			Result = ErrNum_FFMPEG2SAV(iErrAV);
		}
		iErrAV = avio_close(pObj->pAVFC->pb);
		if(iErrAV < 0){
			Result = ErrNum_FFMPEG2SAV(iErrAV);
		}
		if(pContext->pMetaData)//for mux, pMedaData is set by user
			SAVDict_Free(&pContext->pMetaData);
		avformat_free_context(pObj->pAVFC);
		memset(pContext, 0, sizeof(*pContext));
		return Result;
	}else{
		avformat_close_input(&pObj->pAVFC);
		if(pContext->piStreamSeekIndexCount){
			av_freep(&pContext->piStreamSeekIndexCount);
		}
		if(pContext->iStreamCount > 0){
			int i;
			for(i = 0; i < pContext->iStreamCount; i++){
				if(pContext->ppStreams[i]){
					av_freep(&pContext->ppStreams[i]);
				}
			}
			av_freep(&pContext->ppStreams);
		}
		if(pObj->avPacket.data){
			av_free_packet(&pObj->avPacket);
		}
		av_freep(&pObj);
		return SAV_RESULT_S_OK;
	}
}

SAV_Result SAVPriv_ParseH264Head(SAV_TYPE_UINT8* pData, SAV_TYPE_INT32 iDataLen,
							SAV_TYPE_INT32* piWidth, SAV_TYPE_INT32* piHeight)
{
	SAV_Result Result = SAV_RESULT_S_OK;
	return Result;
}

SAV_Result SAVPriv_ParseAACHead(SAV_TYPE_UINT8* pData, SAV_TYPE_INT32 iDataLen,
							SAVPriv_AACExtra* pAACExtra)
{
	return SAV_RESULT_S_OK;
}
