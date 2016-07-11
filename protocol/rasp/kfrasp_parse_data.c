/*
 *===========================================================================
 *
 *          Name: kfrasp_parse_data.c
 *        Create: 2015年11月17日 星期二 09时14分21秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include "rasp/kfrasp_parse_data.h"
#include "bvpu_utils.h"
#include "rt_print.h"
#include "rasp/kfrasp_string.h"
#include "kfrasp.h"

// Ignore it.
static int kf_rasp_process_common_result(struct json *J, KFRaspBody *rb)
{
    rtdebug("Start to parse common result %d", rb->iData);
    if (!J) return -1;
    if (!rb) return -2;
    int ret = 0;
    // Parse out the data
    json_push(J, KF_JSON_STRING(kfrasp_m_body()));
    json_push(J, KF_JSON_STRING(kfrasp_m_result()));

    // received command, not response.
    const char *str = json_string(J, KF_JSON_STRING(kfrasp_m_resultcode()));
    if (!str) {
        rterror("No result code.");
        ret = -1;
    } else {
        rtinfo("Get result code : %s", str);
        snprintf(rb->ResultCode, sizeof(rb->ResultCode), "%s", str);
        if (kf_string_compare(rb->ResultCode, "SUCCESS") == 0) {
//            rb->iData = KFRASP_PARSE_DATA_DEVICEINFO_RESULT;
            // Parse to struct KFDeviceInfo
        } else {
            rb->iData = 0;
            ret = -3;
            // fail.
            const char *p = json_string(J, KF_JSON_STRING(kfrasp_m_resultdata()));
            if (p) {
                snprintf(rb->ResultData, sizeof(rb->ResultData), "%s", p);
            } else {
                rtdebug("No resultdata section");
                ret = -2;
            }
        }
    }

    json_pop(J);
    json_pop(J);
   
    return ret;
}

// Ignore now.
KFRaspParseDataHandle KFRaspParseCommonResult = {
    .num = KFRASP_PARSE_DATA_COMMON_RESULT,
    .process = &kf_rasp_process_common_result,
};

static int kf_rasp_process_deviceinfo(struct json *J, KFRaspBody *rb)
{
    rtdebug("Start to parse device info query.");
    if (!J) return -1;
    
    if (!rb) return -2;
    json_push(J, ".MsgBody[0].PARAMS"); 
    json_error_t err = json_push(J, ".deviceInfo");
    if (!err) {
        rb->iData = KFRASP_PARSE_DATA_DEVICEINFO;
        KFDeviceInfo *di = &rb->di;
        snprintf(di->szName, sizeof(di->szName), "%s", json_string(J, ".name"));
        snprintf(di->szAddr, sizeof(di->szAddr), "%s", json_string(J, ".addr"));
    } else {
        rb->iData = 0;
        rterror("Device info not exist.");
    }

    json_pop(J);

    json_pop(J);

    return 0;
}

KFRaspParseDataHandle KFRaspParseDeviceInfo = {
    .num = KFRASP_PARSE_DATA_DEVICEINFO,
    .process = &kf_rasp_process_deviceinfo,
};

static int kf_rasp_process_bindlists_result(struct json *J, KFRaspBody *rb)
{
    rtdebug("Start to parse bindlists result");
    if (!J) return -1;
    if (!rb) return -2;
    int ret = 0;
    // Parse out the data
    json_push(J, KF_JSON_STRING(kfrasp_m_body()));
    json_push(J, KF_JSON_STRING(kfrasp_m_result()));

    // received command, not response.
    const char *str = json_string(J, KF_JSON_STRING(kfrasp_m_resultcode()));
    if (!str) {
        rterror("No result code.");
        ret = -1;
    } else {
        rtinfo("Get result code : %s", str);
        snprintf(rb->ResultCode, sizeof(rb->ResultCode), "%s", str);
        if (kf_string_compare(rb->ResultCode, "SUCCESS") == 0) {
            rb->iData = KFRASP_PARSE_DATA_BINDLISTS_RESULT;
            // Parse to struct KFBindLists
            //json_push(J, KF_JSON_STRING(kfrasp_m_resultdata()));
            json_push(J, ".resultData");
            int cnts = json_count(J, ".bindLists");
            rtdebug("Bindlists counts is %d", cnts);
            rb->bl.counts = cnts;
            if (cnts > 0) {
                rb->bl.cii = jk_mem_calloc(cnts, sizeof(KFConnectItemInfo));
                if (rb->bl.cii) {
                    size_t i;
                    for (i = 0; i < cnts; i++) {
                        char indxstr[32] = {0};
                        KFConnectItemInfo *ii = &rb->bl.cii[i];
                        sprintf(indxstr, ".bindLists[%1ld].deviceMac", i);
                        snprintf(ii->szDeviceMac, sizeof(ii->szDeviceMac), "%s",
                                json_string(J, indxstr));
                        sprintf(indxstr, ".bindLists[%1ld].routerMac", i);
                        snprintf(ii->szRouterMac, sizeof(ii->szRouterMac), "%s",
                                json_string(J, indxstr));
                    }
                } else ret = -4;
            } else {
                ret = -10;
            }
        } else {
            rb->iData = 0;
            ret = -3;
            // fail.
            json_push(J, KF_JSON_STRING(kfrasp_m_resultdata()));
            const char *p = json_string(J, KF_JSON_STRING(kfrasp_m_desc()));
            if (p) {
                snprintf(rb->ResultData, sizeof(rb->ResultData), "%s", p);
            } else {
                rtdebug("No resultdata section");
                ret = -2;
            }
            json_pop(J);
        }
    }

    json_pop(J);
    json_pop(J);

    return ret;
}

KFRaspParseDataHandle KFRaspParseBindListsResult = {
    .num = KFRASP_PARSE_DATA_BINDLISTS_RESULT,
    .process = &kf_rasp_process_bindlists_result,
};

/*=============== End of file: kfrasp_parse_data.c ==========================*/
