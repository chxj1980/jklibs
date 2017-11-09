/*
 * Author: jmdvirus
 * Create: 2017-11-09
 * Desc: V4L2 operation
 */

#define API_PREFIX
#ifdef _WIN32
#endif

typedef void *VDevHandle;

/*
 * Open device
 * return: < 0 fail, 0 success
 */
API_PREFIX int VDevOpen(VDevHandle* handle, const char *devname);

/*
 * Free
 */
API_PREFIX int VDevClose(VDevHandle* handle);

/*
 * Start Stream
 */
API_PREFIX int VDevStart(VDevHandle handle);

/*
 * Stop stream
 */
API_PREFIX int VDevStop(VDevHandle handle);

/*
 * Get Video data
 * return:
 * 0 success
 * < 0 fail
 */
API_PREFIX int VDevGetStream(VDevHandle handle, char **buffer, unsigned int *len);
