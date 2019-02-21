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

int cm_http_parse_url(const char *origin, char *addr, int *port)
{
    // http://xxxx.xx.xx:8989
    //      p
    //        p
    // 
    char *p = strstr(origin, "/"); // http://
    if (p) {
        p += 2;
    } else p = (char*)origin;
    
    char *pn = strstr(p, ":"); // xxxx:80
    if (pn) {
        char portstr[8] = {0};
        strncpy(portstr, pn + 1, sizeof(portstr));
        *port = atoi(portstr);
    } else {
        pn = p + strlen(p);
        *port = 80;
    }
    strncpy(addr, p, pn - p);

    return 0;
}

int cm_http_parse_response_data(const char *origin, char *data, int maxdata)
{
	if (!origin || !data || maxdata <= 0) return -1;
	int datalen = 0;
    char *p = strstr(origin, "Content-Length: ");
	if (!p) {
		return -2;
	}
	char reslenstr[16] = {0};
	p += strlen("Content-Length: ");
	char *pe = strstr(p, "\r\n");
	if (pe) {
		int tlen = pe - p;
        strncpy(reslenstr, p, tlen < 16 ? tlen : 16);
	}
	datalen = atoi(reslenstr);
	// Just give one condition, we think response never so long, we needn't
	if (datalen > 10240) {
		return -3;
	}
	// skip \r\n\r\n
	// content start postion
	pe += 4;
	strncpy(data, pe, datalen > maxdata ? maxdata : datalen);

	return datalen;
}

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
    for (i=0;i<len;++i) {
        *(data+i) ^= *(mask+(i%4));
	}
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

//
// http upload file
//
#include <sys/types.h>
#include <sys/stat.h>

// POST /upload HTTP/1.1
// Host: 192.168.5.180:12306
// Referer: http://192.168.5.180:12306/upload
// Origin: http://192.168.5.180:12306
// Content-Type: multipart/form-data; boundary=----WebKitFormBoundarydrZIDvGZnDmlxVu9
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
// User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/11.0 Safari/605.1.15 Epiphany/605.1.15
// Upgrade-Insecure-Requests: 1
// Accept-Encoding: gzip, deflate
// Accept-Language: en-us, en;q=0.90
// Connection: Keep-Alive
// Content-Length: 4875

int cm_http_upload_init(CMHttpMessage *msg, const char *url, const char *path, const char *filename)
{
    struct stat st;
    int ret = stat(filename, &st);
    if (ret < 0) {
        return -1;
    }

    memset(msg, 0, sizeof (*msg));
    msg->szHeader.iVersionMajor = 1;
    msg->szHeader.iVersionMinor = 1;
    snprintf(msg->szHeader.szPath, sizeof(msg->szHeader.szPath), "%s", path);
    snprintf(msg->szHeader.szHost, sizeof(msg->szHeader.szHost), "%s", url);
    snprintf(msg->szHeader.szOrigin, sizeof(msg->szHeader.szOrigin), "http://%s", url);
    snprintf(msg->szHeader.szUploadFilename, sizeof(msg->szHeader.szUploadFilename), "%s", filename);
    strcpy(msg->szHeader.szUploadKeyname, "uploadfile");
    strcpy(msg->szHeader.szContentType, "application/octet-stream");

    char *p = strstr(path, "?");
    if (p) {
        int pl = p - path;
        char pp[64] = {0};
        strncpy(pp, path, pl);
        snprintf(msg->szHeader.szReferer, sizeof(msg->szHeader.szReferer), "http://%s/%s", url, pp);
    } else {
        snprintf(msg->szHeader.szReferer, sizeof(msg->szHeader.szReferer), "http://%s/%s", url, path);
    }
    char ramvalue[18] = {0};
    cm_random_with_num_char(ramvalue, 16);
    snprintf(msg->szHeader.szBoundary, sizeof(msg->szHeader.szBoundary), "----------------------%s", 
                                                    ramvalue);

    msg->szHeader.szContentLength = st.st_size;
    return 0;
}

int cm_http_upload_ext_add(CMHttpMessage *msg, const char *key, const char *value) 
{
    if (!msg || !key || !value) return -1;

    CMHttpExt *e = &msg->szHeader.szExt[msg->szHeader.iExtIndex++];
    strncpy(e->key, key, sizeof(e->key));
    strncpy(e->value, value, sizeof(e->value));

    return 0;
}

