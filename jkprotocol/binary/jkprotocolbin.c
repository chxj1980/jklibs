//
// Created by v on 16-1-4.
// Author: jmdvirus@roamter.com
//

//
// +-----------+-----------+----------+--------+-----------+-----------+
// | version   | direction | reseved  |   cmd  |  subcmd   |  code     |  (4bytes)
// | 4 bits    |   1bits   |  3 bits  |  4 bits|   12 bits |   8bits   |  (32 bits, 4 byte)
// +-----------+-----------+----------+--------+-----------+-----------+
// | id                                            |   reserve         |  (8bytes)
// | 48bits                                        |   16 bits         |  (96 bits, 12 byte)
// +-----------+-----------+----------+--------+-----------+-----------+
// |  transaction                                                      |  (8bytes)
// |   64bits                                                          |
// |                                                                   |  (160 bits, 20 byte)
// +-----------+-----------+----------+--------+-----------+-----------+
// |  sequence                                     |        length     |  (4bytes)
// |   16bits                                      |        16bits     |  (192 bits, 24 byte)
// +-----------+-----------+----------+--------+-----------+-----------+
// |  Sign                                                             |  (16bytes)
// |  16 bytes                                                         |
// |                                                                   |
// |                                                                   |  (~ bits, 40 byte)
// +-----------+-----------+----------+--------+-----------+-----------+
// |   Data                                                            |
// +-----------+-----------+----------+--------+-----------+-----------+
//
// direction: 0 send 1 recv
// code: first bits 1 success, 0 fail, others: fail code max for 2^7-1
// Sender will ignore code.
//
// We are not success with the binary define, we will use it later,
// but now we just use string for quick developer to use.
// string format like below.
// header: version-id-direction-cmd-subcmd-transaction-sequence-sign-length-ret-retcode
// direction : 0 for send command, 1 for response
// header must 120 bytes, pad 0 if not enough. ret and retcode only effective when direction is 1
//

#include <stdio.h>
#include <rt_print.h>
#include "jkprotocolbin.h"
#include "bvpu_utils.h"
#include "ngx_md5.h"

#define JK_USE_TEXT

const char *jkpbin_key = "ab3w-be82f231-aa-cd1b";

static const int valid_header_len = 120; // 120 bytes

typedef struct tagJKProtocolBinHeader {
    int      version;
    int      direction;
    int      cmd;
    int      subcmd;
    char     id[16];
//    long     id;
    long     transaction;
    int      sequence;
    int      length;
    char     sign[34];
    int      headerLen;

    int      ret; // 0 for fail, 1 for success
    int      retCode; // fail/success code
} Header;

typedef struct tagJKProtocolBinBody {
    int       length;
    char     *data;
} Body;

struct tagJKProtocolBin {
    Header     header;
    Body       body;
    int        curseq;
};

int jk_protocol_bin_init(JKProtocolBin *pbin)
{
    JKProtocolBin inBin = (JKProtocolBin)bvpu_mem_calloc(1, sizeof(struct tagJKProtocolBin));
    if (!inBin) {
        return -1;
    }
    inBin->header.version = 1;
    inBin->header.headerLen = valid_header_len; // 120 bytes
    inBin->curseq = 1;

    if (pbin) *pbin = inBin;
    return 0;
}

int jk_protocol_bin_deinit(JKProtocolBin *pbin)
{
    if (pbin) {
        free(*pbin);
        *pbin = NULL;
    }
    return 0;
}

int jk_protocol_bin_get_curseq(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->curseq;
}

int jk_protocol_bin_set_args(JKProtocolBin  pbin, int cmd, int subcmd, char* id)
{
    if (!pbin) return -1;
    pbin->header.cmd = cmd;
    pbin->header.subcmd = subcmd;
    snprintf(pbin->header.id, sizeof(pbin->header.id), "%s", id);

    return 0;
}

// We don't copy data, so please keep data exist until you don't need it with the interface.
int jk_protocol_bin_set_data(JKProtocolBin pbin, int len, char *data)
{
    if (!pbin) return -1;
    pbin->header.length = len;

    pbin->body.length = len;
    if (data) pbin->body.data = data;
    return 0;
}

static unsigned char *jk_protocol_bin_generate_sign(JKProtocolBin pbin)
{
    if (!pbin) return NULL;

    unsigned char sign[16] = {0};
    static unsigned char saveSign[34] = {0};

    char lenstr[1024] = {0};
    char version[4] = {0};
    sprintf(version, "%02d", pbin->header.version);
    sprintf(lenstr, "%s-%s-%d-%d-%ld-%d-%s-%d", version, pbin->header.id, pbin->header.cmd,
    pbin->header.subcmd, pbin->header.transaction, pbin->header.sequence, jkpbin_key,
    pbin->header.length);
    rtdebug("string is : %s", lenstr);
    kfmd5_ngx(lenstr, strlen(lenstr), (unsigned char*)sign);
    kfmd5_ngx_tostring(sign, saveSign);
    return (unsigned char*)saveSign;
}

