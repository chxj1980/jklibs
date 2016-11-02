//
// Created by v on 16-10-26.
//

#include <fcntl.h>
#include <unistd.h>
#include "PSPacket.h"

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

        nalu->type = buf[nalhead_pos] & 0xff;
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

static int fi;
static int fo;

#define LOG(fmt, ...) \
   printf("%d: ", __LINE__); printf(fmt, ##__VA_ARGS__);  \
   printf("\n")

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
        printf("%02x ", data[i]);
    }
    printf("\n");
}

int process() {
    for (;;) {
        int ilen = 1024 * 800;
        char idata[ilen];
        int in = read(fi, idata, ilen);
        if (in < 0) {
            LOG("read data failed %d", in);
            break;
        }
        if (in == 0) {
            LOG("read over. ");
            break;
        }

        NaluUnit nu;
        memset(&nu, 0, sizeof(NaluUnit));
        int nalhead_pos = 0;
        int start_one = 0;
        char *start_ba;
        char *end_ba;
        for (;;) {
            nalhead_pos = ReadNaluFromBuf(&nu, idata, ilen, nalhead_pos);
            if (nalhead_pos < 0) break;

            if ((nu.type & 0xBA) == 0xBA && !start_one) {
                start_one = 1;
                start_ba = (char*)(nu.data-3);
                continue;
            }
            if (!start_one) {
                continue;
            }
            if (start_one) {
                if ((nu.type & 0xBA) == 0xBA) {
                    end_ba = (char*)(nu.data-3);
                } else {
                    continue;
                }
            }
            start_one = 0;
            int iilen = end_ba - start_ba;

            debug_data(start_ba, iilen);

            PSPacket *psp = new PSPacket();
            psp->PSWrite(start_ba, iilen);
            naked_tuple data = psp->naked_payload();
            bool has_data = std::tr1::get<0>(data);
            if (has_data) {
                unsigned char type = std::tr1::get<1>(data);
                if (type == 0xE0) {
                    // It is video
                    unsigned int dlen = std::tr1::get<5>(data);
                    char *ddata = std::tr1::get<4>(data);
                    int on = write(fo, ddata, dlen);
                    if (on <= 0) {
                        LOG("failed write, becarefull of it %d", on);
                        break;
                    }
                } else {
                    LOG("Invalid type %x", type);
                }
            } else {
                LOG("No data, just continue");
            }
        }

    }
}

int main() {

    file_ready("/opt/data/public/28181.dat", "/opt/data/public/ps-out.dat");
    process();
    file_close();

    return 0;
}