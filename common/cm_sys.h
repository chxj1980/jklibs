/*********************************************************
 *
 * Filename: cm_sys.h
 *   Author: jmdvirus
 *   Create: 2018年12月12日 星期三 10时42分48秒
 *
 *********************************************************/

#include "cm_common.h"

/**
 * System encapsulation
 */

#ifdef _WIN32
#else
#include <unistd.h>
#define cm_sys_usleep(x) usleep(x)
#endif

