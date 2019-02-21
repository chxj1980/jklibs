//
// Created by v on 18-6-5.
//

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "cm_conn_ws.h"
#include "cm_http.h"
#include "cm_utils.h"
#include "cm_logprint.h"

//
// Web socket connection
//

int cm_websocket_request_read(CMConnTCP conn, CMWSFrameHeader* head)
{
    char tmpdata[32] = {0};
    char *p = tmpdata;

    int len = 1;
    /*read fin and op code*/
    int n = cm_conn_tcp_recv_simple(conn, tmpdata, &len);
    if (n <= 0) {
        return n == 0 ? 0 : -1;
    }

    head->cFin = (p[0] & 0x80) == 0x80;
    head->cOpcode = p[0] & 0x0F;

    n = cm_conn_tcp_recv_simple(conn, tmpdata, &len);
    if (n <= 0) {
        return n == 0 ? 0 : -2;
    }
    head->cMask = (p[0] & 0x80) == 0x80;
    head->ullPayloadLength = p[0] & 0x7f;

    if (head->ullPayloadLength == 126)
    {
        char extern_len[2];
        len = 2;
        n = cm_conn_tcp_recv_simple(conn, extern_len, &len);
        if (n <= 0)
        {
            return n == 0 ? 0 : -3;
        }
        head->ullPayloadLength = (extern_len[0]&0xFF) << 8 | (extern_len[1]&0xFF);
    }
    else if (head->ullPayloadLength == 127)
    {
        char extern_len[8];
        len = 8;
        n = cm_conn_tcp_recv_simple(conn, extern_len, &len);
        if (n <= 0)
        {
            return n == 0 ? 0 : -4;
        }
        cm_ws_invert_string(extern_len,8);
        memcpy(&(head->ullPayloadLength),extern_len,8);
    }

    /*read masking-key*/
    if (head->cMask) {
        len = 4;
        n = cm_conn_tcp_recv_simple(conn, tmpdata, &len);
        if (n <= 0) {
            return n == 0 ? 0 : -5;
        }
    }

    return n;
}

int cm_websocket_request_recv(CMConnTCP conn, char *result, size_t *lenresult)
{
    if (!conn) return -1;

    int errvalue = 0;
    CMWSFrameHeader head;
    int timeover = 8000;
    unsigned long long lasttime = cm_gettime_milli();
    while(1) {

        unsigned long long now = cm_gettime_milli();
        if (now - lasttime > (unsigned long long)timeover) {
            cmdebug("websocket recv timeout [%d]ms\n", timeover);
            errvalue = -4;
            break;
        }

        int n = cm_websocket_request_read(conn, &head);
        int readlen = (int)head.ullPayloadLength;
        if (n == 0) {
            usleep(20000);
            continue;
        }
        if (n < 0) {
            cmerror("recv data header errror [%d]\n", n);
            errvalue = -2;
            break;
        } else {
            cmdebug("Websocket Need read content len [%d]\n", readlen);
            int data_pos = 0;
            int needread = readlen;
            int remainread = readlen;
            while (1) {
                if (needread <= 0) break;
                n = cm_conn_tcp_recv_simple(conn, result+data_pos, &needread);
                if (n < 0) {
                    cmerror("recv data body error [%d]\n", n);
                    return -5;
                }
                data_pos += needread;
                remainread = readlen - data_pos;
                needread = remainread;
            }
            cmdebug("DEBUG: Websocket read out content of len [%d]\n", data_pos);
            *lenresult = data_pos;
            break;
        }
    }

    return errvalue;
}

int cm_websocket_request_recv_ex(CMConnTCP conn, char **result, size_t *lenresult)
{
    if (!conn) return -1;

    int errvalue = 0;
    CMWSFrameHeader head;
    int timeover = 8000;
    unsigned long long lasttime = cm_gettime_milli();
    size_t all_readlen = 0;
    while(1) {

#if 0
        unsigned long long now = cm_gettime_milli();
        if (now - lasttime > (unsigned long long)timeover) {
            cmdebug("websocket recv timeout [%d]ms\n", timeover);
            errvalue = -4;
            break;
        }
#endif

        int n = cm_websocket_request_read(conn, &head);
        int readlen = (int)head.ullPayloadLength;
        if (n == 0) {
            break;
#if 0
            usleep(20000);
            continue;
#endif
        }
        if (n < 0) {
            cmerror("recv data header errror [%d] [%s]\n", n, strerror(errno));
            errvalue = -2;
            break;
        } else {
            cmdebug("Websocket Need read content len [%d]\n", readlen);

            if (*result == NULL || *lenresult < (unsigned long)readlen + all_readlen) {
                *result = (char *) cm_mem_realloc(*result, all_readlen + readlen + 1);
            }

            int data_pos = 0;
            int needread = readlen;
            int remainread = readlen;
            while (1) {
                if (needread <= 0) break;
                n = cm_conn_tcp_recv_simple(conn, (*result)+all_readlen + data_pos, &needread);
                if (n == -1 || n == -12) {
                    cmerror("recv data body error [%d]\n", n);
                    return -5;
                } else if (n <= 0) {
                    continue;
                }
                data_pos += needread;
                remainread = readlen - data_pos;
                needread = remainread;
            }
            cmdebug("DEBUG: Websocket read out content of len [%d] have length [%d] all result length [%d]\n", data_pos,
                    all_readlen, strlen(*result));
            all_readlen += data_pos;
            if (head.cFin) {
                *lenresult = all_readlen;
                errvalue = *lenresult;
                break;
            }
        }
    }

    return errvalue;
}


