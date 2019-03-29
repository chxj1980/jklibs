/*********************************************************
 *
 * Filename: common/misc/cm_fifo.h
 *   Author: jmdvirus
 *   Create: 2019年03月28日 星期四 15时07分27秒
 *
 *********************************************************/

#include <stdio.h>

typedef void *CMFifoReadHandle;

typedef void (*fifo_read_callback)(char *data, int len);

int cm_fifo_r_open(CMFifoReadHandle *fd, const char *path, fifo_read_callback callback);

int cm_fifo_r_close(CMFifoReadHandle *fd);


typedef void *CMFifoWriteHandle;

int cm_fifo_w_open(CMFifoWriteHandle *fd, const char *path);

int cm_fifo_w_write(CMFifoWriteHandle fd, const char *data, int len);

int cm_fifo_w_close(CMFifoWriteHandle *fd);

