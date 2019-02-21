/*********************************************************
 *
 * Filename: base/cm_debug_out.h
 *   Author: jmdvirus
 *   Create: 2018年12月12日 星期三 10时36分24秒
 *
 *********************************************************/

#include "cm_common.h"

/**
 * debug function connect to server with tcp and websocket
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CMDebugOut *CMDebugOut;

enum {
	CM_DOUT_NONE = 0,
	CM_DOUT_ERROR = 1, // connect error or recv error NOT IMPLEMENT
	CM_DOUT_DATA = 2,  // recv data
};

// @type CM_DOUT_*
typedef int (*recv_callback)(int type, char *data, size_t length, void *userdata);

API_PREFIX int cm_debug_out_init(CMDebugOut *dout, const char *url, const char *addr, int port, recv_callback cb, void *userdata);

API_PREFIX int cm_debug_out_deinit(CMDebugOut *dout);

/**
 * @resdata, @reslength must set if @waitres == 1
 */
API_PREFIX int cm_debug_out_request(CMDebugOut dout, int waitres, const char *data, size_t length,
		char *resdata, size_t *reslength);

/**
 * Never block, caller call always
 */
API_PREFIX int cm_debug_out_run(CMDebugOut dout);

#ifdef __cplusplus
}
#endif