/**
 * This function just send data and wait response, ignore what protocol
 * TODO: this function should change better
 * */
int cm_websocket_request_send_wait_res(CMConnTCP conn, const char *msg, size_t len, char *res, size_t *lenres)
{
    if (*lenres == 0) {
        cmerror("Give a wrong length of place to holder data\n");
        return -5;
    }

    int n = cm_conn_tcp_send(conn, msg, len);
    if (n <= 0) {
        cmerror("Request send data failed ret [%d]\n", n);
        return -11;
    }

    cmdebug("Request send data success of len [%d], start recv\n", n);

    char recvdata[10240] = {0};
    long recvlen = 10240;
    int check_times = 5000;
    n = 0;
    unsigned long long start = cm_gettime_milli();
    int startlen = 0;
    while (1) {
        unsigned long long now = cm_gettime_milli();
        if (now - start > (unsigned long long)check_times) break;

        n = cm_conn_tcp_recv(conn, recvdata, &recvlen, 0);
        // TODO: It's a bad idea doing like this, need change a style to read.
        if (recvlen > 0) {
            if ((unsigned )startlen + (unsigned long)recvlen < *lenres) {
                memcpy(res + startlen, recvdata, (unsigned long)recvlen > *lenres ? *lenres : recvlen);
                startlen += recvlen;
            }
            if (recvlen < 10) {
                start = now;
                continue;
            }
            break;
        }
    }
    if (n < 0) {
        cmerror("Websocket recv data failed\n");
        return -12;
    }
    cmdebug("Request send wait res recv len [%d]\n", startlen);

    *lenres = startlen;
    return 0;
}



/**
 * Generate websocket header, concatenat msg , then send, give response to @res
 * @waitres: if need wait response
 * */
int cm_websocket_request(CMConnTCP conn, int waitres, const char *msg, size_t len, char *res, size_t *lenres)
{
    // Generate websocket header
    CMWSFrameHeader header;
    memset(&header, 0, sizeof(CMWSFrameHeader));
    CMWSFrameHeader resheader;
    memset(&resheader, 0, sizeof(CMWSFrameHeader));

    char *head_str = NULL;
    int head_str_len = 0;

    // Set header
    header.ullPayloadLength = len;
    header.cMask = 1;
    cm_random_with_num_char(header.cMasking_key, 4);
    cm_ws_generate_frame_head(&header, &head_str, &head_str_len);

    cmdebug("websocket request generated header length is [%d] payload length is [%d]\n", head_str_len, len);

    // Send with data
    int sendlen = head_str_len + len;
    char *senddata = (char*)malloc(sendlen);
    memcpy(senddata, head_str, head_str_len);

    cmdebug("websocket request send data len=%d, \n[%s]\n", len, msg);
    if (header.cMask) {
        cm_ws_umask((char*)msg, len, header.cMasking_key);
    }

    memcpy(senddata+head_str_len, msg, len);

    int n = cm_conn_tcp_send(conn, (char*)senddata, sendlen);
    if (n <= 0) {
        cmerror("websocket Request send data failed ret [%d]\n", n);
        goto errout;
    }

    cmdebug("websocket Request send data success of len [%d], start recv if need [%d]\n", n, waitres);
    if (waitres) {

        int nn = cm_websocket_request_recv(conn, res, lenres);

        if (nn < 0) {
            cmerror("net request send wait res failed ret [%d]\n", nn);
            goto errout;
        }
    }

    errout:
    if (senddata) free(senddata);
    if (head_str) free(head_str);

    return n;
}

#define NETR_DEBUG_PREFIX "WS handleshake "
int cm_websocket_handshake(CMConnTCP conn, const char *url, const char *addr, int port)
{
    if (!conn) return -1;

    CMHttpHeader  httpheader;

    cm_http_ws_header_init(&httpheader, url, addr, port);

    char allreq[1024] = {0};
    int alllen = cm_http_ws_generate_str(&httpheader, allreq, sizeof(allreq));

    cmdebug("%s send data [%d]\n[%s]\n", NETR_DEBUG_PREFIX, alllen, allreq);

    char recv[10240] = {0};
    size_t recvlen = sizeof(recv);
    int ret = cm_websocket_request_send_wait_res(conn, allreq, alllen, recv, &recvlen);

    cmdebug("%s recv data done. len [%d] data[%s]\n", NETR_DEBUG_PREFIX, recvlen, recv);

    ret = cm_http_ws_parser_result_code(recv, recvlen);
    if (ret <= 0) {
        cmerror("%s response fail of handshake [%d]\n", NETR_DEBUG_PREFIX, ret);
        return -4;
    }

    return 0;
}
