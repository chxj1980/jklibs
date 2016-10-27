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
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"

#include "SAVCodec.h"

#ifndef NULL
#define NULL 0
#endif

#define CHECK_ARG(p) if(!(p)) return SAV_RESULT_E_INVALIDARG; \
				if(sizeof(*(p)) != (p)->iSize) \
					return SAV_RESULT_E_INVALIDARG 

struct _SAVCodec{	
	AVCodecContext* pAVCodecCtx;
};
typedef struct _SAVCodec SAVCodec;

static int avcodec_inited = 0;

LIBSAV_API SAV_Result SAVCodec_Open(SAVCodec_Context* pContext)
{
	SAV_Result Result = SAV_RESULT_E_FAILED;
	CHECK_ARG(pContext);
	if(!avcodec_inited){
		avcodec_inited = 1;		
		avcodec_register_all();
	}
	pContext->pCodec = NULL;
	do{
		AVCodecContext* pAVCodecCtx = NULL;
		AVCodec* pAVCodec = NULL;
		SAVCodec* pObj;		
		pObj =  (SAVCodec*)av_mallocz(sizeof(SAVCodec));
		
		if(!pObj){
			Result = SAV_RESULT_E_ALLOCMEMFAILED;
			break;
		}
		pContext->pCodec = pObj;
		if(pContext->bEncode)
			pAVCodec = avcodec_find_encoder(pContext->eCodecID);
		else
			pAVCodec = avcodec_find_decoder(pContext->eCodecID);
		if(!pAVCodec){
			Result = SAV_RESULT_E_UNSUPPORTED;
			break;
		}
		pAVCodecCtx = avcodec_alloc_context3(pAVCodec);
		if(!pAVCodecCtx){
			Result = SAV_RESULT_E_ALLOCMEMFAILED;
			break;
		}
		pObj->pAVCodecCtx = pAVCodecCtx;
		pAVCodecCtx->flags |= CODEC_FLAG_LOW_DELAY;
		if(pContext->bEncode){
			if((SAV_MediaType)pAVCodec->type != pContext->eMediaType){
				Result = SAV_RESULT_E_INVALIDARG;
				break;
			}
		}else{
			pContext->eMediaType = pAVCodec->type;
		}
		
		if(pContext->eMediaType == (SAV_MediaType)AVMEDIA_TYPE_VIDEO){
			if(pContext->bEncode){
				if(pContext->stVideoParam.iWidth <= 0 ||  pContext->stVideoParam.iHeight <= 0
					|| pContext->stVideoParam.ePixelFmt <= SAV_PIX_FMT_NONE
					|| pContext->stVideoParam.ePixelFmt >= (SAV_PixelFormat)PIX_FMT_NB){
					Result = SAV_RESULT_E_INVALIDARG;
					break;
				}
			}
			pAVCodecCtx->width = pContext->stVideoParam.iWidth;
			pAVCodecCtx->height = pContext->stVideoParam.iHeight;
			pAVCodecCtx->bit_rate = pContext->iBitRate;
   			pAVCodecCtx->gop_size = pContext->stVideoParam.iGOP;
   			pAVCodecCtx->time_base= *((AVRational*)&(pContext->TimeBase));
    		pAVCodecCtx->max_b_frames = pContext->stVideoParam.iMaxBFrames;
    		pAVCodecCtx->pix_fmt = (enum PixelFormat)pContext->stVideoParam.ePixelFmt;
    		if(pContext->bEncode){
    			if(SAVCODEC_ID_MJPEG == pContext->eCodecID){
    				SAV_TYPE_BOOL bSupported = SAV_BOOL_TRUE;
    				switch(pContext->stVideoParam.ePixelFmt){
    					case SAV_PIX_FMT_YUV420P:
    						pAVCodecCtx->pix_fmt = PIX_FMT_YUVJ420P;
    						break;
    					case SAV_PIX_FMT_YUV422P:
    						pAVCodecCtx->pix_fmt = PIX_FMT_YUVJ422P;
    						break;
    					case PIX_FMT_YUVJ420P:
    					case PIX_FMT_YUVJ422P:
    						break;
    					default:
    						bSupported = SAV_BOOL_FALSE;    						
    				}
    				if(!bSupported){
    					Result = SAV_RESULT_E_UNSUPPORTED;
						break;
    				}
    				pAVCodecCtx->flags &= ~CODEC_FLAG_LOW_DELAY;
    				pAVCodecCtx->flags |= CODEC_FLAG_QSCALE;
    				pAVCodecCtx->global_quality = FF_QP2LAMBDA * 5;//default value
    				pAVCodecCtx->max_b_frames = 0;
    				if(!pAVCodecCtx->time_base.den || !pAVCodecCtx->time_base.num){
    					pAVCodecCtx->time_base.den = 25;
    					pAVCodecCtx->time_base.num = 1;
    				}
    			}
    		}
		}else if(pContext->eMediaType == (SAV_MediaType)AVMEDIA_TYPE_AUDIO){//audio
			pAVCodecCtx->bit_rate = pContext->iBitRate;
			pAVCodecCtx->time_base= *((AVRational*)&(pContext->TimeBase));
    		pAVCodecCtx->sample_rate = pContext->stAudioParam.iSampleRate;
    		pAVCodecCtx->channels = pContext->stAudioParam.iChannelCount;
    		pAVCodecCtx->sample_fmt = pContext->stAudioParam.eSampleFormat;
    		pAVCodecCtx->frame_size = pContext->stAudioParam.iFrameSize;
    		if(pAVCodecCtx->sample_rate)//for G.726
    			pAVCodecCtx->bits_per_coded_sample = pAVCodecCtx->bit_rate/pAVCodecCtx->sample_rate;
		}else{
			Result = SAV_RESULT_E_INVALIDARG;
			break;
		}
		if(!pContext->bEncode){
			pAVCodecCtx->extradata = pContext->pExtraData;
			pAVCodecCtx->extradata_size = pContext->iExtraDataSize;
			//if(pAVCodec->capabilities&CODEC_CAP_TRUNCATED)
		      //  pAVCodecCtx->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
		      if(SAVCODEC_ID_G726 == pContext->eCodecID){
		      	   pContext->stAudioParam.iSampleRate = pAVCodecCtx->sample_rate = 8000;
		      	   pContext->stAudioParam.iChannelCount = pAVCodecCtx->channels = 1;
		      	   pContext->stAudioParam.eSampleFormat = pAVCodecCtx->sample_fmt = SAV_SAMPLE_FMT_S16;
		      	   
		      }
		}
		#if FF_API_ER
		pAVCodecCtx->error_recognition = FF_ER_CAREFUL;
		#endif
		
		if (avcodec_open2(pAVCodecCtx, pAVCodec, (AVDictionary**)&pContext->pOptions) < 0){
			Result = SAV_RESULT_E_FAILED;
			break;
		}
		if(pContext->bEncode){
			pContext->pExtraData = pAVCodecCtx->extradata;
			pContext->iExtraDataSize = pAVCodecCtx->extradata_size;
		}
		if(SAVCODEC_ID_G726 == pContext->eCodecID){
			pObj->pAVCodecCtx->frame_size = pContext->stAudioParam.iFrameSize;
		}
		if(pContext->stAudioParam.iFrameSize != pAVCodecCtx->frame_size){
			pContext->stAudioParam.iFrameSize = pAVCodecCtx->frame_size;
		}
		Result = SAV_RESULT_S_OK;
	}while(0);
	
	if(Result != SAV_RESULT_S_OK){
		SAVCodec_Close(pContext);
	}
	return Result;
}


