//
// Created by v on 16-10-28.
//

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include "PS2ES.h"

static int fi;
static int fo;

#define LOG(fmt, ...) \
   printf("%d: ", __LINE__); printf(fmt, ##__VA_ARGS__);  \
   printf("\n")

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

int file_ready(const char *inputfile, const char *outputfile)
{
    fi = open(inputfile, O_RDONLY);
    if (fi < 0) {
        LOG("error open file %d", fi);
        return -1;
    }

    fo = open(outputfile, O_RDWR|O_CREAT|O_TRUNC);
    if (fo <= 0) {
        LOG("error open file for write %d, %s", fo, strerror(errno));
        return -2;
    }
    return 0;
}

int file_close()
{
    if (fi) close(fi);
    if (fo) close(fo);
    return 0;
}


void debug_data(char *data, int len)
{
    int i;
    printf("\n");
    for (i = 0; i < len; i++) {
        printf("%02x ", (unsigned char)data[i]);
    }
    printf("\n");
}

int process(PS2ES_HANDLE handle) {

    for (;;) {
        int ilen = 1024 * 9000;
        char *idata = (char*)malloc(ilen);
        int in = read(fi, idata, ilen);
        if (in < 0) {
            LOG("read data failed %d", in);
            break;
        }
        LOG("read out data: %d", in);
        if (in == 0) {
            LOG("read over. ");
            break;
        }

        NaluUnit nu;
        memset(&nu, 0, sizeof(nu));
        int nalhead_pos = 0;

        for (;;) {
            nalhead_pos = ReadNaluFromBuf(&nu, idata, ilen, nalhead_pos);
            if (nalhead_pos < 0) break;

//            debug_data((char *) (nu.data - 3), (unsigned int) nu.size + 3);

//            int ret = PS2ES_PushIn(handle, (unsigned char*)idata, in);
            int ret = 0;
            if ((unsigned char)nu.data[0] == 0x61) {
                ret = PS2ES_PushIn(handle, nu.data-4, nu.size+4);
            } else {
                ret = PS2ES_PushIn(handle, nu.data-3, nu.size+3);
            }
            if (ret < 0) {
                LOG("Push in data failed %d", ret);
                continue;
//            break;
            }
            int out = 0;

            for (;;) {
                int olen = 1024 * 9000;
                unsigned char *odata = (unsigned char*)malloc(olen);
                int frmtype = 0;
                unsigned char streamtype[32] = {};
                unsigned int timestamp = 0;
                int ret = PS2ES_PopOut(handle, odata, &olen, &timestamp, &frmtype, streamtype);
                if (ret < 0) {
                    out = 1;
                    break;
                }
                LOG("push out data %d, %s", olen, streamtype);
                if (olen > 0) {
                    int on = write(fo, odata, olen);
                    if (on <= 0) {
                        LOG("write failed %d", on);
                    }
                }
            }
//            if (out) break;
        }
    }

    return 0;
}

int parse_rtp_to_ps(const char *filename, const char *outfile)
{
    // Every ps from 28181, is rtp stream, the first 12 bytes is the head,
    // we need remove the head, keep the real data.

    int fi = open(filename, O_RDONLY);
    if (fi < 0) {
        LOG("Open failed: %d", fi);
        return -1;
    }

    int foo = open(outfile, O_RDWR| O_CREAT|O_TRUNC);
    if (foo < 0) {
        LOG("Open out file failed %d", foo);
    }

    int rlen = 1024*18000;
    char *rdata = (char*)malloc(rlen);
    if (!rdata) {
        LOG("malloc failed");
        return -2;
    }

    int n = read(fi, rdata, rlen);
    if (n < 0) {
        LOG("Read failed %d", n);
        return -3;
    }
    if (fi) close(fi);

    char mark[4] = {};
    mark[0] = 0x17;
    mark[1] = 0x8A;
    mark[2] = 0x4D;
    mark[3] = 0xAD;

    // 80 E0 00 49 00 00 0E 10 17 8A 4D AD
    // E0/60, 60 for start, E0 for last
    // 00 49 : is sequence
    // For more, please see RTP

    char *p_start = rdata;
    char *p_startn = NULL;
    char *p_end = NULL;
    int do_save = 0;
    for (;;) {
        if (p_start >= rdata+rlen-4) {
            break;
        }
            int i,j;
            int find = 1;
            for (i = 0; i < 4; i++) {
                if (p_start[i] != mark[i]) {
                    find = 0;
                    break;
                }
            }
            if (find) {
                char *firstrtp = p_start-8;
//                debug_data(firstrtp, 12);
                if ((unsigned char)firstrtp[0] == 0x80 && (unsigned char)firstrtp[1] == 0x60) {
                    if (do_save == 0) {
                        p_startn = p_start+4;
                        do_save++;
                    } else {
                        do_save++;
                        p_end = p_start -8;
                    }
                } else if ((unsigned char)firstrtp[0] == 0x80 && (unsigned char)firstrtp[1] == 0xE0) {
                    if (do_save == 0) {
                        p_startn = p_start + 4;
                        do_save++;
                    } else {
                        do_save++;
                        p_end = p_start -8;
                    }
                }
                if (do_save > 1) {
                    do_save = 0;
                    int dlen = p_end - p_startn;
                    int n = write(foo, p_startn, dlen);
                    if (n < 0) {
                        LOG("write failed %d", n);
                    }
                }
            }
            p_start++; // go to next position
    }

    return 0;
}

int main()
{
#if 0
    int ret = parse_rtp_to_ps("/opt/data/public/28181.dat", "/opt/data/public/rtp-ps.dat");
    LOG("Result : %d", ret);
    return 0;
#endif
    PS2ES_HANDLE handle;

    int i = PS2ES_Create(&handle, 1024*9000);
    if (i != 0) {
        LOG("Create failed\n");
        return -1;
    }
    file_ready("/opt/data/public/rtp-ps.dat", "/opt/data/public/ps-out.dat");

    process(handle);

    file_close();

    PS2ES_Delete(handle);


    return 0;
}