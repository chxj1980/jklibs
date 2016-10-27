/*======================================================
	Copyright(c) BesoVideo Ltd. 2011
	changjin.liu@besovideo.com
========================================================*/

#if defined(_MSC_VER) & !defined(__cplusplus)
#ifndef inline
#    define inline __inline
#endif
#endif
#include "config.h"
#include "libavutil/avutil.h"
#include "libavutil/dict.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixdesc.h"
#include "libswscale/swscale.h"

#include "SAVCommon.h"
#include "SAVUtil.h"
#include "SAVPriv.h"

#define CHECK_ARG(p) if(!(p)) return SAV_RESULT_E_INVALIDARG; \
				if(sizeof(*(p)) != (p)->iSize) \
					return SAV_RESULT_E_INVALIDARG 
#define INVALID_PIXFMT(a)  ((enum PixelFormat)a <= PIX_FMT_NONE || (enum PixelFormat)a >= PIX_FMT_NB)
#define INVALID_VIDEOPARAM(p) ((p)->iWidth <= 0 || (p)->iHeight <= 0 || INVALID_PIXFMT(p->ePixelFmt))

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
#define AVUTIL(pDict) ((AVDictoionary*)(pDict))

LIBSAV_API SAV_TYPE_INT8* SAVDict_Get(SAVDictionary* pDict, const SAV_TYPE_INT8* pKey, SAVDict_Flag eFlags)
{
	AVDictionaryEntry* pAVE = av_dict_get((AVDictionary*)pDict, pKey, NULL, eFlags);
	if(pAVE)
		return pAVE->value;
	return NULL;
}

LIBSAV_API SAVDict_Entry* SAVDict_Enum(SAVDictionary* pDict, SAVDict_Entry* pEntryPrev)
{
    return (SAVDict_Entry*)av_dict_get((AVDictionary*)pDict, "", (AVDictionaryEntry*)pEntryPrev, AV_DICT_IGNORE_SUFFIX);
}

LIBSAV_API SAV_Result SAVDict_Set(SAVDictionary** ppDict, SAVDict_Entry* pEntry, SAVDict_Flag eFlags)
{
	int iRet =	av_dict_set((AVDictionary**)ppDict, pEntry->pKey, pEntry->pValue, eFlags);
	if(iRet >= 0)
		return SAV_RESULT_S_OK;
	else
		return SAV_RESULT_E_FAILED;
}

LIBSAV_API void SAVDict_Free(SAVDictionary **ppDict)
{
	if(ppDict)
		av_dict_free((AVDictionary**)ppDict);
}

LIBSAV_API SAV_TYPE_INT32 SAVImg_GetSize(SAV_VideoParam* pParam, SAV_Frame* pFrame)
{
	int i;
	int iLineSize[SAV_IMG_COMPONENT];
	uint8_t* ppData[SAV_IMG_COMPONENT];
	if(!pParam)
		return SAV_RESULT_E_INVALIDARG;
	if(INVALID_VIDEOPARAM(pParam))
		return SAV_RESULT_E_INVALIDARG;
	if(pFrame && pFrame->iDataSize[0] > 0){
		for(i = 0; i < SAV_IMG_COMPONENT; i++)
			iLineSize[i] = pFrame->iDataSize[i];
	}else{
		i = av_image_fill_linesizes(iLineSize, (enum PixelFormat)pParam->ePixelFmt, pParam->iWidth);
		if(i < 0){
			return ErrNum_FFMPEG2SAV(i);
		}
	}
	i = av_image_fill_pointers(ppData, (enum PixelFormat)pParam->ePixelFmt, pParam->iHeight, NULL, iLineSize);
	if(i < 0){
		return ErrNum_FFMPEG2SAV(i);
	}else{
		return i;
	}
}