/*
Content-Disposition: form-data; name="uploadfile"; filename="include.mk"
Content-Type: text/x-makefile
Content-Type: application/octet-stream
Content-Type: image/jpeg
*/
static int cm_http_upload_gen_file(CMHttpMessage *msg, const char *keyname, const char *contenttype, char *result, int maxresult)
{
    int n = snprintf(result, maxresult, 
                    "--%s\r\n"
                    "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
                    "Content-Type: %s\r\n\r\n",
                    msg->szHeader.szBoundary,
                    keyname == NULL ? "uploadfile" : keyname,
                    msg->szHeader.szUploadFilename,
                    contenttype == NULL ? "application/octet-stream" : contenttype);

    return n;
}

static int cm_http_upload_gen_ext(CMHttpMessage *msg)
{
    int i = 0;
    msg->szHeader.iExtLength = 0;
    for (i = 0; i < msg->szHeader.iExtIndex; i++) {
        CMHttpExt *e = &msg->szHeader.szExt[i];
        snprintf(e->str, sizeof(e->str), 
                            "\r\n--%s\r\n"
                            "Content-Disposition: form-data; name=\"%s\"\r\n\r\n%s",
                            msg->szHeader.szBoundary,
                            e->key, e->value);
        msg->szHeader.iExtLength += strlen(e->str);
    }
    return 0;
}

int cm_http_upload_generate(CMHttpMessage *msg, char *result, int maxresult)
{
    if (!msg || !result || !maxresult) return -1;

    cm_http_upload_gen_file(msg, msg->szHeader.szUploadKeyname, msg->szHeader.szContentType, 
                        msg->szHeader.szFileHeader, sizeof(msg->szHeader.szFileHeader));

    cm_http_upload_gen_ext(msg);

    snprintf(msg->szHeader.szLastBoundary, sizeof(msg->szHeader.szLastBoundary), "\r\n--%s--\r\n", msg->szHeader.szBoundary);

    msg->szHeader.szContentLength += msg->szHeader.iExtLength + strlen(msg->szHeader.szFileHeader) + 
            strlen(msg->szHeader.szLastBoundary);

    int n = snprintf(result, maxresult, "POST /%s HTTP/%d.%d\r\n"
                    "Host: %s\r\n"
                    "Referer: %s\r\n"
                    "Origin: %s\r\n"
                    "Accept: */*\r\n"
                    "Content-Type: multipart/form-data; boundary=%s\r\n"
                    "User-Agent: cm\r\n"
                    "Content-Length: %d\r\n\r\n",
                    msg->szHeader.szPath, msg->szHeader.iVersionMajor, msg->szHeader.iVersionMinor,
                    msg->szHeader.szHost, msg->szHeader.szReferer,
                    msg->szHeader.szOrigin, msg->szHeader.szBoundary,
                    msg->szHeader.szContentLength);

    return n;
}


const char *cm_http_upload_fileheader(CMHttpMessage *msg, int *len)
{
    if (msg) {
        if (len) *len = strlen(msg->szHeader.szFileHeader);
        return msg->szHeader.szFileHeader;
    }
    return NULL;
}

const char* cm_http_upload_ext_string(CMHttpMessage *msg, int *len, int index)
{
    if (index < 0 || index >= msg->szHeader.iExtIndex) return NULL;
    if (msg) {
        if (len) *len = strlen(msg->szHeader.szExt[index].str);
        return msg->szHeader.szExt[index].str;
    }
    return NULL;
}

const char *cm_http_upload_lastboundary(CMHttpMessage *msg, int *len) 
{
    if (!msg) return NULL;
    if (len) *len = strlen(msg->szHeader.szLastBoundary);
    return msg->szHeader.szLastBoundary;
}

int cm_http_upload_ext_data(CMHttpMessage *msg, char *result, int maxlen)
{
    if (!msg || !result) return -1;
    char *p = result;
    int i;
    int remain = maxlen;
    for (i = 0; i < msg->szHeader.iExtIndex; i++) {
        CMHttpExt *e = &msg->szHeader.szExt[i];
        snprintf(p, remain, 
                            "\r\n--%s\r\n"
                            "Content-Disposition: form-data; name=\"%s\"\r\n\r\n%s",
                            msg->szHeader.szBoundary,
                            e->key, e->value);
        int plen = strlen(p);
        p += plen;
        remain -= plen;
    }
    strncpy(p, msg->szHeader.szLastBoundary, remain);

    return 0;
}
