//
// Created by v on 18-7-31.
//

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cm_utils.h"
#include "cm_utils_ex.h"

ThreadBase::ThreadBase() {
    iAlive = 0;
    pthread_mutex_init(&iLock, NULL);
}

ThreadBase::~ThreadBase() {
    pthread_cancel(iThread);
    pthread_mutex_destroy(&iLock);
}

int ThreadBase::start() {
    int ret = pthread_create(&iThread, NULL, local_run, this);
    if (ret > 0) {
        pthread_detach(iThread);
    } else {
        iAlive = 0;
    }
    return ret;
}

int ThreadBase::stop() {
    iAlive = 0;
    pthread_cancel(iThread);
    return 0;
}

void* ThreadBase::local_run(void *args) {
    ThreadBase *tb = (ThreadBase*)args;
    tb->iAlive = 1;
    tb->run(tb);
    return NULL;
}


ProcessThreadBase::ProcessThreadBase() {
    iExit = 0;
}

ProcessThreadBase::~ProcessThreadBase() {

}

void ProcessThreadBase::runit(void *args) {
    ProcessThreadBase *p = (ProcessThreadBase*)args;
    p->run(args);
}

int ProcessThreadBase::start() {
    szThread = std::thread(runit, this);
    return 0;
}

int ProcessThreadBase::stop() {
    iExit = 1;
    szThread.join();
    return 0;
}

CMWifiInfo::CMWifiInfo() {

}

CMWifiInfo::~CMWifiInfo() {

}

int CMWifiInfo::parse_from_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (f) {
        char data[1024] = {0};
        int len = 1024;
        const char *catch_format = "%*[^=]=\"%[^\"]";

        while (fgets(data, len, f) != NULL) {
            cm_remove_space(data);
            if (strstr(data, "network=")) {
                CMWifiItem item;
                memset(&item, 0, sizeof(CMWifiItem));
                while (fgets(data, len, f)) {
                    cm_remove_space(data);
                    if (data[0] == '}') break;
                    char *p = strstr(data, "ssid=");
                    if (p) {
                        sscanf(p, catch_format, item.szSSID);
                        continue;
                    }
                    p = strstr(data, "psk=");
                    if (p) {
                        sscanf(p, catch_format, item.szPassword);
                        continue;
                    }
                    p = strstr(data, "bssid=");
                    if (p) {
                        sscanf(p, catch_format, item.szBSSID);
                        continue;
                    }
                }
                szWifiList.push_back(item);

                if (data[0] == '}') continue;
            }
        }

        fclose(f);
        return 0;
    }
    return -2;
}

int CMWifiInfo::init_from_file(const char *filename) {
    if (access(filename, F_OK) != 0) {
        return -1;
    }
    szWifiList.clear();
    return parse_from_file(filename);
}

int CMWifiInfo::get_wifi_item(char *ssid, CMWifiItem *item) {
    std::list<CMWifiItem>::iterator iter;
    for (iter = szWifiList.begin(); iter != szWifiList.end(); iter++) {
        CMWifiItem titem = *iter;
        if (std::string(ssid) == titem.szSSID) {
            *item = titem;
            return 0;
        }
    }
    return -1;
}
