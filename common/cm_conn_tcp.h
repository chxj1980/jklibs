/*
 *===================================================================
 *
 *          Name: cm_conn_tcp.h
 *        Create: 2015年09月25日 星期五 19时57分30秒
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: jmdvirus
 *
 *===================================================================
 */
#ifndef __CM_CONN_TCP
#define __CM_CONN_TCP

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagCMConnTCP *CMConnTCP;

/*
 * Create a connect.
 */
int cm_conn_tcp_create(CMConnTCP *conn, const char *addr, int port);

/*
 * New method will never use connect method.
 * It will connect when created.
 */
int cm_conn_tcp_create_new(CMConnTCP *conn, const char *addr, int port);
int cm_conn_tcp_create_new_timeout(CMConnTCP *conn, const char *addr, int port, int timeout);

/*
 * connect to server.
 * @bCycle: if need reconnect if failed.
 * @waitTime: wait some time reconnect when connect failed.
 * @checktimes: how many times need we retry when connect failed.
 */
int cm_conn_tcp_connect(CMConnTCP conn, int bCycle, int waitTime, int checktimes);

/*
 * close the conn.
 */
int cm_conn_tcp_close(CMConnTCP *conn);

/*
 * Send data out.
 */
int cm_conn_tcp_send(CMConnTCP conn, const char *data, long len);

/*
 * recv data from connected data.
 * @bCycle: if connected until nothing.
 */
int cm_conn_tcp_recv(CMConnTCP conn, char *recvData, long *recvLen, int bCycle);

/**
 * Caller do read, no more process
 * < 0 fail
 * > 0 len read
 * anytime, will set result to *recvlen
 * @param conn
 * @param recvData
 * @param recvlen
 * @return
 */
int cm_conn_tcp_recv_simple(CMConnTCP conn, char *recvData, int *recvlen);


#ifdef __cplusplus
}
#endif

#endif

/*=============== End of file: cm_conn_tcp.h =====================*/
