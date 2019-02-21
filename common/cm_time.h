/*********************************************************
 *
 * Filename: cm/cm_time.h
 *   Author: jmdvirus
 *   Create: 2019年01月02日 星期三 20时42分16秒
 *
 *********************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "cm_utils.h"

typedef struct {
	int                iStart;
    unsigned long long ullLastTime;
	int                iDuration;   // ms
} CM_TimeWait;

int cm_timewait_start(CM_TimeWait *tw, int duration);
int cm_timewait_stop(CM_TimeWait *tw);

/**
 * return: > 0 time up
 *           0 cycle
 *          -1 not start or other error
 */
int cm_timewait_check(CM_TimeWait *tw);

#ifdef __cplusplus
}
#endif