LIBSAV_API SAV_TYPE_INT32 SAVImg_SetFramePointer(SAV_VideoParam* pParam, SAV_Frame* pFrame, void* pBuf)
{
	int i;
	int iLineSize[SAV_IMG_COMPONENT];
	if(!pParam || !pFrame)
		return SAV_RESULT_E_INVALIDARG;
	if(INVALID_VIDEOPARAM(pParam))
		return SAV_RESULT_E_INVALIDARG;
	if(pFrame->iDataSize[0] > 0){
		i = av_image_fill_linesizes(iLineSize, (enum PixelFormat)pParam->ePixelFmt, pParam->iWidth);
		if(i < 0){
			return ErrNum_FFMPEG2SAV(i);
		}
		for(i = 0; i < SAV_IMG_COMPONENT; i++){
			if(pFrame->iDataSize[i] < iLineSize[i]){
				return SAV_RESULT_E_INVALIDARG;
			}
		}
	}else{
		i = av_image_fill_linesizes(pFrame->iDataSize, (enum PixelFormat)pParam->ePixelFmt, pParam->iWidth);
		if(i < 0){
			return ErrNum_FFMPEG2SAV(i);
		}
	}
	i = av_image_fill_pointers(pFrame->ppData, (enum PixelFormat)pParam->ePixelFmt, pParam->iHeight, pBuf, pFrame->iDataSize);
	if(i < 0){
		return ErrNum_FFMPEG2SAV(i);
	}
	return SAV_RESULT_S_OK;
}

LIBSAV_API SAV_Result SAVImg_Copy(SAV_VideoParam* pParam, SAV_Frame* pFrameSrc, SAV_Frame* pFrameDst, void* pBufDst)
{
	int i;
	if(!pParam || !pFrameSrc || !pFrameDst)
		return SAV_RESULT_E_INVALIDARG;
	CHECK_ARG(pFrameSrc);
	
	if(INVALID_VIDEOPARAM(pParam))
		return SAV_RESULT_E_INVALIDARG;
		
	if(!pBufDst){
		SAV_TYPE_UINT8* ppData[SAV_IMG_COMPONENT];
		int iLineSize[SAV_IMG_COMPONENT];
		i = av_image_fill_linesizes(iLineSize, (enum PixelFormat)pParam->ePixelFmt, pParam->iWidth);
		if(i < 0){
			return ErrNum_FFMPEG2SAV(i);
		}
		for(i = 0; i < SAV_IMG_COMPONENT; i++){
			if(pFrameDst->iDataSize[i] < iLineSize[i]){
				return SAV_RESULT_E_INVALIDARG;
			}
			if(iLineSize[i] && !pFrameDst->ppData[i]){
				return SAV_RESULT_E_INVALIDARG;
			}
			ppData[i] = pFrameDst->ppData[i];
			iLineSize[i] = pFrameDst->iDataSize[i];
		}
		*pFrameDst = *pFrameSrc;
		for(i = 0; i < SAV_IMG_COMPONENT; i++){
			pFrameDst->ppData[i] = ppData[i];
			pFrameDst->iDataSize[i] = iLineSize[i];
		}
	}else{
		*pFrameDst = *pFrameSrc;
		i = av_image_fill_linesizes(pFrameDst->iDataSize, (enum PixelFormat)pParam->ePixelFmt, pParam->iWidth);
		if(i < 0){
			return ErrNum_FFMPEG2SAV(i);
		}
		i = av_image_fill_pointers(pFrameDst->ppData, (enum PixelFormat)pParam->ePixelFmt, pParam->iHeight, pBufDst, pFrameDst->iDataSize);
		if(i < 0){
			return ErrNum_FFMPEG2SAV(i);
		}
		//for(i = 0; i < SAV_IMG_COMPONENT; i++)
			//pFrameDst->iDataSize[i] = pFrameSrc->iDataSize[i];
	}
	av_image_copy(pFrameDst->ppData, pFrameDst->iDataSize, pFrameSrc->ppData, pFrameSrc->iDataSize,
                   (enum PixelFormat)pParam->ePixelFmt, pParam->iWidth, pParam->iHeight);
    return SAV_RESULT_S_OK;
}

