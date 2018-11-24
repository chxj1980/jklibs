/*********************************************************
 *
 * Filename: demo/demo_llhttp.c
 *   Author: jmdvirus
 *   Create: 2018年11月24日 星期六 11时01分02秒
 *
 *********************************************************/

#include <stdio.h>

#include "llhttp.h"
#include "cm_logprint.h"

CMLogPrint logPrint;

int handle_on_message_complete(llhttp_t *ll) {
    printf("response of message complete status code [%d]\n", ll->status_code);

    return 0;
}

int demo_start(const char *url) {
    llhttp_t parser;
    llhttp_settings_t settings;

    /* Initialize user callbacks and settings */
    llhttp_settings_init(&settings);

    /* Set user callback */
    settings.on_message_complete = handle_on_message_complete;

    /* Initialize the parser in HTTP_BOTH mode, meaning that it will select between
    * HTTP_REQUEST and HTTP_RESPONSE parsing automatically while reading the first
    * input.
    */
    llhttp_init(&parser, HTTP_BOTH, &settings);

    /* Use `llhttp_set_type(&parser, HTTP_REQUEST);` to override the mode */

    /* Parse request! */
    const char* request = "GET / HTTP/1.1\r\n\r\n";
    int request_len = strlen(request);

    enum llhttp_errno err = llhttp_execute(&parser, request, request_len);
    if (err == HPE_OK) {
      /* Successfully parsed! */
    } else {
      fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(err),
              parser.reason);
    }
}

int main(int argc, char **args) {
  demo_start(args[1]);
	return 0;
}
