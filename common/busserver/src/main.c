/*================================================================
*   Copyright (C) 2018 FREEDOM Ltd. All rights reserved.
*   
*   文件名称：main.c
*   创 建 者：chenjie.gu
*   创建日期：2018年05月23日
*   描    述：
*
================================================================*/

#include "busserver.h"
#include <stdio.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void cb(const char *topic, const char *topic_data, int data_len, void *user) {

    printf("%s: %s\n", topic, topic_data);
    
}

void ccb1(const char *topic, const char *topic_data, int data_len, void *user) {
    printf("cb1 %s: %s\n", topic, topic_data);
}

void ccb2(const char *topic, const char *topic_data, int data_len, void *user) {
    printf("cb2 %s: %s\n", topic, topic_data);
}

void *busserver_routine(void *user) {
    printf("listen busserver\n");
    busserver_run_ex(cb, NULL);
    // busserver_run("127.0.0.1:50001", cb, NULL);
    printf("listen busserver exit\n");
    return (void *)0;
}

void *busclient_demo1(void *user) {
    again:
    printf("connect to aispeech\n");
    int ret = busclient_connect("aispeech", "127.0.0.1:50001", ccb1, NULL);
    printf("connect to aispeech out [%d]\n", ret);
    sleep(1);
    goto again;
    return NULL;
}

void *busclient_demo2(void *user) {
    again:
    printf("connect to btvak\n");
    int ret = busclient_connect("btavk", "127.0.0.1:50001", ccb2, NULL);
    printf("connect to btavk out [%d]\n", ret);

        sleep(1);
        goto again;
    
    return NULL;
}

int main (int argc, char *args[]) {
    //
    //
    if (argc < 2) {
        printf("Usage: %s type(c,s) (client num)\n", args[0]);
        return -1;
    }
    
    int type = strcmp(args[1], "s") == 0 ? 1 : 0;
    int nu = 0;
    if (argc > 2) {
        nu = atoi(args[2]);
    }
    printf("Do with [%d] [%d]\n", type, nu);

    int ret = 0;
    if (type) {
        pthread_t tid;
        ret = pthread_create(&tid, NULL, busserver_routine, NULL);
        struct timeval tv = {10, 0};
        select(0, 0, 0, 0, &tv);
        busserver_send_msg_to("aispeech", "do.start", "nn");
        busserver_send_msg_to("btavk", "db.save", "nnnnn");
    }

    if (type == 0) {
        if (nu == 0) {
            pthread_t c1;
            pthread_create(&c1, NULL, busclient_demo1, NULL);
        }
        else if (nu == 1) {
            pthread_t c2;
            pthread_create(&c2, NULL, busclient_demo2, NULL);
        }
    }
    
    // busserver_send_msg("bus.event", "yyyyyyyyyyy");

    select(0, 0, 0, 0, 0);

    return 0;
}
