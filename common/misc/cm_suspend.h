/*********************************************************
 *
 * Filename: cm/misc/cm_suspend.h
 *   Author: jmdvirus
 *   Create: 2019年01月06日 星期日 10时53分42秒
 *
 *********************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

typedef struct {
	int         iStart;  // if start to check suspend
	int         iStatus; // 0 not suspend, 1 suspend
	int         iDuration; // how long to suspend (ms)
    unsigned long long    ullLastTime;
	unsigned long long    ullLastSuspend;

	int         iWakeUp;        // If enable wakeup check
	int         iWakeDuration;  // how long to wakeup (ms)

	pthread_mutex_t       iLock;
} CMSuspend;

// @duration: ms
int cm_suspend_start(CMSuspend *sus, int duration);
/**
 * Stop suspend and wake up
 */
int cm_suspend_stop(CMSuspend *sus);

/**
 * Enable suspend and set duration to wakeup
 * @duration: ms
 */
int cm_suspend_wake(CMSuspend *sus, int duration);

/**
 * return 1 suspend
 *        0 no suspend/ wakeup
 */
int cm_suspend_status(CMSuspend *sus);

/**
 * return < 0 error/disable
 *        = 0 checking
 *        = 10 time to suspend
 *        = 11 have report suspend, so change to other number
 *        ** below only when suspend
 *        = 20 time to wakeup - then return to other status next
 */
int cm_suspend_check(CMSuspend *sus);

#ifdef __cplusplus
}
#endif

