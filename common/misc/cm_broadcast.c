//
// Created by v on 18-9-12.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>

#include "cm_broadcast.h"
#include "cm_utils.h"

struct CMBroadcast {
    int         iSockFD;
    int         iPort;
    pthread_t   iThread;
    int         iExit;

    char        szMac[32];
    char        szIP[32];
    char        szVersion[32];
    char        szBroadMessage[128];
};

int cm_broadcast_init(CMBroadcastHandle *h, int port)
{
    struct CMBroadcast *in = (struct CMBroadcast *)cm_mem_malloc(sizeof(struct CMBroadcast));
    if (!in) {
        return -1;
    }
    memset(in, 0, sizeof(struct CMBroadcast));
    in->iPort = port;
    in->iExit = 0;

    *h = in;
    return 0;
}

int cm_broadcast_set_info(CMBroadcastHandle h, const char *mac, const char *ip, const char *version)
{
    if (!h) return -1;
    sprintf(h->szMac, "%s", mac);
    sprintf(h->szIP, "%s", ip);
    sprintf(h->szVersion, "%s", version);
    sprintf(h->szBroadMessage, "%s,%s,%s,%s", "1.1", mac, ip, version);
    return 0;
}

int cm_broadcast_socket_init(CMBroadcastHandle h)
{
    if (!h) return -1;

    h->iSockFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (h->iSockFD == -1) {
        return -2;
    }
    const int opt = 1;
    int nb = setsockopt(h->iSockFD, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt));
    if (nb == -1) {
        return -3;
    }

    return 0;
}

void *broadcast_thread(void *args)
{
    struct CMBroadcast *in = (struct CMBroadcast*)args;

    struct sockaddr_in addrto;
    memset(&addrto, 0, sizeof(addrto));
    addrto.sin_family = AF_INET;
    addrto.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    addrto.sin_port = htons(in->iPort);

    int len = sizeof(addrto);

    char *msg = in->szBroadMessage;
    while (!in->iExit) {
        char ip[32] = {0};
        cm_get_ip_dev("wlan0", ip);
        if (strcmp(ip, in->szBroadMessage) != 0) {
            cm_broadcast_set_info(in, in->szMac, ip, in->szVersion);
        }

        int lenmsg = strlen(in->szBroadMessage);

        int ret = sendto(in->iSockFD, msg, lenmsg, 0, (struct sockaddr*)&addrto, len);
        if (ret < 0) {

//            printf("send error of message [%d][%s]\n", ret, strerror(errno));
        } else {
//            printf("send success of message [%d]\n", ret);

        }
        sleep(30);
    }
    return NULL;
}

int cm_broadcast_start(CMBroadcastHandle h)
{
    int ret = cm_broadcast_socket_init(h);
    if (ret < 0) {
        return ret;
    }

    ret = pthread_create(&h->iThread, NULL, broadcast_thread, h);
    if (ret < 0) {
        return -11;
    }
    pthread_detach(h->iThread);

    return 0;
}

int cm_broadcast_close(CMBroadcastHandle *h)
{
    if (h) {
        pthread_cancel((*h)->iThread);
        close((*h)->iSockFD);
        free(*h);
    }
    *h = NULL;
    return 0;
}

#ifdef __MAIN_TEST__
int main() {
    int port = 12306;
    CMBroadcastHandle  h;
    int ret = cm_broadcast_init(&h, port);
    if (ret < 0) {
        printf("error init [%d]\n", ret);
        return -1;
    }
    cm_broadcast_set_info(h, "12345678", "128.123.123.123", "1.0.20");

    ret = cm_broadcast_start(h);
    if (ret < 0) {
        printf("start error [%d]\n", ret);
        return -2;
    }
    printf("start success\n");
    int count = 0;
    while (count ++ < 50) {
        sleep(1);
    }

    cm_broadcast_close(&h);
    return 0;
}
#endif
