/*======================================================
	Copyright(c) BesoVideo Ltd. 2011
	changjin.liu@besovideo.com
========================================================*/
/************************ for internal use by SAVCodec/SAVContainer *********************/
#include "libavutil/pixdesc.h"
#include "libavutil/avutil.h"
#include "libavcodec/get_bits.h"
#include "libavcodec/golomb.h"
#include "libavutil/internal.h"  // This file is used for mpeg4audio.h
#include "libavcodec/mpeg4audio.h"
#include "SAVCommon.h"
#include "SAVPriv.h"

#define BREAK_WITH_RESULT(r) Result = r; \
							 break;
							 
LIBSAV_API SAV_Result SAVPriv_ParseH264Head(SAV_TYPE_UINT8* pData, SAV_TYPE_INT32 iDataLen,
							SAV_TYPE_INT32* piWidth, SAV_TYPE_INT32* piHeight)
{
	SAV_TYPE_UINT8* p = pData;
	SAV_TYPE_INT32 iLen = iDataLen;
	SAV_TYPE_UINT8* pSPS = NULL;
	SAV_Result Result = SAV_RESULT_S_OK;
	int iSPSLen = 0;
	int i = 0;
	
	//for bit parse
	GetBitContext bits;
	int poc_type;
	int profile_idc;
	
	do{
		if(iLen < 5){
			BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDARG);
		}
		if(p[0] == 0x01){//is avc
			if(iLen < 9){
				BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDDATA);
			}
			//AVC example: 01 42 80 1e ff e1 00 08 67 42 80 1e da 05 82 51 01 00 04 68 ce 3c 80
			iSPSLen = (p[6] << 8) | p[7];
			if(iSPSLen + 6 > iLen){
				BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDDATA);
			}
			//parse only the first SPS		
			pSPS = p + 8;
		}else if((p[0] | p[1]) == 0 && (p[2] == 1 || (p[2] | p[3]) == 1)){//check annex b
			int iEnd = iLen - 3 - 3;		
			pSPS = p + 3;
			if(p[2] == 0){
				iEnd--;
				pSPS++;
			}
			p = pSPS;
			iSPSLen = iLen - 3;
			i = 0;
			while(i < iEnd){
				if((p[i] | p[i+1])==0 && p[i+2] == 1){
					iSPSLen = i;
					if(p[i-1] == 0)
						iSPSLen--;
				}
				i++;
			}
		}else{
			BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDDATA);
		}
		if((pSPS[0] & 0x1F) != 7){
			BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDDATA);
		}
		
		//do parse
		init_get_bits(&bits, pSPS + 1, iSPSLen * 8);
		profile_idc = get_bits(&bits, 8);//profile_idc
		skip_bits(&bits, 4);//4 constraint set_flag
		skip_bits(&bits, 4);//reserved
		skip_bits(&bits, 8);//level_idc
		if(get_ue_golomb_31(&bits) > 32){//sps id
			BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDDATA);
		}
		if(profile_idc >= 100){//high profile
			int chroma_format_idc = get_ue_golomb_31(&bits);
			if(chroma_format_idc == 3){//chroma_format_idc
				skip_bits1(&bits);//residual_color_transform_flag
			}
			get_ue_golomb(&bits);//bit_depth_luma
			get_ue_golomb(&bits);//bit_depth_chroma
			skip_bits1(&bits);//transform_bypass
			//decode_scaling_matrices
			if(get_bits1(&bits)){//scaling_matrix_present
				for(i = 0; i < ((chroma_format_idc != 3) ? 8 : 12); ++i){				
					if(get_bits1(&bits)){//seq_scaling_list_present_flag
						int sizeOfScalingList = i < 6 ? 16 : 64;
						int lastScale = 8;
						int nextScale = 8;
						int j;
						for (j = 0; j < sizeOfScalingList; ++j) {
							if (nextScale != 0) {							
								nextScale = (lastScale + get_se_golomb(&bits)) & 0x0FF;
							}
							lastScale = (nextScale == 0) ? lastScale : nextScale;
						}
					}
				}
			}
		}
		i = get_ue_golomb(&bits);//log2_max_frame_num
		poc_type = get_ue_golomb_31(&bits);//poc_type
		if(poc_type == 0){ //FIXME #define
			get_ue_golomb(&bits);//log2_max_poc_lsb
		}else if(poc_type == 1){
			int poc_cycle_length;
			skip_bits1(&bits);//delta_pic_order_always_zero_flag
			get_se_golomb(&bits);//offset_for_non_ref_pic
			get_se_golomb(&bits);//offset_for_top_to_bottom_field
			poc_cycle_length = get_ue_golomb(&bits);//poc_cycle_length
			for(i=0; i<poc_cycle_length; i++)
				get_se_golomb(&bits);
		}else if(poc_type != 2){
			BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDDATA);
		} 
		if(get_ue_golomb_31(&bits) > 16U){ //ref_frame_count
			BREAK_WITH_RESULT(SAV_RESULT_E_INVALIDDATA);
		}
		skip_bits1(&bits);//gaps_in_frame_num_allowed_flag
		
		*piWidth = (get_ue_golomb(&bits) + 1) * 16;
		*piHeight = (get_ue_golomb(&bits) + 1) * 16;
		Result = SAV_RESULT_S_OK;
	}while(0);
	return Result;
}

