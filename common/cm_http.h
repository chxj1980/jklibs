/**
 * Http parse and generate
 * Create: 20180515
 * Author: jmdvirus
 */

#ifndef __CM_HTTP_H
#define __CM_HTTP_H

#include <stdio.h>

// #define LOCAL_DEBUG
// define server information
#ifdef LOCAL_DEBUG
#define JDH_SERVER_SERVER      "127.0.0.1"
#define JDH_SERVER_PORT        "8080"
#else
#define JDH_SERVER_SERVER      "106.14.61.92"
#define JDH_SERVER_PORT        "8081"
#endif

#define JDH_SERVER_WEBSOCKET   "ws://" JDH_SERVER_SERVER ":" JDH_SERVER_PORT "/message/ws"
#define JDH_SERVER_HTTP_URL         "http://" JDH_SERVER_SERVER ":" JDH_SERVER_PORT "/message/ws"
#define JDH_SERVER_HOST             JDH_SERVER_SERVER ":" JDH_SERVER_PORT
#define JDH_SERVER_ORIGIN           "http://" JDH_SERVER_SERVER ":" JDH_SERVER_PORT

#define JDH_SERVER_HTTP_REQUEST     "/message/ws"

#define JDH_USER_AGENT          "jdh-client-"JDH_VERSION

typedef struct {
    char key[64];
    char value[128];
    char str[1024];
} CMHttpExt;

typedef struct {
    char                  szMethod[8]; // GET/POST/PUT/...
    int                   iVersionMajor;
    int                   iVersionMinor;
    char                  szURL[256]; 
    char                  szUserAgent[512]; 
    char                  szContentType[32];
    char                  szServer[128];
    char                  szAcceptRanges[32];
    int                   szContentLength;
    char                  szConnection[32];

    char                  szFileHeader[2048];
    char                  szReferer[512];
    char                  szPath[64];
    char                  szBoundary[64];
    char                  szExtString[2048];
    char                  szLastBoundary[128];

    char                  szAccept[64];
    char                  szAcceptLanguage[64];
    char                  szHost[128];
    char                  szOrigin[128];

    char                  szUploadKeyname[128];
    char                  szUploadFilename[1024];
    int                   iExtIndex;
    CMHttpExt             szExt[16]; // more with 15 ext
    int                   iExtLength;

    // for websocket
    int                   iUpgrade; // 1 means enable websocket
    char                  szWSUpgrade[16];
    char                  szWSKey[32];
    char                  szWSProtocol[32];
    int                   iWSVersion;
    // for websocket response
    char                  szWSAccept[32];
} CMHttpHeader;

typedef struct {
    int                     iResponse; // If response
    CMHttpHeader            szHeader;
    char                   *pData;
} CMHttpMessage;


// parse out addr and port from url
int cm_http_parse_url(const char *origin, char *addr, int *port);

// parse origin to data 
// http response 
// return Content-Length value
// data save content
int cm_http_parse_response_data(const char *origin, char *data, int maxdata);

/*
GET /chat HTTP/1.1
        Host: server.example.com
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
        Origin: http://example.com
        Sec-WebSocket-Protocol: chat, superchat
        Sec-WebSocket-Version: 13
*/
/*
HTTP/1.1 101 Switching Protocols
        Upgrade: websocket
        Connection: Upgrade
        Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
        Sec-WebSocket-Protocol: chat
*/

/**
 * Init header struct, and set some value.
 * Others add from struct
 */
int cm_http_generate_header(CMHttpHeader *header, const char *method, const char *url);

/**
 * Init msg and set data and len
 * as header has set before.
 */
int cm_http_generate_msg(CMHttpMessage *msg, CMHttpHeader *header, const char * data, int len);

/**
 * Generate string to @result as @msg
 * Basic http header
 */
int cm_http_generate_str(CMHttpMessage *msg, char *result, int maxresult);

//
// Websocket 
//

/**
 * Websocket relative operation (message operation)
 * */

typedef struct {
    char cFin;
    char cOpcode;
    char cMask;
    unsigned long long ullPayloadLength;
    char cMasking_key[4];
    char *pData;

    int  iHeaderLength;
} CMWSFrameHeader;

/**
 * invert string
 * 
 * */
void cm_ws_invert_string(char *str,int len);

/**
 * umask key data to normal data
 * */
void cm_ws_umask(char *data,int len,char *mask);

/**
 * Generate frameheader to *data
 * Need free data
 * */
int cm_ws_generate_frame_head(CMWSFrameHeader *header, char **data, int *len);

/**
 * Parse out @data to @head
 * */
int cm_ws_parse_frame_header(const char *data, int len, CMWSFrameHeader* head);

//
// For websocket handshake
//
/**
 * Use this function int websocket header
 * Any args you want to change, just do after call this init.
 * @url: like /chat, /message/ws, ...
 */
int cm_http_ws_header_init(CMHttpHeader *header, const char *url, const char *addr, int port);

/**
 * Generate string of websocket header to @result
 * Websocket has no content.
 * */
int cm_http_ws_generate_str(CMHttpHeader *header, char *result, int maxresult);

/**
 * Parse out result code of websocket
 * return: 1 - success (response of 301)
 * -1 - invalid data
 * 0 - success, but result number is not 301, handshake fail
 * */
// TODO: should parse all to CMHttpMessage as Response.
int cm_http_ws_parser_result_code(const char *str, size_t len);

//
// http upload file
//
// How to use this interface:
//   1. call init
//   2. call ext add if you want add some external args
//   3. call generate to generate base messages exclude file binary data.
//   4. send the data have generated.
//   5. call ext_string get ext string
//   6. send the message.
// DON'T change anything when you have call generated.
// You can change keyname of uploadfile, and contenttype before call generate
//
// You should set header to CMHttpHeader
// http://url/path
// You should add external param to path like path = (path?value=xxx)
// path conclude value=xxx
int cm_http_upload_init(CMHttpMessage *msg, const char *url, const char *path,  const char *filename);

/**
 * Add ext args post with file
 * This should call before generate and after init
 */
int cm_http_upload_ext_add(CMHttpMessage *msg, const char *key, const char *value);

/**
 * Generate http header string as init set.
 */
int cm_http_upload_generate(CMHttpMessage *msg, char *result, int maxresult);

/**
 * Get fileheader string
 **/
const char *cm_http_upload_fileheader(CMHttpMessage *msg, int *len);

//
// You can split ext string and last boundary to send, use ext_string and _lastboundary
// You can send all ext string and last boundary with _ext_data also.
/**
 * return string with all ext string combined.
 */
const char* cm_http_upload_ext_string(CMHttpMessage *msg, int *len, int index);

/**
 * Generate Last boundary
 * */
const char *cm_http_upload_lastboundary(CMHttpMessage *msg, int *len);

/**
 * All data for last, conclude ext_string and last boundary
 * */
int cm_http_upload_ext_data(CMHttpMessage *msg, char *result, int maxlen);

#endif // __CM_HTTP_H
