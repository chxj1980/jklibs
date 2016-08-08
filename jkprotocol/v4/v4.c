//
// Created by v on 16-8-4.
//

#include <string.h>
#include <malloc.h>
#include "v4.h"
#include "rt_print.h"

#define JK_PRO_V4_HEADER_LENGTH   2

#define NULL_RETURN(v4) if (!v4) { return -1; }

int jk_pro_v4_deinit(JKProV4 *v4)
{
    NULL_RETURN(v4)
    if (v4->szBody.szData) {
        free(v4->szBody.szData);
        v4->szBody.szData = NULL;
    }
    memset(v4, 0, sizeof(JKProV4));
    return 0;
}

int jk_pro_v4_clear(JKProV4 *v4) {
    NULL_RETURN(v4)
    if (v4->szBody.szData) {
        free(v4->szBody.szData);
        v4->szBody.szData = NULL;
    }
    v4->szHeader.iLength = 0;
    v4->szHeader.iACK = 0;
    return 0;
}

int jk_pro_v4_set_header(JKProV4 *v4, int ack, int length)
{
    NULL_RETURN(v4)
    v4->szHeader.iACK = ack;
    v4->szHeader.iLength = length;
    return 0;
}

int jk_pro_v4_free_body(JKProV4 *v4)
{
    NULL_RETURN(v4)
    if (v4->szBody.szData) {
        free(v4->szBody.szData);
        v4->szBody.szData = NULL;
    }
    return 0;
}

int jk_pro_v4_to_bytes(JKProV4 *v4, char **result)
{
    NULL_RETURN(v4)
    // set header
    char *ret = (char*)calloc(1, v4->szHeader.iLength+JK_PRO_V4_HEADER_LENGTH);
    if (!ret) return -2;

    rtdebug("Version: %d, crypt: %d, ack: %d, length: %d", v4->szHeader.iVersion,
    v4->szHeader.iCrypt, v4->szHeader.iACK, v4->szHeader.iLength);

    char *p = &ret[0];
    int i;
    for (i = 0; i < 4; i++) {
        int v = v4->szHeader.iVersion & (1<<i);
        if (v) {
            p[0] |= (1<<(i+4));
        }
    }

    for (i = 0; i < 2; i++) {
        int v = v4->szHeader.iCrypt & (1<<i);
        if (v) {
            p[0] |= (1<<(i+2));
        }
    }

    int v = v4->szHeader.iACK;
    if (v) {
        p[0] |= (1<<i);
    }

    for (i = 0; i < 32; i++) {
        int v = v4->szHeader.iLength & (1<<i);
        if (v) {
            p[1] |= (1<<i);
        }
    }

    memcpy(&p[2], v4->szBody.szData, v4->szHeader.iLength);

    if (result) *result = ret;
    return 0;
}

int jk_pro_v4_parse(JKProV4 *v4, const char *bytes)
{
    NULL_RETURN(v4)
    if (!bytes) return -2;

    memset(&v4->szHeader, 0, sizeof(JKProV4Header));
    if (v4->szBody.szData) {
        free(v4->szBody.szData);
    }
    memset(&v4->szBody, 0, sizeof(JKProV4Body));

    const char *p = bytes;
    int v = p[0] & (1<<1);
    if (v) {
        v4->szHeader.iACK = 1;
    }

    int i;
    for (i = 0; i < 2; i++) {
        int v = p[0] & (1<<(i+2));
        if (v) {
            v4->szHeader.iCrypt |= 1<<i;
        }
    }

    for (i = 0; i < 4; i++) {
        int v = p[0] & (1<<(i+4));
        if (v) {
            v4->szHeader.iVersion |= 1<<i;
        }
    }

    for (i = 0; i < 32; i++) {
        int v = p[1] & (1<<i);
        if (v) {
            v4->szHeader.iLength |= 1<<i;
        }
    }

    if (v4->szHeader.iLength >= (1<<31-1)) {
        rterror("Invalid length %d", v4->szHeader.iLength);
        return -3;
    }

    rtdebug("parseout: version: %d, crypt: %d, ack: %d, length: %d", v4->szHeader.iVersion,
    v4->szHeader.iCrypt, v4->szHeader.iACK, v4->szHeader.iLength);

    v4->szBody.szData = (char*)calloc(1, sizeof(v4->szHeader.iLength));
    if (!v4->szBody.szData) return -4;
    memcpy(v4->szBody.szData, p+2, v4->szHeader.iLength);

    return 0;
}

int jk_pro_v4_set_body(JKProV4 *v4, const char *data)
{
    NULL_RETURN(v4)
    v4->szBody.szData = (char*)calloc(1, v4->szHeader.iLength);
    if (!v4->szBody.szData) return -2;
    memcpy(v4->szBody.szData, data, v4->szHeader.iLength);
    return 0;
}

int jk_pro_v4_init(JKProV4 *v4, int version, int crypt)
{
    memset(v4, 0, sizeof(JKProV4));

    v4->szHeader.iVersion = version;
    v4->szHeader.iCrypt = crypt;

    v4->deinit = jk_pro_v4_deinit;
    v4->clear = jk_pro_v4_clear;

    v4->parse = jk_pro_v4_parse;
    v4->to_bytes = jk_pro_v4_to_bytes;
    v4->free_body = jk_pro_v4_free_body;
    v4->set_header = jk_pro_v4_set_header;
    v4->set_body = jk_pro_v4_set_body;

    return 0;
}