/*
 *===================================================================
 *
 *          Name: kfrasp.h
 *        Create: 2015年10月23日 星期五 17时42分17秒
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: jmdvirus@roamter.com
 *
 *===================================================================
 */
#ifndef __KF_RASP_H
#define __KF_RASP_H

#ifdef __cplusplus
extern "C" {
#endif

// Router: The router for user to connect network.
// App: The terminal conntect to router, such as ios, android.
// Server: The program for Router and App to connect in the cloud.

// Command
enum {
    KF_RASP_CMD_QUERY = 1,
    KF_RASP_CMD_CONTROL,
    KF_RASP_CMD_NOTIFY
};

//SubCommand
//Connect between App,Server and Router,Server
enum {
    KF_RASP_SUBCMD_REGISTER = 1, // App send register to Router.
    KF_RASP_SUBCMD_BINDREQUEST,  // App to Router, send bind request.
    KF_RASP_SUBCMD_CONNECTAUTHORITY,  // Router to Server, send information of local connect authority, like username, password.
    KF_RASP_SUBCMD_CONNECTREQUEST, // App to Server, app request connect info from server.
    KF_RASP_SUBCMD_BINDLISTS,   // Router to Server, request bind lists of the router, used when local router has been clean up.
    KF_RASP_SUBCMD_UNBINDREQUEST,  // Router to Server, tell server, Router unbind device.
};

// Used for kfrasp_parse_data
// and KFRaspBody
// NO RESULT command means from sender
// HAS RESULT command means response of receiver.
enum {
    KFRASP_PARSE_DATA_COMMON_RESULT = 1, // some args just need result success or fail, no msg, so use it
    KFRASP_PARSE_DATA_DEVICEINFO,
    KFRASP_PARSE_DATA_DEVICEINFO_RESULT,
    KFRASP_PARSE_DATA_BINDREQUEST,
    KFRASP_PARSE_DATA_BINDREQUEST_RESULT,
    KFRASP_PARSE_DATA_BINDLISTS,
    KFRASP_PARSE_DATA_BINDLISTS_RESULT,
    KFRASP_PARSE_DATA_CONNECTITEMINFO,
    KFRASP_PARSE_DATA_CONNECTITEMINFO_RESULT,
    KFRASP_PARSE_DATA_CONNECTAUTHORITY,
    KFRASP_PARSE_DATA_CONNECTAUTHORITY_RESULT,
};


// DeviceInfo 
// for App to Router, Router will check them to valid it.
typedef struct {
    char         szName[256]; // Name of the device.
    char         szAddr[64]; // Addr of device, the ipv4 address
    char         szMac[64];  // Mac of device, set null if can find.
} KFDeviceInfo;

// Router response to App with this info.
// Router send to Server, and App send to Server use this info.
// Server will check the valid between them.
// Server may only need szDeviceMac and szRouterMac
typedef struct {
    char         szName[256];  // Name of the device from KFDeviceInfo
    char         szAddr[64];   // Addr of device, from KFDeviceInfo
    char         szDeviceMac[64]; // Mac of Device, from KFDeviceInfo
    char         szRouterMac[64]; // Router Mac.
} KFConnectItemInfo;

// Router will send to Server for App to connect.
// Also Router accept request and response it.
typedef struct {
    char         szAddr[64];  // Addr for App to connect.
    int          iPort ;    // The Port for App to connect.
    char         szUsername[32]; // The Name to connect
    char         szPassword[64];  // Password to connect.
    char         szLocalMac[64]; // Router mac
    char         szKey[128];   // ikev1 use.
    int          iType ;  // 0 : normal, 1 special
    int          iStatus ; // 0 : enable, 1 disable
} KFConnectAuthority;

typedef struct {
    int                counts;
    KFConnectItemInfo *cii;
} KFBindLists;

typedef struct {
    int     iData; // KFRASP_PARSE_DATA_* tell you which union use.
    char    ResultCode[32];
    // iData == 0 use ResultData
    char    ResultData[265];
    // iData > 0 with below.
    union {
        KFDeviceInfo di;
        KFConnectItemInfo cii;
        KFConnectAuthority ca;
        KFBindLists bl;
    };
} KFRaspBody;

typedef struct tagKFRasp *KFRasp;

int kf_rasp_init(KFRasp *rasp);

int kf_rasp_deinit(KFRasp *rasp);

// Generate header for ready everything before send.
// @cmd: KF_RASP_CMD_*
// @subcmd: KF_RASP_SUBCMD_*
int kf_rasp_generate_header(KFRasp rasp, int cmd, int subcmd, const char *id);

// Set body, let function knows the body info.
int kf_rasp_set_body(KFRasp rasp, KFRaspBody *body);

// Generate string
// like json string
// @len is the length enough of @dataSave when as input.
// Save string to @dataSave and set len also
// Call this must after call kf_rasp_generate_header and kf_rasp_set_body.
int kf_rasp_generate_data(KFRasp rasp, char *dataSave, int *len);

///////////////////////////////////////////
// Parse data to struct rasp.
int kf_rasp_parse_data(KFRasp rasp, const char *data);

// Only for free bindlists data.
// Free it if you needn't them 
int kf_rasp_parse_data_free(KFRasp rasp);

//=======================================================================
//          Get data from outside.
//=======================================================================
int kf_rasp_cmd(KFRasp rasp);
int kf_rasp_subcmd(KFRasp rasp);
KFRaspBody* kf_rasp_body(KFRasp rasp);


#ifdef __cplusplus
}
#endif

#endif  // __KF_RASP_H

/*=============== End of file: kfrasp.h =====================*/
