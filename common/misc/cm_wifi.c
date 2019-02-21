//
// Created by v on 19-1-20.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cm_wifi.h"
#include "cm_utils.h"

int cm_wifi_conf_init(CMWifiConf *conf, const char *file)
{
    if (!conf || !file) return -1;
    if (access(file, F_OK) != 0) return -2;

    memset(conf, 0, sizeof(CMWifiConf));

    int ex_index = 0;
    int ssid_index = 0;

    FILE *f = fopen(file, "r");
    if (f) {
        char data[1024] = {0};
        int len = 1024;
        const char *catch_format = "%*[^=]=\"%[^\"]";

        while (fgets(data, len, f) != NULL) {
            cm_remove_space(data);
            if (strstr(data, "network=") ||
                strstr(data, "network =")) {
                CMWifiItem *item = &conf->item[ssid_index++];
                memset(item, 0, sizeof(CMWifiItem));
                while (fgets(data, len, f)) {
                    cm_remove_space(data);
                    if (data[0] == '}') break;
                    char *p = strstr(data, "ssid=");
                    if (p) {
                        sscanf(p, catch_format, item->ssid);
                        continue;
                    }
                    p = strstr(data, "psk=");
                    if (p) {
                        sscanf(p, catch_format, item->pwd);
                        continue;
                    }
                }

                if (data[0] == '}') continue;
            } else {
                if (data[0] != '\n') {
                    strncpy(conf->exlines[ex_index], data, sizeof(conf->exlines[ex_index]));
                    ex_index++;
                }
            }
        }
        fclose(f);
    }
    return 0;
}

int cm_wifi_conf_write(CMWifiConf *conf, const char *file)
{
    if (!conf || !file) return -1;
    FILE * f = fopen(file, "w+");
    if (f) {
        int i = 0;
        for (i = 0; i < CM_WIFI_ITEM_MAX; i++) {
            if (conf->exlines[i][0] == '\0') break;
            {
                fwrite(conf->exlines[i], 1, strlen(conf->exlines[i]), f);
                fwrite("\n", 1, 1, f);
            }
        }
        const char *nettitle = "network = {\n";
        const char *nettitle_e = "}\n";
        for (i = 0; i < CM_WIFI_ITEM_MAX; i++) {
            CMWifiItem *item = &conf->item[i];
            if (item->ssid[0] == '\0') break;
            fwrite(nettitle, 1, strlen(nettitle), f);
            fwrite("\t", 1, 1, f);
            char ssidstr[64] = {0};
            sprintf(ssidstr, "ssid=\"%s\"\n", item->ssid);
            fwrite(ssidstr, 1, strlen(ssidstr), f);
            fwrite("\t", 1, 1, f);
            char psdstr[32] = {0};
            sprintf(psdstr, "psk=\"%s\"\n", item->pwd);
            fwrite(psdstr, 1, strlen(psdstr), f);

            fwrite(nettitle_e, 1, strlen(nettitle_e), f);
        }
        fclose(f);
    }
    return 0;
}

int cm_wifi_conf_add(CMWifiConf *conf, CMWifiItem *item)
{
    if (!conf || !item) return -1;

    int i = 0;
    for (i = 0; i < CM_WIFI_ITEM_MAX; i++) {
        if (conf->item[i].ssid[0] == '\0') break;
        if (strncmp(item->ssid, conf->item[i].ssid, strlen(item->ssid)) == 0) {
            memset(conf->item[i].pwd, 0, sizeof(conf->item[i].pwd));
            strncpy(conf->item[i].pwd, item->pwd, sizeof(item->pwd));
            break;
        }
    }
    if (conf->item[i].ssid[0] != '\0') return 0;
    memcpy(&conf->item[i], item, sizeof(*item));
    return 0;
}

int cm_wifi_conf_debug(CMWifiConf *conf)
{
    if (!conf) return -1;

    int i = 0;
    for (i = 0; i < CM_WIFI_ITEM_MAX; i++) {
        if (conf->exlines[i][0] == '\0') break;
        printf("%s\n", conf->exlines[i]);
    }
    for (i = 0; i < CM_WIFI_ITEM_MAX; i++) {
        CMWifiItem *item = &conf->item[i];
        if (item->ssid[0] == '\0') break;
        printf("%s, %s\n", item->ssid, item->pwd);
    }
    return 0;
}

#ifdef __MAIN_TEST
int main(int argc, char **args)
{
    if (argc < 2) {
        printf("Usage: %s file\n", args[0]);
        return -1;
    }
    const char *file = args[1];
    CMWifiConf conf;
    cm_wifi_conf_init(&conf, file);

    cm_wifi_conf_debug(&conf);

    CMWifiItem *item = &conf.item[0];
    int i = 0;
    for (i = 0; i < CM_WIFI_ITEM_MAX; i++) {
        if (conf.item[i].ssid[0] == '\0') {
            item = &conf.item[i];
            printf("Find index [%d]\n", i);
            break;
        }
    }

    CMWifiItem additem;
    strcpy(additem.ssid, "AA34");
    strcpy(additem.pwd, "12348765");
    cm_wifi_conf_add(&conf, &additem);

    cm_wifi_conf_write(&conf, file);

    return 0;
}
#endif