LIBSAV_API SAV_TYPE_INT32 SAVCodec_Process(SAVCodec_Context* pContext, SAV_Frame* pFrame, SAV_Packet* pPacket)
{
	SAV_TYPE_INT32 iRet = SAV_RESULT_E_FAILED;
	SAVCodec* pObj;
	AVCodecContext* pAVContext;
	CHECK_ARG(pContext);
	CHECK_ARG(pFrame);
	CHECK_ARG(pPacket);
	
	if(!(pObj = pContext->pCodec))
		return SAV_RESULT_E_NOTOPENED;
	if(!(pAVContext = pObj->pAVCodecCtx))
		return SAV_RESULT_E_NOTOPENED;
	
	if(pContext->bEncode){
		AVFrame avFrame;
		pPacket->iPTS = SAV_PTS_VALUE_NOSET;
		if(pContext->eMediaType == (SAV_MediaType)AVMEDIA_TYPE_VIDEO){
			int i;
			avcodec_get_frame_defaults(&avFrame);
			if(pAVContext->time_base.num > 0 && pAVContext->time_base.den > 0 && pFrame->iPTS >= 0)
				avFrame.pts = av_rescale(pFrame->iPTS, pAVContext->time_base.den, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVContext->time_base.num);
			avFrame.pict_type = (enum AVPictureType)pFrame->ePictType;
			for(i = 0; i < SAV_IMG_COMPONENT; i++){
				avFrame.data[i] = pFrame->ppData[i];
				avFrame.linesize[i] = pFrame->iDataSize[i];
			}
			if(SAVCODEC_ID_MJPEG == pContext->eCodecID){
				avFrame.quality = pAVContext->global_quality;
			}
			iRet = avcodec_encode_video(pAVContext, pPacket->pData, pPacket->iDataSize, &avFrame);
			if(iRet >= 0){
				pPacket->ePictType = (SAV_PictureType)avFrame.pict_type;
			}
		}else if(pContext->eMediaType == (SAV_MediaType)AVMEDIA_TYPE_AUDIO){//audio
			int iGot = 0;
			AVPacket avPkt;
			av_init_packet(&avPkt);			
			avPkt.data = pPacket->pData;
			avPkt.size = pPacket->iDataSize;
			avcodec_get_frame_defaults(&avFrame);
			avFrame.nb_samples = pContext->stAudioParam.iFrameSize;
			avFrame.data[0] = pFrame->ppData[0];
			avFrame.extended_data = avFrame.data;
			avFrame.pts = pFrame->iPTS;
			iRet = avcodec_encode_audio2(pAVContext, &avPkt, &avFrame, &iGot);
			if(!iRet){
				iRet = avPkt.size;
			}else{
				iRet = SAV_RESULT_E_FAILED;
			}
			//iRet = avcodec_encode_audio(pAVContext, pPacket->pData, pPacket->iDataSize, (const short *)pFrame->ppData[0]);
		}else{
			return SAV_RESULT_E_INVALIDARG;
		}
		if(iRet >= 0){
			if(pAVContext->coded_frame){
				if(pAVContext->coded_frame->key_frame)
					pPacket->iFlags |= SAV_PKT_FLAG_KEY;
				else
					pPacket->iFlags &= ~SAV_PKT_FLAG_KEY;
				if(pAVContext->coded_frame->pts != SAV_PTS_VALUE_NOSET){
					pPacket->iPTS = av_rescale(pAVContext->coded_frame->pts, SAV_TIME_BASE * (SAV_TYPE_INT64)pAVContext->time_base.num, pAVContext->time_base.den);
				}
			}else{
				pPacket->iFlags |= SAV_PKT_FLAG_KEY;
			}
			
			if(pPacket->iPTS == SAV_PTS_VALUE_NOSET && pFrame->iPTS >= 0)
				pPacket->iPTS = pFrame->iPTS;
			
		}
	}else{
		AVPacket avpkt;
		AVFrame avFrame;
		int got_frame;
    	av_init_packet(&avpkt);
    	avpkt.data = pPacket->pData;
    	avpkt.size = pPacket->iDataSize;
    	avpkt.pts = pPacket->iPTS;
    	avpkt.duration = (int)pPacket->iDuration;
    	pFrame->iPTS = SAV_PTS_VALUE_NOSET;
		if(pContext->eMediaType == (SAV_MediaType)AVMEDIA_TYPE_VIDEO){
			int i;
			avcodec_get_frame_defaults(&avFrame);
			pFrame->iFlags &= ~SAV_PKT_FLAG_KEY;
			iRet = avcodec_decode_video2(pAVContext, &avFrame, &got_frame, &avpkt);
			if(iRet >= 0 && got_frame){
				pContext->stVideoParam.iWidth = pAVContext->width;
				pContext->stVideoParam.iHeight = pAVContext->height;
				pContext->TimeBase = *((SAV_Rational*)&(pAVContext->time_base));				
				pContext->stVideoParam.ePixelFmt = pAVContext->pix_fmt;
				pFrame->ePictType = avFrame.pict_type;
				pFrame->iPTS = avFrame.best_effort_timestamp;
				if(avFrame.key_frame)
					pFrame->iFlags |= SAV_PKT_FLAG_KEY;
				else
					pFrame->iFlags &= ~SAV_PKT_FLAG_KEY;

				for(i = 0; i < SAV_IMG_COMPONENT; i++){
					pFrame->ppData[i] = avFrame.data[i];
					pFrame->iDataSize[i] = avFrame.linesize[i];
				}
    		}else{
    			pFrame->ePictType = SAV_PICTURE_TYPE_NONE;
    			for(i = 0; i < SAV_IMG_COMPONENT; i++){
					pFrame->ppData[i] = NULL;
					pFrame->iDataSize[i] = 0;
				}
				iRet = SAV_RESULT_E_FAILED;
    		}
		}else{//audio			
			if (HAVE_MMX){
				if(((long)pFrame->ppData[0] ) & 31){
					return SAV_RESULT_E_MEMALIGNMENT;
				}
			}

			avcodec_get_frame_defaults(&avFrame);
			iRet = avcodec_decode_audio4(pAVContext, &avFrame, &got_frame, &avpkt);
			//iRet = avcodec_decode_audio3(pAVContext, (int16_t *)pFrame->ppData[0], &pFrame->iDataSize[0], &avpkt);			
			if (iRet >= 0 && got_frame) {
				
				int ch, plane_size;
				int planar = av_sample_fmt_is_planar(pAVContext->sample_fmt);
				int data_size = av_samples_get_buffer_size(&plane_size, pAVContext->channels,avFrame.nb_samples,pAVContext->sample_fmt, 1);
				if (pFrame->iDataSize[0] < data_size) {
					return SAV_RESULT_E_BUFSMALL;
				}
				memcpy(pFrame->ppData[0], avFrame.extended_data[0], plane_size);
				if (planar && pAVContext->channels > 1) {
					uint8_t *out = ((uint8_t *)pFrame->ppData[0]) + plane_size;
					for (ch = 1; ch < pAVContext->channels; ch++) {
						memcpy(out, avFrame.extended_data[ch], plane_size);
						out += plane_size;
					}
				}
				pFrame->iDataSize[0] = data_size;
				
				pContext->stAudioParam.iSampleRate = pAVContext->sample_rate;
    			pContext->stAudioParam.iChannelCount = pAVContext->channels;
    			pContext->stAudioParam.eSampleFormat = pAVContext->sample_fmt;
    			pFrame->iFlags |= SAV_PKT_FLAG_KEY;
			}else{
				pFrame->iDataSize[0] = 0;
				pFrame->iFlags &= ~SAV_PKT_FLAG_KEY;
			}
			if(pPacket->iPTS >= 0)
				pFrame->iPTS = pPacket->iPTS;
		}
	}
	return iRet;
}

