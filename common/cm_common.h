/**
 * Create: 20180515
 * Author: jmdvirus
 */

#ifndef __CM_COMMON_H
#define __CM_COMMON_H

#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#define API_PREFIX

#ifdef __cplusplus
extern "C" {
#endif

enum {
   JK_RESULT_SUCCESS = 0,
   JK_RESULT_E_FAIL = -1,
   JK_RESULT_E_MALLOC_FAIL = -2,
   JK_RESULT_E_PARAM_ERROR = -3,
 
   JK_RESULT_E_OP_OPEN_FAILED = -11,
};

#define  JKTRUE 1
#define  JKFALSE 0
#define  JKINVALID -1


#ifdef __cplusplus
}
#endif

#endif
