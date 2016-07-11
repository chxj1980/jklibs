/*
 *===========================================================================
 *
 *          Name: demo_http.c
 *        Create: 2015年10月22日 星期四 15时07分18秒
 *
 *   Discription: desc
 *
 *        Author: jmdvirus
 *         Email: jmdvirus@roamter.com
 *
 *===========================================================================
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "http/ghttp.h"
#include "rt_print.h"

ghttp_request *kfhttp;

int main() {
	kfhttp = ghttp_request_new();
	if (!kfhttp) {
		rterror("request new failed.");
		return -1;
	}

	int ret = ghttp_set_uri(kfhttp, "http://114.215.128.113:8085/unite/service");
    if (ret < 0) {
    	rterror("set uri failed %d", ret);
    	return -2;
    }

    ret = ghttp_set_type(kfhttp, ghttp_type_post);
    if (ret < 0) {
    	rterror("set type error %d", ret);
    	return -6;
    }

	char *requestdata = "{\"MsgBody\": [{\"PARAMS\": { } } ], \"MsgHead\": {\"ServiceCode\": "
	"\"queryAvailableServerIp\", \"SrcSysID\": \"0001\", \"SrcSysSign\": \"46c541617d7dddd7936175412a99137d\", "
	"\"transactionID\": \"1439347340985\"} }";
	int lenreq = strlen(requestdata);

	ret = ghttp_set_body(kfhttp, requestdata, lenreq);
	if (ret < 0) {
		rterror("set body failed. %d", ret);
		return -3;
	}

	// ghttp_set_sync(kfhttp, ghttp_sync);

	ret = ghttp_prepare(kfhttp);
	if (ret < 0) {
		rterror("set prepare fail %d", ret);
		return -4;
	}

	ret = ghttp_process(kfhttp);
	rtinfo("process result %d", ret);

    ret = ghttp_status_code(kfhttp);
    rtinfo("status code : %d", ret);

    const char *error = ghttp_get_error(kfhttp);
    if (error) {
    	rtinfo("The error : %s", error);
    }

    char *body = ghttp_get_body(kfhttp);	
    if (body) {
    	rtinfo("the body is : %s", body);
    } else {
    	rterror("get body failed.")
    	return -5;
    }

	ghttp_request_destroy(kfhttp);
	return 0;
}

/*=============== End of file: demo_http.c ==========================*/
