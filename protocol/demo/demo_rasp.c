/*
 *===========================================================================
 *
 *          Name: demo_rasp.c
 *        Create: 2015年10月23日 星期五 20时12分16秒
 *
 *   Discription: 
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <kfrasp.h>

#include "rasp/kfrasp.h"
#include "rt_print.h"

// First test generate command.


// Parse data
static int kf_rasp_parse_data_test(KFRasp rasp)
{
    // Error response test
    const char *dataerror = "{\"MsgBody\":{\"result\":{\"resultCode\":\"FAIL-001\",\"resultData\":{\"desc\":\"Itisa test\","
          "\"Operation\":{\"Cmd\":\"Query\",\"Source\":\"Server\"}}}},"
          "\"MsgHead\":{\"ServiceCode\":\"BindLists\",\"SrcSysID\":\"0002\","
          "\"SrcSysSign\":\"9624cc6c436d8c81aa3b6f8c7ed9045e\",\"transactionID\":\"1447727574224\"}}";

    // Success response test
    const char *datadi = "{\"MsgBody\":[{\"PARAMS\":"
          "{\"Operation\":{\"Cmd\":\"Control\",\"Source\":\"Device\"},"
          "\"DeviceInfo\":{\"Name\":\"Iphone-5S\",\"Addr\":\"192.168.133.111\"}}}],"
          "\"MsgHead\":{\"ServiceCode\":\"Register\",\"SrcSysID\":\"0002\","
          "\"SrcSysSign\":\"534aaeac4dcef900ff457a4ffede0938\",\"transactionID\":\"1447727574224\"}}";

    const char *databl = "{\"MsgBody\":{\"result\":{\"resultCode\":\"success\",\"resultData\":"
          "{\"Operation\":{\"Cmd\":\"Query\",\"Source\":\"Router\"},"
          "\"BindLists\":[{\"DeviceMac\":\"abcd132a\",\"RouterMac\":\"bd32abce\"},"
          "{\"RouterMac\":\"be21a32e2\",\"DeviceMac\":\"83291ab32\"}]}}},"
          "\"MsgHead\":{\"ServiceCode\":\"BindLists\",\"SrcSysID\":\"0002\","
          "\"SrcSysSign\":\"9624cc6c436d8c81aa3b6f8c7ed9045e\",\"transactionID\":\"1447727574224\"}}";

    int ret = kf_rasp_parse_data(rasp, dataerror);
    return ret;
}

static int kf_rasp_generate_data_test_bindrequest(KFRasp rasp)
{
    if (!rasp) return -1;

    kf_rasp_generate_header(rasp, KF_RASP_CMD_NOTIFY, KF_RASP_SUBCMD_BINDREQUEST,"0002");
    KFRaspBody body;
    memset(&body, 0, sizeof(KFRaspBody));
    // tell function what you need want to do.
    body.iData = KFRASP_PARSE_DATA_CONNECTITEMINFO;
    // The structs depends on the @iData
    sprintf(body.cii.szDeviceMac, "%s", "12345678");
    sprintf(body.cii.szRouterMac, "%s", "87654321");

    kf_rasp_set_body(rasp, &body);

    char data[2048] = {0};
    int len = 2048;
    int ret = kf_rasp_generate_data(rasp, data, &len);
    if (ret < 0) {
        rterror("generate data failed. %d", ret);
        return -2;
    }
    rtinfo("len: %d, %d", len, ret);
    rtinfo("string: %s", data);
    return 0;
}


static int kf_rasp_generate_data_test_register_response(KFRasp rasp)
{
    if (!rasp) return -1;

    kf_rasp_generate_header(rasp, KF_RASP_CMD_CONTROL, KF_RASP_SUBCMD_REGISTER, "");
    KFRaspBody body;
    memset(&body, 0, sizeof(KFRaspBody));
    // tell function what you need want to do.
    body.iData = KFRASP_PARSE_DATA_CONNECTITEMINFO_RESULT;
    // The structs depends on the @iData
    sprintf(body.cii.szName, "%s", "iPhone");
    sprintf(body.cii.szAddr, "%s", "192.168.133.122");
    sprintf(body.cii.szDeviceMac, "%s", "123456789");
    sprintf(body.cii.szRouterMac, "%s", "876543210");

    // As response, you must set resultCode success or fail.
    sprintf(body.ResultCode, "%s", "success");

    kf_rasp_set_body(rasp, &body);

    char data[2048] = {0};
    int len = 2048;
    int ret = kf_rasp_generate_data(rasp, data, &len);
    if (ret < 0) {
        rterror("generate data failed. %d", ret);
        return -2;
    }
    rtinfo("len: %d, %d", len, ret);
    rtinfo("string: %s", data);
    return 0;
}

static int kf_rasp_generate_data_test_register_response_fail(KFRasp rasp)
{
    if (!rasp) return -1;

    kf_rasp_generate_header(rasp, KF_RASP_CMD_CONTROL, KF_RASP_SUBCMD_REGISTER, "");
    KFRaspBody body;
    memset(&body, 0, sizeof(KFRaspBody));
    // tell function what you need want to do.
    body.iData = KFRASP_PARSE_DATA_CONNECTITEMINFO_RESULT;
    // The structs depends on the @iData
    sprintf(body.cii.szName, "%s", "iPhone");
    sprintf(body.cii.szAddr, "%s", "192.168.133.122");
    sprintf(body.cii.szDeviceMac, "%s", "123456789");
    sprintf(body.cii.szRouterMac, "%s", "876543210");

    // As response, you must set resultCode success or fail.
    sprintf(body.ResultCode, "%s", "fail-002");
    // Set data if failed.
    sprintf(body.ResultData, "%s", "It is just failed.");

    kf_rasp_set_body(rasp, &body);

    char data[2048] = {0};
    int len = 2048;
    int ret = kf_rasp_generate_data(rasp, data, &len);
    if (ret < 0) {
        rterror("generate data failed. %d", ret);
        return -2;
    }
    rtinfo("len: %d, %d", len, ret);
    rtinfo("string: %s", data);
    return 0;
}

static int kf_rasp_generate_data_test_connectauthority(KFRasp rasp)
{
    if (!rasp) return -1;

    kf_rasp_generate_header(rasp, KF_RASP_CMD_NOTIFY, KF_RASP_SUBCMD_CONNECTAUTHORITY, "");
    KFRaspBody body;
    memset(&body, 0, sizeof(KFRaspBody));
    // tell function what you need want to do.
    body.iData = KFRASP_PARSE_DATA_CONNECTAUTHORITY;
    // The structs depends on the @iData
    sprintf(body.ca.szAddr, "%s", "192.168.133.182");
    body.ca.iPort = 12345;
    sprintf(body.ca.szUsername, "%s", "12345678");
    sprintf(body.ca.szPassword, "%s", "87654321");
    sprintf(body.ca.szLocalMac, "%s", "localmac");
    sprintf(body.ca.szKey, "%s", "key");
    body.ca.iType = 0;
    body.ca.iStatus = 1;

    kf_rasp_set_body(rasp, &body);

    char data[2048] = {0};
    int len = 2048;
    int ret = kf_rasp_generate_data(rasp, data, &len);
    if (ret < 0) {
        rterror("generate data failed. %d", ret);
        return -2;
    }
    rtinfo("len: %d, %d", len, ret);
    rtinfo("string: %s", data);
    return 0;
}

static int kf_rasp_generate_data_test_bindlists(KFRasp rasp)
{
    if (!rasp) return -1;

    kf_rasp_generate_header(rasp, KF_RASP_CMD_QUERY, KF_RASP_SUBCMD_BINDLISTS, "");
    KFRaspBody body;
    memset(&body, 0, sizeof(KFRaspBody));
    // tell function what you need want to do.
    body.iData = KFRASP_PARSE_DATA_BINDLISTS;
    // The structs depends on the @iData

    kf_rasp_set_body(rasp, &body);

    char data[2048] = {0};
    int len = 2048;
    int ret = kf_rasp_generate_data(rasp, data, &len);
    if (ret < 0) {
        rterror("generate data failed. %d", ret);
        return -2;
    }
    rtinfo("len: %d, %d", len, ret);
    rtinfo("string: %s", data);
    return 0;
}

int main()
{
    rtinfo("Start rasp test...");

    KFRasp rasp = NULL;
    int ret = kf_rasp_init(&rasp);
    if (ret < 0) {
        rterror("rasp init failed, %d", ret);
        return -1;
    }

#if 0
    kf_rasp_generate_data_test_bindrequest(rasp);
    kf_rasp_generate_data_test_register_response(rasp);
    kf_rasp_generate_data_test_register_response_fail(rasp);
    kf_rasp_generate_data_test_connectauthority(rasp);
    kf_rasp_generate_data_test_bindlists(rasp);
#endif

#if 0   // Test for parse data.
    // Test parse.
    ret = kf_rasp_parse_data_test(rasp);
    if (ret < 0) {
        rterror("rasp parse data failed %d", ret);
        return -2;
    }

    KFRaspBody *body = kf_rasp_body(rasp);
    if (body) {
        if (body->iData == 0) {
            // error.
            rtinfo("fail Data: %s", body->ResultData);
        } else if (body->iData == KFRASP_PARSE_DATA_DEVICEINFO) {
            rtinfo("DeviceInfo:Name :%s", body->di.szName);
            rtinfo("DeviceInfo:Addr: %s", body->di.szAddr);
        } else if (body->iData == KFRASP_PARSE_DATA_DEVICEINFO_RESULT) {
        } else if (body->iData == KFRASP_PARSE_DATA_BINDLISTS_RESULT) {
            KFBindLists *bl = &body->bl;
            size_t i;
            for (i = 0; i < bl->counts; i++) {
                KFConnectItemInfo *ii = bl->cii+i;
                rtinfo("BindLists %d: DeviceMac: %s", i, ii->szDeviceMac);
                rtinfo("RouterMac: %s", ii->szRouterMac);
            }
            // free bindlists data
            kf_rasp_parse_data_free(rasp);
        }
    } else {
        rterror("Something wrong of body");
    }
#endif

    kf_rasp_deinit(&rasp);

    rtinfo("End rasp test ...");
  
    return 0;
}


/*=============== End of file: demo_rasp.c ==========================*/