// return 1: valid, others: invalid
static int jk_protocol_bin_check_valid(JKProtocolBin pbin)
{
    if (!pbin) return -1;

    if (pbin->header.version > (1<< 4 -1)) {
        rterror("header version is invalid");
        return -7;
    }

    if (pbin->header.cmd > (1<<4-1)) {
        rterror("header cmd is too long")
        return -2;
    }
    if (pbin->header.subcmd > (1<<12-1)) {
        rterror("header subcommand is too long")
        return -3;
    }
    if (pbin->header.direction != 0 && pbin->header.direction != 1) {
        rterror("direction is invalid.")
        return -4;
    }
    if (pbin->header.length > (1<<16-1)) {
        rterror("length is too long")
        return -5;
    }
    if (pbin->header.sequence > (1<<16-1)) {
        rterror("sequence is too long")
        return -6;
    }
    if (pbin->header.headerLen != valid_header_len) {
        rterror("header len is not valid.")
        return -7;
    }
    if (pbin->header.ret != 1 && pbin->header.ret != 0) {
        rterror("header ret is not true or failse")
        return -8;
    }
    if (pbin->header.retCode < 0 || (pbin->header.retCode > (1<<7 - 1))) {
        rterror("header ret code is not valid.")
        return -9;
    }
    return 1;
}

static int jk_protocol_bin_save_text(JKProtocolBin pbin, char *save, int *len)
{
    if (!pbin || !save) return -1;

    char *p = save;

    memset(save, 0, pbin->header.headerLen+pbin->header.length);

    // version-id-direction-cmd-subcmd-transaction-sequence-sign-length-ret-retcode
    sprintf(save, "%02d-%s-%d-%d-%d-%ld-%d-%s-%d-%d-%d",
        pbin->header.version, pbin->header.id, pbin->header.direction,
        pbin->header.cmd, pbin->header.subcmd, pbin->header.transaction, pbin->header.sequence,
        pbin->header.sign, pbin->header.length, pbin->header.ret, pbin->header.retCode);

    memcpy(save+pbin->header.headerLen, pbin->body.data, pbin->header.length);
    if (len) *len = pbin->header.headerLen+pbin->header.length;

    return 0;
}

static int jk_protocol_bin_memcpy_data(JKProtocolBin pbin, char *save, int *len)
{
    if (!pbin || !save) return -1;

    char *p = save;

    // version 4 bits
    int i;
    for (i = 0; i < 4; i++) {
        int j = pbin->header.version & (1<<i);
        if (j != 0) {
            p[0] = p[0] | (1 << (i+4));
        }
    }

    // direction 1 bit
    if (pbin->header.direction == 1) {
        p[0] |= 1<<3;
    }

    // reserved 3 bits

    // command 4 bits
    for (i = 0; i < 4; i++) {
        int j = pbin->header.cmd & (1<<i);
        if (j != 0) {
            p[1] |= (1 << (i+4));
        }
    }

    // subcommand 12 bits
    for (i = 0; i < 4; i++) {
        int j = pbin->header.subcmd & (1 << (i+8));
        if (j != 0) {
            p[1] |= (1 << i);
        }
    }
    // subcommand later 8bits
    for (i = 0; i < 8; i++) {
        int j = pbin->header.subcmd & (1 << i);
        if (j != 0) {
            p[2] |= (1<<i);
        }
    }

    // code 8 bits
    // send command it is 0 all.
    if (pbin->header.ret == 1) {
        p[3] |= (1<< 7);
    }
    for (i = 0; i < 7; i++) {
        int j = pbin->header.retCode & (1<< i);
        if (j != 0) {
            p[3] |= (1<< i);
        }
    }

    int index = 4;
    // id 6 bytes
    memcpy(p+index, &pbin->header.id, 6);

    // reserved 2 bytes
    index += 8;
    // transaction 8 bytes
    memcpy(p+index, &pbin->header.transaction, 8);

    index += 8;
    // sequence 2 bytes
    memcpy(p+index, &pbin->header.sequence, 2);

    index += 2;
    // length 2 bytes
    // If we need work as bits, memcpy will copy failed.
    for (i = 0; i < 8; i++) {
        int j = pbin->header.length & (1<<i);
        if (j != 0) {
            p[index+1] |= (1<<i);
        }
    }
    for (i = 0; i < 8; i++) {
        int j = pbin->header.length & (1<<(i+8));
        if (j != 0) {
            p[index] |= (1<<i);
        }
    }

    index += 2;
    // sign 16 bytes
    memcpy(p+index, pbin->header.sign, 16);

    index += 16;
    // data
    memcpy(p+index, pbin->body.data, pbin->header.length);

    if (len) *len = pbin->header.headerLen+pbin->header.length;

    return pbin->header.headerLen + pbin->header.length;
}

