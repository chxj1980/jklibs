//
// Created by v on 16-1-4.
//

#ifndef KFCONFIG_CM_CONN_UDP_H
#define KFCONFIG_CM_CONN_UDP_H

typedef struct tagCMConnUDP *CMConnUDP;

int cm_conn_udp_create(CMConnUDP *conn, const char *addr, int port);

int cm_conn_udp_connect(CMConnUDP conn, int bCycle, int waitTime, int checktimes);

int cm_conn_udp_bind(CMConnUDP conn);

int cm_conn_udp_close(CMConnUDP *conn);

int cm_conn_udp_send(CMConnUDP conn, const char *data, long len);

int cm_conn_udp_recv(CMConnUDP conn, char *recvData, long *recvLen, int bCycle, int block);

#endif //KFCONFIG_CM_CONN_UDP_H
