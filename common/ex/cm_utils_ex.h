//
// Created by v on 18-7-31.
//

#ifndef PROJECT_CM_UTILS_EX_H
#define PROJECT_CM_UTILS_EX_H

#include <pthread.h>
#include <thread>
#include <mutex>
#include <list>

class ThreadBase {
public:
    ThreadBase();
    virtual ~ThreadBase();

    int start();
    int stop();

    int lock() {
        return pthread_mutex_lock(&iLock);
    }
    int unlock() {
        return pthread_mutex_unlock(&iLock);
    }
    int trylock() {
        return pthread_mutex_trylock(&iLock) == 0;
    }

    int isAlive() {
        return iAlive;
    }

    int bexit() {
        return !iAlive;
    }

    virtual void run(void *args) = 0;

protected:
    static void *local_run(void *args);

private:
    pthread_t         iThread;
    pthread_mutex_t   iLock;
    unsigned  int               iSleep;
    int               iAlive;
};


class ProcessThreadBase {
public:
    ProcessThreadBase();
    virtual ~ProcessThreadBase();

    int start();
    int stop();

    void lock() {
        szMutex.lock();
    }
    void trylock() {
        szMutex.try_lock();
    }
    void unlock() {
        szMutex.unlock();
    }

    int bexit() {
        return iExit;
    }

protected:
    static void runit(void *args);
    virtual void run(void *args) = 0;

protected:
    std::mutex                szMutex;

private:
    std::thread               szThread;
    int                       iExit;
};

typedef struct {
    char              szSSID[64];
    char              szBSSID[128];
    char              szPassword[65];
    char              szType[32];
} CMWifiItem;

class CMWifiInfo {
public:
    CMWifiInfo();
    ~CMWifiInfo();

    int init_from_file(const char *filename);

    int get_wifi_item(char *ssid, CMWifiItem *item);

    int get_wifi_list(std::list<CMWifiItem> &wifilist) {
        wifilist = szWifiList;
        return 0;
    }

protected:
    int parse_from_file(const char *filename);

private:
    std::list<CMWifiItem>        szWifiList;
};

#endif //PROJECT_CM_UTILS_EX_H
