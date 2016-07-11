//
// Created by v on 16-1-4.
//

#ifndef KFCONFIG_JK_CONN_UDP_H
#define KFCONFIG_JK_CONN_UDP_H

typedef struct tagJKConnUDP *JKConnUDP;

int jk_conn_udp_create(JKConnUDP *conn, const char *addr, int port);

int jk_conn_udp_connect(JKConnUDP conn, int bCycle, int waitTime, int checktimes);

int jk_conn_udp_close(JKConnUDP *conn);

int jk_conn_udp_send(JKConnUDP conn, const char *data, long len);

int jk_conn_udp_recv(JKConnUDP conn, char *recvData, long *recvLen, int bCycle, int block);

#endif //KFCONFIG_JK_CONN_UDP_H
