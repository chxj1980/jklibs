//
// Created by v on 19-1-20.
//

#ifndef PROJECT_CM_WIFI_H
#define PROJECT_CM_WIFI_H

#define CM_WIFI_ITEM_MAX   16

typedef struct {
    char          ssid[32];
    char          pwd[16];
} CMWifiItem;

typedef struct {
    char           exlines[16][256];
    CMWifiItem     item[CM_WIFI_ITEM_MAX];
} CMWifiConf;

int cm_wifi_conf_init(CMWifiConf *conf, const char *file);

int cm_wifi_conf_write(CMWifiConf *conf, const char *file);

int cm_wifi_conf_add(CMWifiConf *conf, CMWifiItem *item);

int cm_wifi_conf_debug(CMWifiConf *conf);

#endif //PROJECT_CM_WIFI_H
