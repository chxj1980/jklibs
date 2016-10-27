//
// Created by v on 16-10-27.
//

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include "libsav/SAVCodec.h"

SAVCodec_Context ctx;
int has_init = 0;
int close_all = 0;

/**
* _NaluUnit
* ÄÚ²¿½á¹¹Ìå¡£¸Ã½á¹¹ÌåÖ÷ÒªÓÃÓÚ´æ´¢ºÍ´«µÝNalµ¥ÔªµÄÀàÐÍ¡¢´óÐ¡ºÍÊý¾Ý
*/
typedef struct _NaluUnit
{
    int type;
    int size;
    unsigned char *data;
} NaluUnit;

int ReadNaluFromBuf( NaluUnit *nalu, char* buf, int len, int nalhead_pos)
{
    nalu->size = 0;
    int		naltail_pos = nalhead_pos;
    while ( nalhead_pos < len )
    {
        /* search for nal header */
        if ( buf[nalhead_pos++] == 0x00 &&
             buf[nalhead_pos++] == 0x00 )
        {
            if ( buf[nalhead_pos++] == 0x01 )
                goto gotnal_head;
            else{
                /* cuz we have done an i++ before,so we need to roll back now */
                nalhead_pos--;
                if ( buf[nalhead_pos++] == 0x00 &&
                     buf[nalhead_pos++] == 0x01 )
                    goto gotnal_head;
                else
                    continue;
            }
        }else
            continue;

        /* search for nal tail which is also the head of next nal */
        gotnal_head:
        /* normal case:the whole nal is in this m_pFileBuf */
        naltail_pos = nalhead_pos;
        while ( naltail_pos < len )
        {
            if ( buf[naltail_pos++] == 0x00 &&
                 buf[naltail_pos++] == 0x00 )
            {
                if ( buf[naltail_pos++] == 0x01 )
                {
                    nalu->size = (naltail_pos - 3) - nalhead_pos;
                    break;
                }else {
                    naltail_pos--;
                    if ( buf[naltail_pos++] == 0x00 &&
                         buf[naltail_pos++] == 0x01 )
                    {
                        nalu->size = (naltail_pos - 4) - nalhead_pos;
                        break;
                    }
                }
            }
        }

        nalu->type = buf[nalhead_pos] & 0x1f;
        nalu->data	= (unsigned char*)buf + nalhead_pos;
        if (nalu->size == 0) {
            nalu->size = naltail_pos-nalhead_pos;
            return naltail_pos;
        }
        nalhead_pos	= naltail_pos;
        return nalhead_pos - 4;
    }
    return -1;
}

int debug_data(char *data, size_t len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
    return 0;
}

int sav_decoder(const char *input, size_t ilen, char **output, int *olen)
{
    if (!has_init) {
        memset(&ctx, 0, sizeof(SAVCodec_Context));
        ctx.iSize = sizeof(SAVCodec_Context);
        ctx.eCodecID = SAVCODEC_ID_H264;
        ctx.bEncode = 0;
        ctx.eMediaType = SAV_MEDIATYPE_VIDEO;
        ctx.TimeBase.num = 1;
        ctx.TimeBase.den = 25;
        ctx.stVideoParam.iWidth = 384;
        ctx.stVideoParam.iHeight = 288;
        ctx.stVideoParam.ePixelFmt = SAV_PIX_FMT_YUV420P;
        int ret = SAVCodec_Open(&ctx);
        if (ret != SAV_RESULT_S_OK) {
            printf("Error open context: %x\n", ret);
            return -1;
        }
        has_init = 1;
    }

    int ret = 0;
    if (ilen > 0) {
        SAV_Frame frame;
        memset(&frame, 0, sizeof(SAV_Frame));
        frame.iSize = sizeof(SAV_Frame);

        SAV_Packet pkt;
        memset(&pkt, 0, sizeof(SAV_Packet));
        pkt.iSize = sizeof(SAV_Packet);
        static int ptx = 200;
        pkt.iPTS = ptx;
        ptx += 200;
        pkt.pData = (SAV_TYPE_UINT8 *)input;
        pkt.iDataSize = ilen;

        ret = SAVCodec_Process(&ctx, &frame, &pkt);
        if (ret != SAV_RESULT_S_OK) {
//            printf("Error process : %x\n", ret);
//            return -2;
        }

        if (ret > 0 && frame.ePictType != SAV_PICTURE_TYPE_NONE) {
            int size = SAVImg_GetSize(
                    &ctx.stVideoParam, NULL);
            if (olen) *olen = size;
            *output = (char*)calloc(1, size);
            SAV_Frame dstFrame;
            memset(&dstFrame, 0, sizeof(dstFrame));

            SAVImg_Copy( &ctx.stVideoParam, &frame, &dstFrame, *output);
//                    dstFrame.iDataSize[0] = size;
//                    dstFrame.ppData[0] = *output;
        }

    }
    return ret;
}

int sav_process(const char *inputfile, const char *outputfile)
{
    FILE *f = fopen(inputfile, "r");
    if (! f) {
        return -1;
    }

    FILE *fo = fopen(outputfile, "w+");
    if (! fo) {
        return -2;
    }

    while(!feof(f)) {
        size_t len = 1024*800;
        char data[len];
        size_t n = fread(data, 1, len, f);
        if (n <= 0) {
            printf("Read data failed %d\n", n);
        }

        NaluUnit nu;
        memset(&nu, 0, sizeof(NaluUnit));
        int nalhead_pos = 0;
        int err = 0;
        for (;;) {
            nalhead_pos = ReadNaluFromBuf(&nu, data, n, nalhead_pos);
            if (nalhead_pos < 0) break;

            printf("length: %d, nalhead_post : %d\n", nu.size, nalhead_pos);


            char *odata = NULL;
            int olen = 0;
//            debug_data(nu.data-4, nu.size+4);
            int ret = sav_decoder(nu.data-4, nu.size+4, &odata, &olen);
            if (ret < 0) {
                printf("Something decoder wrong %x...\n", ret);
                if (odata) free(odata);
                continue;
            }

            printf("INFO: get output data len %d, %d\n", olen, ret);
            if (olen > 0 && odata != NULL) {
                ret = fwrite(odata, 1, olen, fo);
                if (ret <= 0) {
                    printf("Write failed %d\n", ret);
                }
            }

            if (odata) free(odata);

        }
        if (err == 1) {
            break;
        }
    }

    if (f) fclose(f);
    if (fo) fclose(fo);

    return 0;
}


int main(int argc, char **args)
{

    int ret = sav_process("/opt/data/public/2.mp4", "/opt/data/public/1.yuv");
    printf("The result %d\n", ret);

    if (has_init) {
        SAVCodec_Close(&ctx);
        has_init = 0;
    }

    return 0;
}