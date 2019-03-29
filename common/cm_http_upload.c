#include <stdio.h>  

#include <string.h>    
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <malloc.h>
#include <unistd.h>

#ifdef USE_LIBCURL
#include <curl/curl.h>
#endif

#include "cm_http.h"
#include "cm_http_upload.h"

/*
 * @url: ip:port
 */
int cm_http_send_file(char *filename, char *url, int subid)
{
#ifdef USE_LIBCURL
  CURL *curl;  
  CURLcode res;  
  
  struct curl_httppost *formpost=NULL;  
  struct curl_httppost *lastptr=NULL;  
  struct curl_slist *headerlist=NULL;  
  static const char buf[] = "Expect:";  

  int ret = 0;
  // only linux
  char *p = rindex(filename, '/');
  if (!p) p = filename;
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "logFile",
               CURLFORM_FILE, filename,  
               CURLFORM_END);

  char subidstr[8] = {0};
  sprintf(subidstr, "%d", subid);
  if (subid != 0) {
      curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "subId",
                 CURLFORM_COPYCONTENTS, subidstr,
                 CURLFORM_END);
  }
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "logFile",
               CURLFORM_COPYCONTENTS, p,  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "submit",  
               CURLFORM_COPYCONTENTS, "Submit",  
               CURLFORM_END);

  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
  headerlist = curl_slist_append(headerlist, buf);  
  if(curl) {  
    /* what URL that receives this POST */  
	  char fullurl[512] = {0};
	  sprintf(fullurl, "http://%s", url);
     curl_easy_setopt(curl, CURLOPT_URL, fullurl);  
    //if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )  
      /* only disable 100-continue header if explicitly requested */  
     // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
  
    /* Perform the request, res will get the return code */  
    res = curl_easy_perform(curl);  
    /* Check for errors */  
    if(res != CURLE_OK) {
      ret = -1;
    }
  
    /* always cleanup */  
    curl_easy_cleanup(curl);  
  
    /* then cleanup the formpost chain */  
    curl_formfree(formpost);  
    /* free slist */  
    curl_slist_free_all (headerlist);  
  } 

  return ret;
#else
  return -1;
#endif
}

int cm_http_send_file_two(char *filename, char *url, const char* imei)
{
#ifdef USE_LIBCURL
  CURL *curl;  
  CURLcode res;  
  
  struct curl_httppost *formpost=NULL;  
  struct curl_httppost *lastptr=NULL;  
  struct curl_slist *headerlist=NULL;  
  static const char buf[] = "Expect:";  

  int ret = 0;
  // only linux
  char *p = rindex(filename, '/');
  if (!p) p = filename;
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "logFile",
               CURLFORM_FILE, filename,  
               CURLFORM_END);

  curl_formadd(&formpost,
              &lastptr,
              CURLFORM_COPYNAME, "imei",
              CURLFORM_COPYCONTENTS, imei,
              CURLFORM_END);
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "logFile",
               CURLFORM_COPYCONTENTS, p,  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "submit",  
               CURLFORM_COPYCONTENTS, "Submit",  
               CURLFORM_END);

  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
  headerlist = curl_slist_append(headerlist, buf);  
  if(curl) {  
    /* what URL that receives this POST */  
	  char fullurl[512] = {0};
	  sprintf(fullurl, "http://%s", url);
     curl_easy_setopt(curl, CURLOPT_URL, fullurl);  
    //if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )  
      /* only disable 100-continue header if explicitly requested */  
     // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
  
    /* Perform the request, res will get the return code */  
    res = curl_easy_perform(curl);  
    /* Check for errors */  
    if(res != CURLE_OK) {
      ret = -1;
    }
  
    /* always cleanup */  
    curl_easy_cleanup(curl);  
  
    /* then cleanup the formpost chain */  
    curl_formfree(formpost);  
    /* free slist */  
    curl_slist_free_all (headerlist);  
  } 

  return ret;
#else
  return -1;