// Finally, you must call this function to genearate data and will save to @save.
// We don't malloc @save, you must be sure it is enough space to save data.
// About > 40 bytes + len data
int jk_protocol_bin_generate_data(JKProtocolBin pbin, char *save, int *len)
{
    if (!pbin || !save) return -1;

    pbin->header.transaction = time(NULL);
    //pbin->header.direction = 0;
    if (pbin->curseq > (1<< 16-1)) {
        pbin->curseq = 0;
    }
    pbin->header.sequence = pbin->curseq++;
    char *sign = jk_protocol_bin_generate_sign(pbin);
    if (!sign) {
        rterror("failed generate sign .. ");
        return -4;
    }
    snprintf(pbin->header.sign, sizeof(pbin->header.sign), "%s", sign);

    int ret = jk_protocol_bin_check_valid(pbin);
    if (ret < 0) {
        return -2;
    }

#ifdef JK_USE_TEXT
    ret = jk_protocol_bin_save_text(pbin, save, len);
    if (ret < 0) {
        return -3;
    }
#else
    ret = jk_protocol_bin_memcpy_data(pbin, save, len);
    if (ret < 0) {
        return -3;
    }
#endif

    return 0;
}

// Set response code
// ret : 0 success, others fail
// retcode: success/fail code max 1<<7-1
int jk_protocol_bin_set_response(JKProtocolBin pbin, int ret, int retcode)
{
    if (!pbin || retcode > (1<<7 -1)) return -1;
    if (retcode < 0) return -2;
    ret = (ret == 0) ? 0 : 1;

    pbin->header.ret = ret;
    pbin->header.retCode = retcode;
    pbin->header.direction = 1; // response.

    return 0;
}

static int jk_protocol_bin_parse_data_text(JKProtocolBin pbin, char *data, int len)
{
    if (!pbin || !data) return -1;

    rtdebug("the data are :[%s]", data);
    if (len < pbin->header.headerLen) {
        rterror("data is not enough long. %d", strlen(data));
        return -3;
    }
    char *p = data;

    //version-id-direction-cmd-subcmd-transaction-sequence-sign-length-ret-retcode
    char *pn = data;
    char tmp[16] = {0};
    pn = strstr(pn, "-");
    if (pn) {
        memcpy(tmp, p, (pn-p));
        pbin->header.version = atoi(tmp);
    }
    p = pn+1;
    pn++;

    // id
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memset(pbin->header.id, 0, sizeof(pbin->header.id));
        int len = pn -p;
        memcpy(tmp, p, len);
        memcpy(pbin->header.id, tmp, sizeof(pbin->header.id));
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // direction
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.direction = atoi(tmp);
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // cmd
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.cmd = atoi(tmp);
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // subcmd
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.subcmd = atoi(tmp);
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // transaction
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.transaction = atol(tmp);
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // sequence
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.sequence = atoi(tmp);
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // sign
    pn = strstr(pn, "-");
    if (pn) {
        memcpy(pbin->header.sign, p, (pn-p));
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // length
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.length = atoi(tmp);
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // ret
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.ret = atoi(tmp);
    }
    p = pn+1;
    pn++;
    if (!pn || !p) return -2;

    // retcode
    pn = strstr(pn, "-");
    if (pn) {
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, p, (pn-p));
        pbin->header.retCode = atoi(tmp);
    }

    if (pbin->header.length > (1<<16-1)) {
        rterror("length is too long");
        return -3;
    }
    pbin->body.length = pbin->header.length;
    pbin->body.data = (char*)bvpu_mem_calloc(1, pbin->body.length+1);
    if (!pbin->body.data) {
        rterror("data malloc failed");
        return -4;
    }
//    memset(pbin->body.data, 0, pbin->body.length);
    memcpy(pbin->body.data, data + pbin->header.headerLen, (size_t)pbin->header.length);

    return 0;
}

