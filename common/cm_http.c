/**
 * http parse and generate
 * Create: 20180515
 * Author: jmdvirus
 */

#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "cm_http.h"
#include "cm_utils.h"
#include "cm_base64.h"

int cm_http_generate_header(CMHttpHeader *header, const char *method, const char *url)
{
     memset(header, 0, sizeof(CMHttpHeader));

     snprintf(header->szMethod, sizeof(header->szMethod), "%s", method);
     header->iVersionMajor = 1;
     header->iVersionMinor = 1;

    //snprintf(header->szHost, sizeof(header->szHost), "%s", JDH_SERVER_HOST);
    snprintf(header->szURL, sizeof(header->szURL), "%s", url);
    //snprintf(header->szUserAgent, sizeof(header->szUserAgent), "%s", JDH_USER_AGENT);
    snprintf(header->szConnection, sizeof(header->szConnection), "%s", "close");

    return 0;
}

int cm_http_generate_msg(CMHttpMessage *msg, CMHttpHeader *header, const char * data, int len)
{
    memset(msg, 0, sizeof(CMHttpMessage));
    msg->iResponse = 0;
    header->szContentLength = len;
    memcpy(&msg->szHeader, header, sizeof(CMHttpHeader));
    msg->pData = (char*)data;

    return 0;
}

int cm_http_generate_str(CMHttpMessage *msg, char *result, int maxresult)
{
    int n = snprintf(result, maxresult, "%s %s HTTP/%d.%d\r\n"
                                "Host: %s\r\n"
                                "Connection: %s\r\n"
                                "User-Agent: %s\r\n"
                                "Accept: text/plain\r\n"
                                "Accept-Encoding: gzip,deflate\r\n"
                                "Accept-Language: zh-CN,zh;q=0.8,nb;q=0.6\r\n"
                                "Content-Length: %d\r\n"
                                "\r\n%s",
                                msg->szHeader.szMethod, msg->szHeader.szURL, 
                                msg->szHeader.iVersionMajor, msg->szHeader.iVersionMinor,
                                msg->szHeader.szHost, 
                                msg->szHeader.szConnection,
                                msg->szHeader.szUserAgent,
                                msg->szHeader.szContentLength,
                                msg->pData);

    return n;
}

//
// Websocket
//

void cm_ws_invert_string(char *str,int len)
{
    int i; char temp;
    for (i=0;i<len/2;++i)
    {
        temp = *(str+i);
        *(str+i) = *(str+len-i-1);
        *(str+len-i-1) = temp;
    }
}

void cm_ws_umask(char *data,int len,char *mask)
{
    int i;
    for (i=0;i<len;++i)
        *(data+i) ^= *(mask+(i%4));
}

void cm_ws_umask_string(char *data, int len, char *umask)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        data[i] = data[i] ^ umask[i % 4];
    }
}

int cm_ws_generate_frame_head(CMWSFrameHeader *header, char **data, int *len)
{
    char *response_head;
    int head_length = 0;
    if(header->ullPayloadLength < 126)
    {
        response_head = (char*)malloc(2);
        response_head[0] = 0x81;
        response_head[1] = header->ullPayloadLength;
        head_length = 2;
    }
    else if (header->ullPayloadLength<0xFFFF)
    {
        response_head = (char*)malloc(4);
        response_head[0] = 0x81;
        response_head[1] = 126;
        response_head[2] = (header->ullPayloadLength >> 8 & 0xFF);
        response_head[3] = (header->ullPayloadLength & 0xFF);
        head_length = 4;
    }
    else
    {
        response_head = (char*)malloc(12);
        response_head[0] = 0x81;
        response_head[1] = 127;
        memcpy(response_head+2, &header->ullPayloadLength,sizeof(unsigned long long));
        cm_ws_invert_string(response_head+2,sizeof(unsigned long long));
        head_length = 12;
    }
    if (header->cMask) {
        response_head[1] |= 0x80;
        response_head = (char*)realloc(response_head, head_length + 4);
        memcpy(response_head + head_length, header->cMasking_key, 4);
        head_length += 4;
    }
    *data = response_head;
    *len = head_length;
    return 0;
}