LIBSAV_API SAV_Result SAVPriv_ParseAACHead(SAV_TYPE_UINT8* pData, SAV_TYPE_INT32 iDataLen,
							SAVPriv_AACExtra* pAACExtra)
{
	SAV_TYPE_BOOL bADTS = SAV_BOOL_TRUE;
	SAV_TYPE_UINT8* p = pData;
	if(iDataLen < 4)
		bADTS = SAV_BOOL_FALSE;
	else if(!(p[0] == 0xFF && (p[1] & 0xF0) == 0xF0)){
		bADTS = SAV_BOOL_FALSE;
	}
	if(bADTS){//adts
		int iProfile;
		int iSampleFreqIndex = (p[2]&0x3C)>>2; // 4 bits
		int iChannelConfiguration = ((p[2]&0x01)<<2)|((p[3]&0xC0)>>6); // 3 bits
		pAACExtra->iSampleRate = avpriv_mpeg4audio_sample_rates[iSampleFreqIndex];
		pAACExtra->iChannelCount = ff_mpeg4audio_channels[iChannelConfiguration];
		//convert the pExtra
		pAACExtra->iExtraDataSize = 2;
		pAACExtra->pExtraData = (uint8_t*)av_mallocz(SAV_INPUT_BUFFER_PADDING_SIZE + pAACExtra->iExtraDataSize);
		if(!pAACExtra->pExtraData){
			pAACExtra->iExtraDataSize = 0;
			return SAV_RESULT_E_ALLOCMEMFAILED;
		}
		// Get and check the 'profile':
		iProfile = ((p[2]&0xC0)>>6) + 1; //2 bit profile
		pAACExtra->pExtraData[0] = (iProfile << 3) | (iSampleFreqIndex >> 1);
		pAACExtra->pExtraData[1] = (iSampleFreqIndex << 7) | (iChannelConfiguration << 3);
	}else{
		MPEG4AudioConfig mp4ac;
		memset(&mp4ac, 0, sizeof(mp4ac));
		avpriv_mpeg4audio_get_config(&mp4ac, p, iDataLen * 8, 0);
		pAACExtra->iSampleRate = mp4ac.sample_rate;
		pAACExtra->iChannelCount = mp4ac.channels;
		pAACExtra->pExtraData = NULL;
		pAACExtra->iExtraDataSize = 0;
	}
	if(pAACExtra->iSampleRate >= 7350 && pAACExtra->iChannelCount >=1 && pAACExtra->iChannelCount <= 8){//valid
		return SAV_RESULT_S_OK;
	}else{
		if(pAACExtra->pExtraData){
			av_freep(&pAACExtra->pExtraData);
			pAACExtra->iExtraDataSize = 0;
		}
		return SAV_RESULT_E_INVALIDDATA;
	}
}														
#define INVALID_PIXFMT(a)  ((enum PixelFormat)a <= PIX_FMT_NONE || (enum PixelFormat)a >= PIX_FMT_NB)
LIBSAV_API const AVPixFmtDescriptor* SAVPriv_GetPixFmtDescriptor(enum PixelFormat pix_fmt)
{
	if(pix_fmt <= PIX_FMT_NONE || pix_fmt >= PIX_FMT_NB)
		return NULL;
	return &av_pix_fmt_descriptors[pix_fmt];
}
