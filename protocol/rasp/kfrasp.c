/*
 *===========================================================================
 *
 *          Name: kfrasp.c
 *        Create: 2015年10月23日 星期五 18时32分12秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <time.h>

#include "rt_print.h"
#include "bvpu_utils.h"
#include "rasp/kfrasp.h"
#include "ngx_md5.h"
#include "json.h"

#include "rasp/kfrasp_parse_data.h"
#include "rasp/kfrasp_string.h"
#include "kfrasp.h"

#define KF_RASP_ARGS_INVALID(p) \
    if (!p) return -1; \
    if (!p->Json) return -2;

// ErrorInfo
// UNBIND: didn't bind.
// BINDFAIL: bind failed as wrong information
// UNBINDFAIL: unbind failed, never happen is right.
// OFFLINE: Router is offline.
const char *KFRASPErrorDesc[] = {
    "UNFIND", "BINDFAIL", "UNBINDFAIL", "OFFLINE",
};

typedef struct {
    int     iCmd;
    int     iSubCmd;
    char    Cmd[32];
    char    SubCmd[32];
    char    Id[8];  // need to use for now.
    time_t  Time;
    char    Sign[34];
    char    Source[16];
} KFRaspHeader;

struct tagKFRasp {
    KFRaspHeader header;
    KFRaspBody   body;
    char         headerStr[128];
    char         *bodyStr;
    struct json  *Json;
};

struct KFRaspCmd {
    int       iCmd;
    char      Cmd[32];
};
static struct KFRaspCmd RaspCmd[] = {
    { KF_RASP_CMD_QUERY, "query" },
    { KF_RASP_CMD_CONTROL, "control" },
    { KF_RASP_CMD_NOTIFY, "notify" },
};
size_t RaspCmdLen = sizeof(RaspCmd)/sizeof(RaspCmd[0]);

struct KFRaspSubCmd {
    int        iCmd;
    char       Cmd[32];
};
static struct KFRaspSubCmd RaspSubCmd[] = {
    { KF_RASP_SUBCMD_REGISTER, "register" },
    { KF_RASP_SUBCMD_BINDREQUEST, "bindRequest" },
    { KF_RASP_SUBCMD_CONNECTAUTHORITY, "connectAuthority" },
    { KF_RASP_SUBCMD_BINDLISTS, "bindLists" },
    { KF_RASP_SUBCMD_UNBINDREQUEST, "unbindRequest" },
};
size_t RaspSubCmdLen = sizeof(RaspSubCmd)/sizeof(RaspSubCmd[0]);

// We need a function convert between string and int for cmd and subcmd
// id 1: cmd, 2 subcmd
static inline int kf_rasp_cmd_str_int(const char *string)
{
    if (!string) return -1;
    size_t i;
    for (i = 0; i < RaspCmdLen; i++) {
        if (kf_string_compare(string, RaspCmd[i].Cmd) == 0) {
            return RaspCmd[i].iCmd;
        }
    }
    return 0;
}

static inline const char *kf_rasp_cmd_int_str(int iCmd)
{
    size_t i;
    for (i = 0; i < RaspCmdLen; i++) {
        if (iCmd == RaspCmd[i].iCmd) {
            return RaspCmd[i].Cmd;
        }
    }
    return NULL;
}

static inline int kf_rasp_subcmd_str_int(const char *string)
{
    if (!string) return -1;
    size_t i;
    for (i = 0; i < RaspSubCmdLen; i++) {
        if (kf_string_compare(string, RaspSubCmd[i].Cmd) == 0) {
            return RaspSubCmd[i].iCmd;
        }
    }
    return 0;
}

static inline const char *kf_rasp_subcmd_int_str(int iCmd)
{
    size_t i;
    for (i = 0; i < RaspSubCmdLen; i++) {
        if (iCmd == RaspSubCmd[i].iCmd) {
            return RaspSubCmd[i].Cmd;
        }
    }
    return NULL;
}

int kf_rasp_init(KFRasp *rasp)
{
    KFRasp inR = (KFRasp)jk_mem_malloc(sizeof(struct tagKFRasp));
    if (!inR) return -1;
    memset(inR, 0, sizeof(struct tagKFRasp));
    int error = 0;
    inR->Json = json_open(JSON_F_NONE, &error);
    if (!inR->Json) {
        rterror("json open failed %d", error);
        return -2;
    }

    if (rasp) *rasp = inR;
 
    return 0;
}

int kf_rasp_deinit(KFRasp *rasp)
{   
    if (rasp) {
        json_close((*rasp)->Json);
        jk_mem_free(*rasp);
        *rasp = NULL;
    }
    return 0;
}

// Generate sign with given string and md5
static int kf_rasp_generate_sign(KFRasp rasp, char *signSave)
{
    KF_RASP_ARGS_INVALID(rasp);
    char tmpstr[128] = {0};
    sprintf(tmpstr, "%s-%s-%ld-%s", rasp->header.SubCmd, rasp->header.Id, rasp->header.Time,
            kfrasp_m_key());
    unsigned char save[16] = {0};
    kfmd5_ngx((unsigned char *)tmpstr, strlen(tmpstr), save);
    kfmd5_ngx_tostring(save, signSave);
    return 0;
}

// Generate header for ready everything before send.
int kf_rasp_generate_header(KFRasp rasp, int cmd, int subcmd, const char *id)
{
    if (!rasp || !cmd || !id) return -1;

    const char *p = kf_rasp_cmd_int_str(cmd);
    if (!p) return -2;
    rasp->header.iCmd = cmd;
    snprintf(rasp->header.Cmd, sizeof(rasp->header.Cmd), "%s", p);
    p = kf_rasp_subcmd_int_str(subcmd);
    if (!p) return -3;
    rasp->header.iSubCmd = subcmd;
    snprintf(rasp->header.SubCmd, sizeof(rasp->header.SubCmd), "%s", p);
    snprintf(rasp->header.Id, sizeof(rasp->header.Id), "%s", id);
    rasp->header.Time = time(NULL);
    //rasp->header.Time = 1452166191;
    int ret = kf_rasp_generate_sign(rasp, rasp->header.Sign);
    sprintf(rasp->header.Source, "%s", "router");
    return ret;
}

// Ready json for rasp->Json, need be use when send it after body ready.
// Call before generate string to send.
static int kf_rasp_header_ready_data(KFRasp rasp)
{
    KF_RASP_ARGS_INVALID(rasp);
    struct json *J = rasp->Json;

    const char *p = KF_JSON_STRING(kfrasp_m_head());
    json_push(J, p);
    json_setstring(J, rasp->header.SubCmd, KF_JSON_STRING(kfrasp_m_cmd()));
    json_setstring(J, rasp->header.Id, KF_JSON_STRING(kfrasp_m_subcmd()));
    //json_setstring(J, rasp->header.Id, ".SrcSysID");
    json_setstring(J, rasp->header.Sign, KF_JSON_STRING(kfrasp_m_sign()));
    json_setstring(J, jk_time_string(rasp->header.Time), KF_JSON_STRING(kfrasp_m_tm()));

    json_pop(J);

    return 0;
}

// This take header to string, ignore it.
const char *kf_rasp_header_todata(KFRasp rasp)
{
    if (!rasp || !rasp->Json) return NULL;
    struct json *J = rasp->Json;

    json_push(J, KF_JSON_STRING(kfrasp_m_head()));
    json_setstring(J, rasp->header.Cmd, KF_JSON_STRING(kfrasp_m_cmd()));
    json_setstring(J, rasp->header.SubCmd, KF_JSON_STRING(kfrasp_m_subcmd()));
    //json_setstring(J, rasp->header.Id, ".SrcSysID");
    json_setstring(J, rasp->header.Sign, KF_JSON_STRING(kfrasp_m_sign()));
    json_setstring(J, jk_time_string(rasp->header.Time), KF_JSON_STRING(kfrasp_m_tm()));

    json_pop(J);

    int error = 0;
    int ret = json_printstring(J, rasp->headerStr, sizeof(rasp->headerStr), JSON_F_NONE,
           &error);
    if (ret < 0) {
        rterror("json to string failed %d", ret);
        return NULL;
    }

    return rasp->headerStr;
}

//=======================================================================
//          Parse json data
//=======================================================================

static int kf_rasp_header_valid(KFRasp rasp)
{
    char genStr[128] = {0};
    sprintf(genStr, "%s-%s-%ld-%s", rasp->header.SubCmd, rasp->header.Id, rasp->header.Time,
            kfrasp_m_key());
    rtdebug("Receiver gen data: %s, receiver data: %s", genStr, rasp->header.Sign);
    char sign[34] = {0};
    kf_rasp_generate_sign(rasp, sign);
    rtdebug("Sign before [%s], next[%s]", sign, rasp->header.Sign);
    if (strcmp(sign, rasp->header.Sign) == 0) return 1;
    else return 0;
}

// Parse out header parameter then check if it is valid.
static int kf_rasp_parse_data_header(KFRasp rasp)
{
    KF_RASP_ARGS_INVALID(rasp);
    KFRaspHeader *h = &rasp->header;
    struct json *J = rasp->Json;

    json_push(J, KF_JSON_STRING(kfrasp_m_head()));

    const char *str = json_string(J, KF_JSON_STRING(kfrasp_m_cmd()));
    if (str) {
        h->iSubCmd = kf_rasp_subcmd_str_int(str);
        snprintf(h->SubCmd, sizeof(h->SubCmd), "%s", str);
        rtdebug("parse out sub cmd [%s][%d]", h->SubCmd, h->iSubCmd);
    }
    str = json_string(J, KF_JSON_STRING(kfrasp_m_subcmd()));
    if (str) {
        //h->iSubCmd = kf_rasp_subcmd_str_int(str);
        snprintf(h->Id, sizeof(h->Id), "%s", str);
        rtdebug("parse out Id [%s]", h->Id);
    }
    str = json_string(J, KF_JSON_STRING(kfrasp_m_sign()));
    if (str) {
        snprintf(h->Sign, sizeof(h->Sign), "%s", str);
    }
    const char *tmStr = json_string(J, KF_JSON_STRING(kfrasp_m_tm()));

    json_pop(J);

    if (tmStr) {
        // Check if validator valid.
        h->Time = atol(tmStr);
        int ret = kf_rasp_header_valid(rasp);
        return ret == 1 ? 0 : -4;
    } else {
        return -3;
    }
    return 0;
}

// Generate num for invite KFRaspParseDataHandle.process with rasp.
// You can call this function to get return @num, then call process with @num.
int kf_rasp_generate_number(KFRasp rasp)
{
    if (rasp->header.iCmd == KF_RASP_CMD_QUERY) {
        if (rasp->header.iSubCmd == KF_RASP_SUBCMD_REGISTER) {
            return KFRASP_PARSE_DATA_DEVICEINFO_RESULT;
        } else if (rasp->header.iSubCmd == KF_RASP_SUBCMD_BINDLISTS) {
            return KFRASP_PARSE_DATA_BINDLISTS_RESULT;
        }
    } else if (rasp->header.iCmd == KF_RASP_CMD_CONTROL) {
        if (rasp->header.iSubCmd == KF_RASP_SUBCMD_REGISTER) {
            return KFRASP_PARSE_DATA_DEVICEINFO;
        } else if (rasp->header.iSubCmd == KF_RASP_SUBCMD_BINDREQUEST) {
            return KFRASP_PARSE_DATA_BINDLISTS_RESULT;
        }
    } else if (rasp->header.iCmd == KF_RASP_CMD_NOTIFY) {
    }
    return 0;
}

// Return pointer to struct need to call process. with @num
KFRaspParseDataHandle *kf_rasp_generate_handle(int num)
{
    switch (num) {
        case KFRASP_PARSE_DATA_DEVICEINFO:
            return &KFRaspParseDeviceInfo;
        break;
        case KFRASP_PARSE_DATA_BINDREQUEST_RESULT:
        case KFRASP_PARSE_DATA_DEVICEINFO_RESULT:
            return &KFRaspParseCommonResult;
        break;
        case KFRASP_PARSE_DATA_BINDLISTS_RESULT:
            return &KFRaspParseBindListsResult;
        break;
        default:break;
    }
    rtdebug("123123123123");
    return NULL;
}

#define KF_RASP_SUCCESS(str) \
      strcmp(str, "success") == 0 ? 1 : 0;

// Parse out the to body,
// each resultData with different structs.
static int kf_rasp_parse_msgbody(KFRasp rasp)
{
    KF_RASP_ARGS_INVALID(rasp);    
    int ret = 0;

    // TODO: how to parse body with different status, maybe string, maybe structs.

    // Here first parse operation of the data
    struct json *J = rasp->Json;
    KFRaspHeader *h = &rasp->header;

    int cnt = json_count(J, ".MsgBody[0].PARAMS");
    if (cnt == 0) {
        rtwarn("This is result data");
        json_push(J, ".MsgBody.result.resultData");
    } else {
        json_push(J, ".MsgBody[0].PARAMS");
    }
    json_push(J, KF_JSON_STRING(kfrasp_m_operation()));

    const char *str = json_string(J, KF_JSON_STRING(kfrasp_m_ccmd()));
    if (str) {
        h->iCmd = kf_rasp_cmd_str_int(str);
        snprintf(h->Cmd, sizeof(h->Cmd), "%s", str);
        rtdebug("parse out cmd [%s][%d]", h->Cmd, h->iCmd);
    } else {
        rtwarn("No cmd args");
    }
    str = json_string(J, KF_JSON_STRING(kfrasp_m_source()));
    if (str) {
        snprintf(h->Source, sizeof(h->Source), "%s", str);
        rtdebug("Parse out source [%s]", h->Source);
    } else {
        rtwarn("No source args");
    }

    json_pop(J);
    json_pop(J);

    int num = kf_rasp_generate_number(rasp);
    rasp->body.iData = num;
    rtdebug("The number is : %d", num);
    KFRaspParseDataHandle *hd = kf_rasp_generate_handle(num);
    if (hd) {
        int ret = hd->process(rasp->Json, &rasp->body);
        if (ret < 0) {
            // parse fail.
            ret = -2; goto errout;
        }
    } else {
        rtdebug("didn't find handle with num: %d", num);
    }

    return 0;
errout:
    return ret;
};

// Parse data to struct rasp.
int kf_rasp_parse_data(KFRasp rasp, const char *data)
{
    KF_RASP_ARGS_INVALID(rasp);
    if (!data) return -3;
    rtdebug("%s", data);

    struct json *J = rasp->Json;

    int error = 0;
    error = json_loadstring(J, data);
    if (error != 0) {
        rterror("json format invalid, %x", error);
        return -4;
    }

    // Parse header
    int ret = kf_rasp_parse_data_header(rasp);
    if (ret < 0) {
        // invalid command
        rterror("header is not valid. %d", ret);
        return -5;
    }

    // Parse data
    ret = kf_rasp_parse_msgbody(rasp);
    if (ret < 0) {
        rterror("body is not valid. %d", ret);
        return -6;
    }

    return 0;
}

int kf_rasp_parse_data_free(KFRasp rasp)
{
    if (!rasp) return -1;
    if (rasp->body.bl.cii) {
        jk_mem_free(rasp->body.bl.cii);
    }
    rasp->body.bl.cii = NULL;
    return 0;
}

//=======================================================================
//          Router and App
//=======================================================================

int kf_rasp_set_body(KFRasp rasp, KFRaspBody *body)
{
    if (!rasp || !body) return -1;

    // Save the data
    // ? If we need save it.
    memcpy(&rasp->body, body, sizeof(KFRaspBody));
    switch (body->iData) {
        case KFRASP_PARSE_DATA_CONNECTITEMINFO:
        // send to server
        break;
        case KFRASP_PARSE_DATA_CONNECTITEMINFO_RESULT:
        // send response to app
        break;
        case KFRASP_PARSE_DATA_CONNECTAUTHORITY:
        // send to server
        break;

        default: break;
    }

    return -2;
}

static int kf_rasp_generate_data_operation(struct json *J, const char *pos, const char *cmd, const char *source)
{
    if (!J || !cmd || !source || !pos) return -1;
    json_push(J, KF_JSON_STRING(pos));
    json_push(J, KF_JSON_STRING(kfrasp_m_operation()));
    json_setstring(J, cmd, KF_JSON_STRING(kfrasp_m_ccmd()));
    json_setstring(J, source, KF_JSON_STRING(kfrasp_m_source()));
    json_pop(J);
    json_pop(J);
    return 0;
}

static int kf_rasp_generate_data_connectiteminfo(struct json *J, KFConnectItemInfo *cii)
{
    if (!J || !cii) return -1;
    json_push(J, KF_JSON_STRING("MsgBody[0].PARAMS"));
    //json_setstring(J, rasp->body.cii.szName, ".Name");
    //json_setstring(J, rasp->body.cii.szAddr, ".Addr");
    json_push(J, KF_JSON_STRING("connectItemInfo"));
    json_setstring(J, cii->szDeviceMac, ".deviceMac");
    json_setstring(J, cii->szRouterMac, ".routerMac");
    json_pop(J);
    json_pop(J);
    return 0;
}

static int kf_rasp_generate_data_connectauthority(struct json *J, KFConnectAuthority *ca)
{
    if (!J || !ca) return -1;
    json_push(J, KF_JSON_STRING("MsgBody[0].PARAMS"));
    json_push(J, KF_JSON_STRING("connectAuthority"));
    json_setstring(J, ca->szAddr, KF_JSON_STRING("addr"));
//    char port[8] = {0};
//    sprintf(port, "%d", ca->iPort);
//    json_setstring(J, port, ".Port");
    json_setstring(J, ca->szUsername, KF_JSON_STRING("userName"));
    json_setstring(J, ca->szPassword, KF_JSON_STRING("password"));
    json_setstring(J, ca->szLocalMac, KF_JSON_STRING("mac"));
    json_setstring(J, ca->szKey, KF_JSON_STRING("secretKey"));
    char type[8] = {0};
    sprintf(type, "%d", ca->iType);
    json_setstring(J, type, KF_JSON_STRING("type"));
    char status[8] = {0};
    sprintf(status, "%d", ca->iStatus);
    json_setstring(J, status, KF_JSON_STRING("status"));
    json_pop(J);
    json_pop(J);
    return 0;
}

static int kf_rasp_generate_data_connectiteminfo_result(struct json *J, KFRaspBody *body)
{
    if (!J || !body) return -1;

    json_push(J, KF_JSON_STRING(kfrasp_m_body()));
    json_push(J, KF_JSON_STRING(kfrasp_m_result()));

    json_setstring(J, body->ResultCode, KF_JSON_STRING(kfrasp_m_resultcode()));
    if (kf_string_compare(body->ResultCode, "success") != 0) {
        json_push(J, KF_JSON_STRING(kfrasp_m_resultdata()));
        json_setstring(J, body->ResultData, KF_JSON_STRING(kfrasp_m_desc()));
        json_pop(J);
    } else {
        json_push(J, KF_JSON_STRING(kfrasp_m_resultdata()));
        KFConnectItemInfo *cii = &body->cii;
        json_push(J, KF_JSON_STRING("connectItemInfo"));
        json_setstring(J, cii->szName, KF_JSON_STRING("name"));
        json_setstring(J, cii->szAddr, KF_JSON_STRING("addr"));
        json_setstring(J, cii->szDeviceMac, KF_JSON_STRING("deviceMac"));
        json_setstring(J, cii->szRouterMac, KF_JSON_STRING("routerMac"));
        json_pop(J);
        json_pop(J);
    }

    json_pop(J);
    json_pop(J);

    return 0;
}

static int kf_rasp_generate_data_null_body(struct json *J)
{
    if (!J) return -1;
    json_push(J, KF_JSON_STRING("MsgBody[0].PARAMS"));

    json_setstring(J, "", "");
    
    json_pop(J);
    return 0;
}

// Generate string
// like json string
// Save string to @dataSave and set len also
int kf_rasp_generate_data(KFRasp rasp, char *dataSave, int *len)
{
    if (!rasp) return -1;
    if (!rasp->Json) return -2;
    if (!dataSave) return -3;
    if (!len) return -4;
    struct json *J = rasp->Json;
    json_setnull(J, KF_JSON_STRING(kfrasp_m_body()));

    kf_rasp_header_ready_data(rasp);
    switch(rasp->body.iData) {
        case KFRASP_PARSE_DATA_CONNECTITEMINFO:
        // send to server
        kf_rasp_generate_data_operation(J, "MsgBody[0].PARAMS", rasp->header.Cmd, rasp->header.Source);
        kf_rasp_generate_data_connectiteminfo(J, &rasp->body.cii);
        break;
        case KFRASP_PARSE_DATA_CONNECTITEMINFO_RESULT:
        // send response to app
        kf_rasp_generate_data_operation(J, "MsgBody.result.resultData", rasp->header.Cmd, rasp->header.Source);
        kf_rasp_generate_data_connectiteminfo_result(J, &rasp->body);
        break;
        case KFRASP_PARSE_DATA_CONNECTAUTHORITY:
        // send to server
        kf_rasp_generate_data_operation(J, "MsgBody[0].PARAMS", rasp->header.Cmd, rasp->header.Source);
        kf_rasp_generate_data_connectauthority(J, &rasp->body.ca);
        break;
        case KFRASP_PARSE_DATA_BINDLISTS:
        // send to server
        kf_rasp_generate_data_operation(J, "MsgBody[0].PARAMS", rasp->header.Cmd, rasp->header.Source);
        kf_rasp_generate_data_null_body(J);
        break;
    }

    json_error_t error = 0;
    int ret = json_printstring(J, dataSave, *len, JSON_F_NONE,
           &error);
    if (error != 0) {
        return -5;
    }
    *len = ret;
    return ret;
}

//=======================================================================
//          Router and Server
//=======================================================================

//=======================================================================
//          Get data from outside.
//=======================================================================

int kf_rasp_cmd(KFRasp rasp)
{
    if (!rasp) return -1;
    return rasp->header.iCmd;
}

int kf_rasp_subcmd(KFRasp rasp)
{
    if (!rasp) return -1;
    return rasp->header.iSubCmd;
}

KFRaspBody* kf_rasp_body(KFRasp rasp)
{
    if (!rasp) return NULL;
    return &rasp->body;
}

/*=============== End of file: kfrasp.c ==========================*/
