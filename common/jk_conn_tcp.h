/*
 *===================================================================
 *
 *          Name: jk_conn_tcp.h
 *        Create: 2015年09月25日 星期五 19时57分30秒
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: jmdvirus@roamter.com
 *
 *===================================================================
 */
#ifndef __JK_CONN_TCP
#define __JK_CONN_TCP

typedef struct tagJKConnTCP *JKConnTCP;

/*
 * Create a connect.
 */
int jk_conn_tcp_create(JKConnTCP *conn, const char *addr, int port);

/*
 * connect to server.
 * @bCycle: if need reconnect if failed.
 * @waitTime: wait some time reconnect when connect failed.
 * @checktimes: how many times need we retry when connect failed.
 */
int jk_conn_tcp_connect(JKConnTCP conn, int bCycle, int waitTime, int checktimes);

/*
 * close the conn.
 */
int jk_conn_tcp_close(JKConnTCP *conn);

/*
 * Send data out.
 */
int jk_conn_tcp_send(JKConnTCP conn, const char *data, long len);

/*
 * recv data from connected data.
 * @bCycle: if connected until nothing.
 */
int jk_conn_tcp_recv(JKConnTCP conn, char *recvData, long *recvLen, int bCycle);

#endif

/*=============== End of file: jk_conn_tcp.h =====================*/