int cm_ws_parse_frame_header(const char *data, int len, CMWSFrameHeader* head)
{
    memset(head, 0, sizeof(CMWSFrameHeader));
    char *p = (char*)data;

    head->cFin = (p[0] & 0x80) == 0x80;
    head->cOpcode = p[0] & 0x0f;

    head->cMask = (p[1] & 0x80) == 0x80;
    head->ullPayloadLength = (p[1] & 0x7f);

    int head_len = 2;
    
    if (head->ullPayloadLength == 126) {
        // p [2] p[3]
        head->ullPayloadLength = (p[2] & 0xff) << 8 | (p[3] & 0xff);
        head_len += 2;
    } else if (head->ullPayloadLength == 127) {
        // p[4]p[5]p[6]p[7]...p[11]
        char extern_len[8] = {0};
        memcpy(extern_len, p+4, 8);
        cm_ws_invert_string(extern_len, 8);
        memcpy(&(head->ullPayloadLength), extern_len, 8);
        head_len += 8;
    }

    // p[11]
    if (head->cMask) {
        memcpy(head->cMasking_key, p+11, 4);
        head_len += 4;
    }
    head->pData = (char*)(data + head_len);
    head->iHeaderLength = head_len;

    return 0;
}

int cm_http_ws_generate_key(char *key, size_t *len)
{
    char gen[18] = {0};
    cm_random_with_num_char(gen, 16);
    cm_base64_encode((unsigned char*)key, len, (unsigned char*)gen, 16);
    return 0;
}

//
int cm_http_ws_header_init(CMHttpHeader *header, const char *url, const char *addr, int port)
{
    memset(header, 0, sizeof(CMHttpHeader));
    header->iVersionMajor = 1;
    header->iVersionMinor = 1;

    snprintf(header->szMethod, sizeof(header->szMethod), "%s", "GET");
    snprintf(header->szURL, sizeof(header->szURL), "%s", url);
    snprintf(header->szHost, sizeof(header->szHost), "%s:%d", addr, port);
    snprintf(header->szOrigin, sizeof(header->szOrigin), "http://%s", addr);
    size_t keylen = sizeof(header->szWSKey);
    cm_http_ws_generate_key(header->szWSKey, &keylen);

    // snprintf(header->szWSKey, sizeof(header->szWSKey), "%s", "bFhlIHNh3dBsZSBub25jZQ==");
    snprintf(header->szConnection, sizeof(header->szConnection), "%s", "Upgrade");
    snprintf(header->szWSProtocol, sizeof(header->szWSProtocol), "%s", "chat, superchat");
    snprintf(header->szWSUpgrade, sizeof(header->szWSUpgrade), "%s", "websocket");
    snprintf(header->szUserAgent, sizeof(header->szUserAgent), "%s", "yt-client");
    header->iWSVersion = 13;
    header->iUpgrade = 1;

    return 0;
}

int cm_http_ws_generate_str(CMHttpHeader *header, char *result, int maxresult)
{
    int n = 0;
    if (header->iUpgrade) {

        n = snprintf(result, maxresult, "%s %s HTTP/%d.%d\r\n"
                                "Host: %s\r\n"
                                "Upgrade: %s\r\n"
                                "Connection: %s\r\n"
                                "Sec-WebSocket-Key: %s\r\n"
                                "Origin: %s\r\n"
                                "Sec-WebSocket-Version: %d\r\n"
                                "UserAgent: %s\r\n"
                                "\r\n",
                                header->szMethod, header->szURL,
                                header->iVersionMajor, header->iVersionMinor,
                                header->szHost, 
                                header->szWSUpgrade,
                                header->szConnection,
                                header->szWSKey,
                                header->szOrigin,
                                header->iWSVersion,
                                header->szUserAgent);

    }
    return n;
}

int cm_http_ws_parser_result_code(const char *str, size_t len)
{
    char version[32] = {0};
    int  resultcode = 0;
    char other[32] = {0};
    int n = sscanf(str, "%s %d %s\r\n", version, &resultcode, other);
    if (n < 2) {
        return -1;
    }
    if (resultcode == 101) {
        // TODO: should check if  accept is valid.
        return 1;
    }

    return 0;
}



