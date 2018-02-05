/*
 */

#ifndef __PROCESS_H_
#define __PROCESS_H_

#include "base.h"

int rs_init(GlobalInfo *gi);
int rs_deinit(GlobalInfo *gi);
int rs_run(GlobalInfo *gi);
int rs_start(GlobalInfo *gi, int width, int height);
int sock_init(GlobalInfo *gi, const char *unixpath);
int sock_send(GlobalInfo *gi, unsigned char *data, unsigned int len);
int sock_deinit(GlobalInfo *gi);

#endif