#endif
}
#define Debug
#ifdef Debug
#define LOGD(fmt, args...) \
    printf(fmt, ##args);
#else
#define LOGD(fmt, ...)
#endif

int cm_http_send_base(CMHttpUpload *up, const char *url, const char *path, const char *filename)
{
    if (!up || !url || !path || !filename) return -1;

    char addr[64] = {0};
    int port = 80;
    cm_http_parse_url(url, addr, &port);
	LOGD("connect to [%s:%d]\n", addr, port);
#if 0
    int ret = cm_conn_tcp_create(&up->szConn, addr, port);
    if (ret < 0) {
		LOGD("tcp create fail [%d]\n", ret);
        return -2;
    }
    ret = cm_conn_tcp_connect(up->szConn, 1, 1, 2);
    if (ret < 0) {
		LOGD("tcp connect fail [%d]\n", ret);
        cm_conn_tcp_close(&up->szConn);
        return -3;
    }
#else
	int ret = cm_conn_tcp_create_new(&up->szConn, addr, port);
	if (ret < 0) {
		LOGD("tcpp connect fail [%d]\n", ret);
		return -2;
	}
#endif

    ret = cm_http_upload_init(&up->szMsg, url, path, filename);
    if (ret < 0) {
		LOGD("upload init fail [%d]\n", ret);
        cm_conn_tcp_close(&up->szConn);
        return -4;
    }
	strcpy(up->szMsg.szHeader.szUploadKeyname, up->szUploadFileKeyName);
	if (strstr(filename, ".jpg")) {
	    strcpy(up->szMsg.szHeader.szContentType, "image/jpg");
	} else if (strstr(filename, ".png")) {
	    strcpy(up->szMsg.szHeader.szContentType, "image/png");
	}

    int i = 0;
    for (i = 0; i < 16; i++) {
        CMHttpExt *e = &up->szExt[i];
        if (e->key[0] != '\0') {
            cm_http_upload_ext_add(&up->szMsg, e->key, e->value);
        }
    }

    char result[10240] = {0};
    int maxresult = 10240;

    // generate header
    cm_http_upload_generate(&up->szMsg, result, maxresult);
    // send header and file content before
    cm_conn_tcp_send_simple(up->szConn, result, strlen(result));
    LOGD("--------------- [%s]\n", result);
    
    // get file header
    int fileheaderlen = 0;
    const char *fileheader = cm_http_upload_fileheader(&up->szMsg, &fileheaderlen);
    cm_conn_tcp_send_simple(up->szConn, fileheader, fileheaderlen);
    LOGD("-------------- send file header [%s]\n", fileheader);

    // send file content
    struct stat st;
    stat(filename, &st);
    unsigned int all = st.st_size;
   
    unsigned int remain = all;
    int section = 10240;
    char data[10240] = {0};
    FILE *f = fopen(filename, "r");
    while (remain >= 0) {
        int n = fread(data, 1, section, f);
        LOGD("--------- read out len [%d] from file [%s] [%s]\n", n, filename, strerror(errno));
        if (n <= 0) break;
        if (n > 0) {
            cm_conn_tcp_send(up->szConn, data, n);
            LOGD("------------- send out len [%d]\n", n);
        }
        remain -= n;
    }
    fclose(f);

    // send extern string
    char lastdata[4096] = {0};
    cm_http_upload_ext_data(&up->szMsg, lastdata, 4096);
    cm_conn_tcp_send_simple(up->szConn, lastdata, strlen(lastdata));
    LOGD("send last boundary [%s]\n", lastdata);

    int rcount = 10;
    int rlen = sizeof(up->szResponse);
    while (rcount--) {
        int n = cm_conn_tcp_recv_simple(up->szConn, up->szResponse, &rlen);
        if (n > 0) break;
        usleep(500000);
    }
    LOGD("Response is [%s]\n", up->szResponse);
    ret = cm_http_parse_response_data(up->szResponse, up->szResContent, sizeof(up->szResContent));
	if (ret < 0) {
		LOGD("Error parse response [%d]\n", ret);
	} else {
		LOGD("Resposne content is [%s]\n", up->szResContent);
	}

    cm_conn_tcp_close(&up->szConn);

    return 0;
}

#ifdef MAIN_TEST
int main(int argc, char *argv[])  
{  
  
#ifdef USE_LIBCURL
  curl_global_init(CURL_GLOBAL_ALL);  
#endif
  
#if USE_LIBCURL
#if 0
  //char *url = "106.14.61.92:8081/mbm-web/logs/uploadLog.html";
  char *url = "192.168.5.180:12306/upload";
  int ret = cm_http_send_file(argv[1], url, 129);
  printf("---------- %d\n", ret);
#else
  char *url2 = "www.jicuobi.com/index.php/Home/Upload/scanpic.html";
  int ret = cm_http_send_file_two(argv[1], url2, "test123");
  printf("---------- %d\n", ret);
#endif
#else
    char *url = "www.jicuobi.com";
    CMHttpUpload up;
    memset(&up, 0, sizeof(CMHttpUpload));
	strcpy(up.szUploadFileKeyName, "img");
    
    strcpy(up.szExt[0].key, "imei");
    strcpy(up.szExt[0].value, "test123");
    //strcpy(up.szExt[1].key, "img");
    //strcpy(up.szExt[1].value, argv[1]);
    
	char uploadpath[1024] = {0};
	sprintf(uploadpath, "index.php/Home/Upload/scanpic.html");
    int ret = cm_http_send_base(&up, url, uploadpath, argv[1]);
	printf("upload result [%d]\n", ret);
#endif
  
  return 0;  
}
#endif