LIBSAV_API SAV_Result SAVCodec_Control(SAVCodec_Context* pContext, SAVCodec_ControlCmd cmd, SAVCodec_ControlParam* pParam)
{
	SAV_Result Result = SAV_RESULT_E_FAILED;
	CHECK_ARG(pContext);
	if(!pContext->pCodec || !pContext->pCodec->pAVCodecCtx)
		return SAV_RESULT_E_NOTOPENED;
	switch(cmd){
		case SAVCODEC_CONTROLCMD_FLUSH:
			avcodec_flush_buffers(pContext->pCodec->pAVCodecCtx);
			Result = SAV_RESULT_S_OK;
		break;
		case SAVCODEC_CONTROLCMD_SETQUALITY:
			{
				SAVCodec_ControlParam_SetQuality* pP = (SAVCodec_ControlParam_SetQuality*)pParam;
				if(!pP || pP->cp.iSize != sizeof(*pP)){
					Result = SAV_RESULT_E_INVALIDARG;
					break;
				}
				if(pContext->eCodecID != SAVCODEC_ID_MJPEG || !pContext->bEncode){
					Result = SAV_RESULT_E_UNSUPPORTED;
					break;
				}else{
					AVCodecContext* pAVCtx = pContext->pCodec->pAVCodecCtx;
					if(pP->iQuality < 1)
						pP->iQuality = 1;
					else if(pP->iQuality > 100)
						pP->iQuality = 100;
					if(pAVCtx->qmax <= 0 || pAVCtx->qmin <= 0 || pAVCtx->qmax - pAVCtx->qmin <= 0){
						pAVCtx->qmax = 31;
						pAVCtx->qmin = 2;
					}
					pAVCtx->global_quality = FF_QP2LAMBDA * ((100 - pP->iQuality) * (pAVCtx->qmax - pAVCtx->qmin + 1)/100 + pAVCtx->qmin);
					Result = SAV_RESULT_S_OK;
				}
			}
			break;
		default:
			Result = SAV_RESULT_E_INVALIDARG;
		break;
	}
	return Result;
}

LIBSAV_API SAV_Result SAVCodec_Close(SAVCodec_Context* pContext)
{	
	CHECK_ARG(pContext);
	if(pContext->pCodec){
		if(pContext->pCodec->pAVCodecCtx){
			avcodec_close(pContext->pCodec->pAVCodecCtx);
			av_freep(&pContext->pCodec->pAVCodecCtx);
		}
		av_freep(&pContext->pCodec);
		if(pContext->pOptions)
			SAVDict_Free(&pContext->pOptions);
	}
	return SAV_RESULT_S_OK;
}
