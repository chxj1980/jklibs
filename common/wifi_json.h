/*================================================================
*   
*   File Name: wifi_json.h
*   Author: CHEN Z.P.
*   Mail: czp@sctek.cn
*   Created Time: 2018-06-26 16:58:00
*
================================================================*/

#ifndef _WIFIJSON_H
#define _WIFIJSON_H

#define NO_IWLIB

#ifndef NO_IWLIB
#include "iwlib.h"
#endif

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h> 

#define IW_SCAN_HACK		0x8000

#define WPA2_FLAG 0x01
#define WPA_FLAG 0x02
#define WPA1X_FLAG 0x04

#define WPA_MIX 0x03

#define ENCRYPT_TKIP 0x01
#define ENCRYPT_CCMP 0x02
#define ENCRYPT_WEP_40 0x04
#define ENCRYPT_WEP_104 0x08
#define ENCRYPT_WRAP 0x10

#ifndef FALSE
#define FALSE -1
#endif

#ifndef TRUE
#define TRUE 0
#endif

#define GETSYSSTR "getSysInfo"
#define SETSYSSTR "setSysInfo"

#define ERROR_SSID "ssid error! "
#define ERROR_PASSWORD "password error! "
#define PARAMETER_ERROR	"parameter error! "

#define FN_SET_JoinWireless "JoinWireless"
	#define JoinWireless_SET_AP "AP"
	#define JoinWireless_SET_AP_name "name"
	#define JoinWireless_SET_AP_encrypt "encrypt"
	#define JoinWireless_SET_AP_channel "channel"
	#define JoinWireless_SET_AP_tkip_aes "tkip_aes"
	#define JoinWireless_SET_AP_password "password"


#ifdef __cplusplus
extern "C" {
#endif

/****************************** TYPES ******************************/

/*
 * Scan state and meta-information, used to decode events...
 */
typedef struct iwscan_state {
    /* State */
    int ap_num;        /* Access Point number 1->N */
    int val_index;    /* Value in table 0->(N-1) */
} iwscan_state;

typedef struct wifi_json_info {
    char name[56];
    char encrypt[32];
    char password[32];
    char tkipaes[32];
    char channel[32];
} WIFI_JSON_INFO;

typedef struct wpa_conf_info {
    char ssid[64];
    char psk[128];
    char key_mgmt[32];
    char bssid[24];
    char priority[8];
} WiFi_Info;


extern int wifi_json_scan(char *ifname, char *outstr);

extern int wifi_json_join_unpack(const char *jsonstr, char *retstr, WIFI_JSON_INFO *wifiinfo);

extern int wifi_json_join_set(WIFI_JSON_INFO *wifiinfo);

int create_wpa_config_file(char *ssid, char *password, char *bssid);

#ifdef __cplusplus
}
#endif

#endif
