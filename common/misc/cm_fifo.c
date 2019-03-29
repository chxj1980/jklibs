/*********************************************************
 *
 * Filename: common/misc/cm_fifo.c
 *   Author: jmdvirus
 *   Create: 2019年03月28日 星期四 15时07分46秒
 *
 *********************************************************/

#include <stdio.h>

#include <malloc.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include "cm_fifo.h"

#include "cm_logprint.h"

#ifdef __MAINTEST
CMLogPrint logPrint;
#endif

typedef struct {
	char             szPath[256];
	int              iFifo;
	int              iFD;
	char             szData[1024];
	int              iLen;
	fifo_read_callback   cb;

	pthread_t        iThread;
	int              iExit;
} CMFifoHandleInfo;

void *cm_fifo_r_thread(void *args)
{
	cmdebug("fifo read thread start\n");
	CMFifoHandleInfo *in = (CMFifoHandleInfo*)args;

	while(!in->iExit) {
		in->iLen = 1024;
        in->iLen = read(in->iFD, in->szData, in->iLen);

		if (in->iLen > 0) {
			if (in->szData[in->iLen-1] == '\n') {
				in->szData[in->iLen-1] = '\0';
			}
			cmdebug("fifo read thread read out data len [%d] [%s]\n", in->iLen, in->szData);
			if (in->cb) {
				in->cb(in->szData, in->iLen);
			}
		}

		sleep(1);
	}

	cmdebug("fifo read thread exit\n");
	return NULL;
}

int cm_fifo_r_open(CMFifoReadHandle *fd, const char *path, fifo_read_callback callback)
{
	if (!path) return -1; 

	CMFifoHandleInfo *in = (CMFifoHandleInfo*)malloc(sizeof(CMFifoHandleInfo));
	if (!in) return -2;

    strncpy(in->szPath, path, sizeof(in->szPath));

	if (access(in->szPath, F_OK) == 0) {
		remove(in->szPath);
	}

	in->iFifo = mkfifo(in->szPath, 0644);
	if (in->iFifo == -1) {
		free(in);
		return -3;
	}

	in->cb = callback;

    in->iFD = open(in->szPath, O_RDONLY);
	if (!in->iFD) {
		free(in);
		return -4;
	}

	int ret = pthread_create(&in->iThread, NULL, cm_fifo_r_thread, in);
	if (ret < 0) {
        cmerror("pthread_create fail\n");
		free(in);
		return -5;
	}

	in->iExit = 0;
	pthread_detach(in->iThread);

	cmdebug("fifo read open success\n");

	if (*fd) *fd = in;

	return 0;
}

int cm_fifo_r_close(CMFifoReadHandle *fd)
{
	if (!fd || !*fd) return -1;
	cmdebug("fifo read close \n");
	CMFifoHandleInfo *in = *fd;
	in->iExit = 1;
	//TODO: Use sema
	sleep(1);
	free(in);
	return 0;
}

int cm_fifo_w_open(CMFifoWriteHandle *fd, const char *path)
{
	if (path) return -1;

	int fdd = open(path, O_WRONLY);
	if (fdd < 0) return -2;

	CMFifoHandleInfo *in = (CMFifoHandleInfo*)malloc(sizeof(CMFifoHandleInfo));
	if (!in) return -3;
	in->iFD = fdd;
	strncpy(in->szPath, path, sizeof(in->szPath));

	if (*fd) *fd = in;

	cmdebug("fifo write open success\n");

	return 0;
}

int cm_fifo_w_write(CMFifoWriteHandle fd, const char *data, int len)
{
	if (!fd || !data || len <= 0) return -1;
	CMFifoHandleInfo *in = (CMFifoHandleInfo*)fd;
	int ret = write(in->iFD, data, len);
	return ret;
}

int cm_fifo_w_close(CMFifoWriteHandle *fd)
{
	if (!fd || !*fd) return -1;
	CMFifoHandleInfo *in = (CMFifoHandleInfo*)fd;
	free(in);
	return 0;
}

#ifdef __MAINTEST

void *ff_cb(char *data, int len)
{
	printf("ff_cb result: %d, [%s]\n", len, data);
	return NULL;
}

int main(int argc, char **args) {

	CM_LOG_PRINT_INIT("fiforead");

    CMFifoReadHandle fd;
    cm_fifo_r_open(&fd, "/tmp/test_cmd", ff_cb);

    while (1) {
		sleep(1);
	}
	return 0;
}
#endif