LIBSAV_API SAV_Result SAVImg_Save(SAV_VideoParam* pParam, SAV_Frame* pFrame, fnSAVImg_Save fCallback, void* pUserData)
{
	int iStride[3], iHeight[3];
	SAV_TYPE_UINT8* pData;
	const AVPixFmtDescriptor* pPFD;
	int i;
	if(!pParam || ! pFrame || !fCallback){
		return SAV_RESULT_E_INVALIDARG;
	}
	if(INVALID_VIDEOPARAM(pParam))
		return SAV_RESULT_E_INVALIDARG;
	CHECK_ARG(pFrame);
#if 0
	pPFD = SAVPriv_GetPixFmtDescriptor((enum PixelFormat)pParam->ePixelFmt);
#endif
	if(!pPFD)
		return SAV_RESULT_E_INVALIDARG;
	if(pPFD->nb_components != 1 && pPFD->nb_components != 3)
		return SAV_RESULT_E_UNSUPPORTED;
	iStride[0] = pParam->iWidth * (pPFD->comp[0].step_minus1 + 1);
	iHeight[0] = pParam->iHeight;
	if(iStride[0] > pFrame->iDataSize[0])
		return SAV_RESULT_E_INVALIDARG;
	if(pPFD->nb_components == 3){
		if(!pPFD->comp[0].plane && !pPFD->comp[1].plane && !pPFD->comp[2].plane){ //all components use plane 0 is OK
		}else{
			if( pPFD->comp[0].plane != 0 || pPFD->comp[1].plane != 1 || pPFD->comp[2].plane != 2)
				return SAV_RESULT_E_UNSUPPORTED;
			else{
				iStride[1] = iStride[2] = pParam->iWidth >> pPFD->log2_chroma_w;
				iHeight[1] = iHeight[2] = pParam->iHeight >> pPFD->log2_chroma_h;
				if(iStride[1] > pFrame->iDataSize[1] || iStride[2] > pFrame->iDataSize[2])
					return SAV_RESULT_E_INVALIDARG;
			}
		}
	}
	
	pData = pFrame->ppData[0];
	for(i = 0; i < iHeight[0]; i++){
		if(SAV_Result_FAILED(fCallback(pUserData, pData, iStride[0]))){
			return SAV_RESULT_E_ABORTED;
		}
		pData += pFrame->iDataSize[0];
	}
	
	if(pPFD->nb_components == 3){
		if(!pPFD->comp[0].plane && !pPFD->comp[1].plane && !pPFD->comp[2].plane)
			return SAV_RESULT_S_OK;
			
		pData = pFrame->ppData[1];
		for(i = 0; i < iHeight[1]; i++){
			if(SAV_Result_FAILED(fCallback(pUserData, pData, iStride[1]))){
				return SAV_RESULT_E_ABORTED;
			}
			pData += pFrame->iDataSize[1];
		}
		
		pData = pFrame->ppData[2];
		for(i = 0; i < iHeight[2]; i++){
			if(SAV_Result_FAILED(fCallback(pUserData, pData, iStride[2]))){
				return SAV_RESULT_E_ABORTED;
			}
			pData += pFrame->iDataSize[2];
		}		
	}
	return SAV_RESULT_S_OK;
}

LIBSAV_API SAV_Result SAVImg_Convert_Open(SAVImg_Convert_Context* pContext)
{
	CHECK_ARG(pContext);
	pContext->pObj = NULL;
	if(INVALID_PIXFMT(pContext->ePixelFmtSrc) || INVALID_PIXFMT(pContext->ePixelFmtDst)){
		return SAV_RESULT_E_INVALIDARG;
	}
	if(pContext->iWidth <= 0 || pContext->iHeight <= 0)
		return SAV_RESULT_E_INVALIDARG;
	pContext->pObj = sws_getCachedContext(NULL, pContext->iWidth, pContext->iHeight, (enum PixelFormat)pContext->ePixelFmtSrc,
			pContext->iWidth, pContext->iHeight, (enum PixelFormat)pContext->ePixelFmtDst,SWS_BICUBLIN, NULL, NULL, NULL);
	if(!pContext->pObj)
		return SAV_RESULT_E_FAILED;
	return SAV_RESULT_S_OK;
}

LIBSAV_API SAV_Result SAVImg_Convert_Process(SAVImg_Convert_Context* pContext, SAV_Frame* pFrameSrc, SAV_Frame* pFrameDst)
{
	CHECK_ARG(pContext);
	if(!pContext->pObj)
		return SAV_RESULT_E_INVALIDARG;
	if(sws_scale((struct SwsContext*)pContext->pObj, pFrameSrc->ppData, pFrameSrc->iDataSize, 0, pContext->iHeight,
		pFrameDst->ppData, pFrameDst->iDataSize) <= 0)
		return SAV_RESULT_E_FAILED;
	return SAV_RESULT_S_OK;
}

LIBSAV_API SAV_Result SAVImg_Convert_Close(SAVImg_Convert_Context* pContext)
{
	CHECK_ARG(pContext);
	if(pContext->pObj){
		sws_freeContext((struct SwsContext*)pContext->pObj);
		pContext->pObj = NULL;
	}
	return SAV_RESULT_S_OK;
}