static int jk_protocol_bin_parse_data_ex(JKProtocolBin pbin, char *data)
{
    if (!pbin || !data) return -1;

    char *p = data;

    // version 4 bits
    int i;
    for (i = 0; i < 4; i++) {
        int j = p[0] & (1<<(i+4));
        if (j != 0) {
            pbin->header.version |= (1<<i);
        }
    }

    // direction 1 bit
    int j = p[0] & (1<<3);
    if (j != 0) {
        pbin->header.direction = 1;
    }

    // reserved 3 bits

    // command 4 bits
    for (i = 0; i < 4; i++) {
        int j = p[1] & (1<<(i+4));
        if (j != 0) {
            pbin->header.cmd |= (1<<i);
        }
    }

    // subcommand 12 bits
    for (i = 0; i < 4; i++) {
        int j = p[1] & (1 << i);
        if (j != 0) {
            pbin->header.subcmd |= (1<<(i+8));
        }
    }
    for (i = 0; i < 8; i++) {
        int j = p[2] & (1 << i);
        if (j != 0) {
            pbin->header.subcmd |= (1<<i);
        }
    }

    // code 8 bits
    j = p[3] & (1 << 7);
    if (j != 0) {
        pbin->header.ret = 1;
    }

    for (i = 0; i < 7; i++) {
        int j = p[3] & (1 << i);
        if (j != 0) {
            pbin->header.retCode |= (1 << i);
        }
    }

    int index = 4;
    // id 6 bytes
    char tmpid[8] = {0};
    memcpy(tmpid, p+index, 6);
    memset(pbin->header.id, 0, sizeof(pbin->header.id));

    index += 6;
    // reserved 2 bytes

    index += 2;
    // transaction 8 bytes
    char tmp[8] = {0};
    memcpy(tmp, p+index, 8);
    pbin->header.transaction = atol(tmp);

    // sequence 2 bytes
    index += 8;
    char tmpseq[2] = {0};
    memcpy(tmpseq, p+index, 2);
    pbin->header.sequence = atoi(tmpseq);

    // length 2 bytes
    index += 2;
    char tmplen[2] = {0};
    memcpy(tmplen, p+index, 2);
    pbin->header.length = atoi(tmplen);

    // sign 16 bytes
    index += 2;
    memcpy(pbin->header.sign, p+index, 16);

    // data
    index += 16;

    if (pbin->header.length > (1 << 16 -1)) {
        rterror("length is wrong...")
        return -3;
    }
    pbin->body.length = pbin->header.length;
    pbin->body.data = (char*) bvpu_mem_calloc(1, pbin->body.length);
    if (!pbin->body.data) {
        rterror("body data malloc failed.")
        return -4;
    }
    memcpy(pbin->body.data, p+index, pbin->body.length);

    return 0;
}

int jk_protocol_bin_check_header_valid(JKProtocolBin pbin)
{
    char *sign = jk_protocol_bin_generate_sign(pbin);
    if (strcmp(sign, pbin->header.sign) == 0) {
        return 1;
    } else {
        rtdebug("failed check sign: [%s][%s]", sign, pbin->header.sign);
    }
    return 0;
}

int jk_protocol_bin_parse_data(JKProtocolBin pbin, char *data, int len)
{
    if (!pbin || !data) return -2;
    if (len < pbin->header.headerLen) return -1;

#ifdef JK_USE_TEXT
    int ret = jk_protocol_bin_parse_data_text(pbin, data, len);
    if (ret < 0) {
        rterror("protocol parse failed: %d", ret);
        return -4;
    }
#else
    int ret = jk_protocol_bin_parse_data_ex(pbin, data);
    if (ret < 0) {
        rterror("protocol parse failed %d", ret);
        return -4;
    }
#endif
    ret = jk_protocol_bin_check_valid(pbin);
    if (ret < 0) {
        rterror("protocol parse has invalid args %d...", ret);
        return -3;
    }
    ret = jk_protocol_bin_check_header_valid(pbin);
    if (ret < 0) {
        rterror("header is not valid %d", ret);
        return -5;
    }

    return 0;
}

// Don't forget free the data receved.
int jk_protocol_bin_parse_data_free(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    if (pbin->body.data) {
        bvpu_mem_free(pbin->body.data);
    }
    pbin->body.data = NULL;
    pbin->body.length = 0;
    return 0;
}

int jk_protocol_bin_get_version(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.version;
}

char* jk_protocol_bin_get_id(JKProtocolBin pbin)
{
    if (!pbin) return NULL;
    return pbin->header.id;
}

int jk_protocol_bin_get_cmd(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.cmd;
}

int jk_protocol_bin_get_subcmd(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.subcmd;
}

int jk_protocol_bin_get_sequence(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.sequence;
}

int jk_protocol_bin_get_direction(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.direction;
}

int jk_protocol_bin_get_ret(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.ret;
}

int jk_protocol_bin_get_retcode(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.retCode;
}

char* jk_protocol_bin_get_data(JKProtocolBin pbin)
{
    if (!pbin) return NULL;
    return pbin->body.data;
}

int jk_protocol_bin_get_data_length(JKProtocolBin pbin)
{
    if (!pbin) return -1;
    return pbin->header.length;
}


