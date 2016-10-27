/*======================================================
	Copyright(c) BesoVideo Ltd. 2011
	changjin.liu@besovideo.com
========================================================*/
#ifndef __SAVPRIV_H__
#define __SAVPRIV_H__

/************************ for internal use by SAVCodec/SAVContainer *********************/
#include <libsav/SAVTypes.h>
#include <libsav/SAVCommon.h>

/**
* Get width/height from h.264 stream header. The stream header can be annex B or MP4 format
*/
LIBSAV_API SAV_Result SAVPriv_ParseH264Head(SAV_TYPE_UINT8* pData, SAV_TYPE_INT32 iDataLen,
							SAV_TYPE_INT32* piWidth, SAV_TYPE_INT32* piHeight);


/*
* get AAC samplerate/channelcount from AAC header. AAC header can be ADTS or mpeg4
*/
typedef struct _SAVPriv_AACExtra{
	SAV_TYPE_INT32 iSampleRate;
	SAV_TYPE_INT32 iChannelCount;
	SAV_TYPE_UINT8* pExtraData;//new ExtraData
	SAV_TYPE_INT32  iExtraDataSize;
}SAVPriv_AACExtra;

LIBSAV_API SAV_Result SAVPriv_ParseAACHead(SAV_TYPE_UINT8* pData, SAV_TYPE_INT32 iDataLen,
							SAVPriv_AACExtra* pAACExtra);

LIBSAV_API const AVPixFmtDescriptor* SAVPriv_GetPixFmtDescriptor(enum PixelFormat pix_fmt);

#endif
