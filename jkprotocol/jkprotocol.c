/*
 *===========================================================================
 *
 *          Name: jkprotocol.c
 *        Create: 2015年10月20日 星期二 13时32分43秒
 *
 *   Discription: 
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "jkprotocol.h"
#include "rt_print.h"
#include "bvpu_utils.h"
#include "ngx_md5.h"

const char *key = "ab3w-be82f231-aa-cd1b";
const char *version = "0.0.1";
const char *seperate = "\r\n";

typedef struct {
    char        version[8];
    char        id[16];
    int         cmd;
    int         subCmd;
    time_t      tm;
    char        sign[40];
} JKProtocolHead;

typedef struct {
    char        filename[256];
    int         lendata;
    char        *data;
} JKProtocolBody;

struct tagJKProtocol {
    JKProtocolHead  head;
    JKProtocolBody  body;
};

int jk_protocol_init(JKProtocol *pro, const char *id)
{
    JKProtocol inPro = (JKProtocol) jk_mem_calloc(1, sizeof(struct tagJKProtocol));
    if (!inPro) return -1;
    sprintf(inPro->head.id, "%s", id);
    sprintf(inPro->head.version, "%s", version);

    if (pro) *pro = inPro;

    return 0;
}

int jk_protocol_deinit(JKProtocol *pro)
{
    if (pro) {
        if ((*pro)->body.data) jk_mem_free((*pro)->body.data);
        jk_mem_free(*pro);
        *pro = NULL;
    }
    return 0;
}

int jk_protocol_tostring(unsigned char *data, char *save)
{
    if (!data || !save) return -1;
    int i;
    for (i = 0; i < 16; i++) {
        char tmp[8] = {0};
        sprintf(tmp, "%02x", data[i]);
        strcat(save, tmp);
    }

    return 0;
}

int jk_protocol_generate_sign(JKProtocol pro, char *sign)
{
    if (!pro) return -1;
    char str[1024] = {0};

    char tmpcmd[16] = {0};
    sprintf(tmpcmd, "%d", pro->head.cmd);
    char tmpsubcmd[16] = {0};
    sprintf(tmpsubcmd, "%d", pro->head.subCmd);
    char tmptm[16] = {0};

    sprintf(tmptm, "%ld", pro->head.tm);
    
    sprintf(str, "%s%s%s%s%s%s%s%s%s%s%s", pro->head.version, seperate, pro->head.id, seperate,
            tmpcmd, seperate, tmpsubcmd, seperate, tmptm, seperate, key);

    unsigned char output[16] = {0};
    int ret = kfmd5_ngx((unsigned char *)str, strlen(str), output);
    jk_protocol_tostring(output, sign);
    return ret;
}

int jk_protocol_generate_register(JKProtocol pro)
{
    if (!pro) return -1;
    pro->head.cmd = JK_PROTOCOL_CMD_REGISTER;
    pro->head.tm = time(NULL);
    memset(pro->head.sign, 0, sizeof(pro->head.sign));
    jk_protocol_generate_sign(pro, pro->head.sign);
    return 0;
}

int jk_protocol_generate_notify_savefile(JKProtocol pro, const char *filename)
{
    if (!pro) return -1;
    pro->head.cmd = JK_PROTOCOL_CMD_NOTIFY;
    pro->head.subCmd = JK_PROTOCOL_SUBCMD_SAVEFILE;
    pro->head.tm = time(NULL);
    memset(pro->head.sign, 0, sizeof(pro->head.sign));
    jk_protocol_generate_sign(pro, pro->head.sign);
    snprintf(pro->body.filename, sizeof(pro->body.filename), "%s", filename);
    return 0;
}

int jk_protocol_generate_control_savefile(JKProtocol pro, const char *filename)
{
    if (!pro) return -1;
    pro->head.cmd = JK_PROTOCOL_CMD_CONTROL;
    pro->head.subCmd = JK_PROTOCOL_SUBCMD_SAVEFILE;
    pro->head.tm = time(NULL);
    memset(pro->head.sign, 0, sizeof(pro->head.sign));
    jk_protocol_generate_sign(pro, pro->head.sign);
    snprintf(pro->body.filename, sizeof(pro->body.filename), "%s", filename);
    return 0;
}

int jk_protocol_generate_string(JKProtocol pro, const char *data, char *save)
{
    if (!data || !save) return -1;
    //char *str = (char*)jk_mem_calloc(1, strlen(data) + 80);
    //if (!str) return -2;

    char tmpcmd[16] = {0};
    sprintf(tmpcmd, "%d", pro->head.cmd);
    char tmpsubcmd[16] = {0};
    sprintf(tmpsubcmd, "%d", pro->head.subCmd);
    char tmptm[16] = {0};
    sprintf(tmptm, "%ld", pro->head.tm);
    
    if (pro->head.subCmd == JK_PROTOCOL_SUBCMD_SAVEFILE) {
        sprintf(save, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", pro->head.version, seperate, pro->head.id, seperate,
            tmpcmd, seperate, tmpsubcmd, seperate, tmptm, seperate, pro->head.sign,
            seperate, pro->body.filename,
            seperate, (char*)data);
    } else {
        sprintf(save, "%s%s%s%s%s%s%s%s%s%s%s%s%s", pro->head.version, seperate, pro->head.id, seperate,
            tmpcmd, seperate, tmpsubcmd, seperate, tmptm, seperate, pro->head.sign,
            seperate, (char*)data);
    }
    //int ret = sprintf(save, "%s", str);
    return 0;
}

int jk_protocol_parse(JKProtocol pro, const char *data, int len)
{
    if (!pro || !data) return -1;
    memset(pro, 0, sizeof(struct tagJKProtocol));
    char *p = (char*)data;
    char *pn = (char*)data;

    int step = strlen(seperate);

    int headlen = 0;

    pn = strstr((char*)data, seperate);
    if (pn) {
        int len = pn - p;
        strncpy(pro->head.version, p, len);
        pn += step;
        p = pn;
        headlen += len + step;
    }
    
    pn = strstr(p, seperate);
    if (pn) {
        int len = pn - p;
        strncpy(pro->head.id, p, len);
        pn += step;
        p = pn;
        headlen += len + step;
    }

    pn = strstr(p, seperate);
    if (pn) {
        char tmp[4] = {0};
        int len = pn - p;
        strncpy(tmp, p, len);
        pro->head.cmd = atoi(tmp);
        pn += step;
        p = pn;
        headlen += len + step;
    }

    pn = strstr(p, seperate);
    if (pn) {
        char tmp[4] = {0};
        int len = pn - p;
        strncpy(tmp, p, len);
        pro->head.subCmd = atoi(tmp);
        pn += step;
        p = pn;
        headlen += len + step;
    }

    pn = strstr(p, seperate);
    if (pn) {
        char tmp[16] = {0};
        int len = pn - p;
        strncpy(tmp, p, len);
        pro->head.tm = atoi(tmp);
        pn += step;
        p = pn;
        headlen += len + step;
    }

    char sign[40] = {0};
    pn = strstr(p, seperate);
    if (pn) {
        int len = pn - p;
        strncpy(sign, p, len);
        pn += step;
        p = pn;
        headlen += len + step;
    }

    memset(pro->head.sign, 0, sizeof(pro->head.sign));
    jk_protocol_generate_sign(pro, pro->head.sign);
    if (strcmp(sign, pro->head.sign) != 0) {
        return -2;
    }

    int datalen = len - headlen - step;
    pro->body.data = (char*)jk_mem_malloc(datalen);
    if (pro->body.data) {
        sprintf(pro->body.data, "%s", p);
    }
    if (pro->head.subCmd == JK_PROTOCOL_SUBCMD_SAVEFILE) {
        char *sdata = strdup(pro->body.data);
        char *p = sdata;
        char *pn = strstr(p, seperate);
        if (pn) {
            int len = pn - p;
            strncpy(pro->body.filename, p, len);
            memset(pro->body.data, 0, datalen);
            strncpy(pro->body.data, pn+step, datalen - len - step);
        }
        if (sdata) jk_mem_free(sdata);
    }
    rtdebug("[%s][%s][%s]", pro->head.version, pro->head.id, pro->body.filename);

    return 0;
}

int jk_protocol_cmd(JKProtocol p)
{
    if (p) return p->head.cmd;
    else return -1;
}

int jk_protocol_subcmd(JKProtocol p)
{
    if (p) return p->head.subCmd;
    else return -1;
}

char *jk_protocol_data(JKProtocol p)
{
    if (p) return p->body.data;
    else return NULL;
}

int jk_protocol_data_free(JKProtocol p)
{
    if (p->body.data) jk_mem_free(p->body.data);
    p->body.data = NULL;
    return 0;
}

/*=============== End of file: jkprotocol.c ==========================*/
