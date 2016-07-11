/*
 * XN_Queue.h
 *
 *  Created on: 2009-12-26
 *      Author: xu_nuo
 */

#ifndef XN_QUEUE_H_
#define XN_QUEUE_H_
#include <time.h>
#include <pthread.h>


/**
 * Type:		XN_QueElem.
 * Parameters:	prev:	Previous element.
 * 				next:	Next element.
 * Description:	Queue element.
 */
typedef	struct XN_QueElem
{
	struct XN_QueElem *prev;
	struct XN_QueElem *next;
} XN_QueElem, *XN_QueHandle;


/**
 * Type:		DVS_QueHead.
 * Parameters:	elem:	Queue element.
 * 				num:	Element number.
 * 				mutex:	Mutex lock.
 * 				cond:	Condition lock.
 */
typedef	struct XN_QueHead
{
	XN_QueElem	elem;
	int				num;
    int         cur;  // Where to get now.
	pthread_mutex_t mutex;
	pthread_cond_t	cond;
} XN_QueHead;


#define	XN_QueueLock(h)			pthread_mutex_lock(&(h->mutex))
#define	XN_QueueUnlock(h)		pthread_mutex_unlock(&(h->mutex))


/**
 * Function:	XN_QueueInit().
 * Input:		head:	Queue head.
 * Output:		None.
 * Return:		None.
 * Description:	Initialize queue head.
 */
static inline void
XN_QueueInit (XN_QueHead *head)
{
	pthread_mutex_init(&(head->mutex), NULL);
	pthread_cond_init(&(head->cond), NULL);
	head->num		= 0;
    head->cur       = -1;  // Set to -1 as we increase it than get the value.
	head->elem.next = &(head->elem);
	head->elem.prev = &(head->elem);
}


/**
 * Function:	XN_QueueDestroy().
 * Input:
 * Output:
 * Return:
 * Description:
 */
static inline void
XN_QueueDestroy (XN_QueHead *head)
{
	pthread_mutex_lock(&(head->mutex));
	pthread_mutex_destroy(&(head->mutex));
	pthread_cond_destroy(&(head->cond));

	head->num		= 0;
	head->elem.next = &(head->elem);
	head->elem.prev = &(head->elem);
}


/**
 * Function:	XN_QueueGet().
 * Input:		head:	Queue head.
 * Output:		None.
 * Return:		Queue element.
 * Description:	Get a queue element from queue head.
 */
static inline XN_QueHandle
XN_QueueGet (XN_QueHead *head)
{
	XN_QueHandle handle;

	XN_QueueLock(head);

	/**
	 * Description:	Get the first element.
	 */
	handle = head->elem.next;
	if (handle == &(head->elem))
	{
		/**
		 * Description:	No element.
		 */
		head->num = 0;
        head->cur = -1;
		handle 	  = NULL;
	}
	else
	{
		/**
		 * Description:	Remove element from head.
		 */
		head->num--;
        head->cur--;
		head->elem.next    = handle->next;
		handle->next->prev = &(head->elem);
	}

	XN_QueueUnlock(head);

	return handle;
}



static inline XN_QueHandle
XN_QueueGetIndex(XN_QueHead *head, int indx)
{
    XN_QueHandle handle;
    
    XN_QueueLock(head);
    
    // If indx == -1, we use head->cur, and increase self.
    if (indx == -1) indx = head->cur+1;
    
    handle = head->elem.next;
    if (handle == &(head->elem)) {
        head->num = 0;
        handle = NULL;
    } else if (indx >= head->num){
        handle = NULL;
    } else {
        int start = 0;
        while (handle && handle->next) {
            if (start == indx) {
                head->cur++;
                break;
            }
            handle = handle->next;
            start++;
        }
    }
    
    XN_QueueUnlock(head);
    
    return handle;
}


/**
 * Function:	XN_QueuePut().
 * Input:		head:	Queue head.
 * 				handle:	Queue element.
 * Output:		None.
 * Return:		None.
 * Description:	Put a element into queue head.
 */
static inline void
XN_QueuePut(XN_QueHead *head, XN_QueHandle handle)
{
	XN_QueueLock(head);

	head->elem.prev->next = handle;
	handle->prev		  = head->elem.prev;
	head->elem.prev		  = handle;
	handle->next		  = &(head->elem);
	head->num++;

	XN_QueueUnlock(head);
}

static inline void
XN_QueuePutTail(XN_QueHead *head, XN_QueHandle handle)
{
    XN_QueueLock(head);
    
    XN_QueHandle inhandle = head->elem.next;

    if ((inhandle = &(head->elem))) {
        head->elem.prev->next = handle;
        handle->prev = head->elem.prev;
        head->elem.prev = handle;
        handle->next = &(head->elem);
        head->num++;
    } else {
        while ((inhandle = inhandle->next)) {
            if (inhandle->next == inhandle) {
                handle->prev = inhandle;
                inhandle->next = handle;
                handle->next = handle;
                head->num++;
                break;
            }
        }
    }
    
    XN_QueueUnlock(head);
}


/**
 * Function:	XN_QueuePut().
 * Input:		head:	Queue head.
 * 				handle:	Queue element.
 * Output:		None.
 * Return:		None.
 * Description:	Put a element into queue head.
 */
static inline int
XN_QueueIsEmpty (XN_QueHead *head)
{
	int empty;

	XN_QueueLock(head);
	empty = (0 == head->num) ? 1 : 0;
	XN_QueueUnlock(head);

	return empty;
}


static inline int
XN_QueueGetNum (XN_QueHead *head)
{
	int num;

	XN_QueueLock(head);
	num = head->num;
	XN_QueueUnlock(head);

	return num;
}


/**
 * Function:	XN_QueueMove().
 * Input:		src:	Source queue head.
 * Output:		dst:	Destination queue head.
 * Return:		None.
 * Description:	Move one queue into another queue head.
 */
static inline void
XN_QueueMove (XN_QueHead *dst, XN_QueHead *src)
{
	XN_QueHandle elem1, elem2;


	XN_QueueLock(src);
	XN_QueueLock(dst);

	/**
	 * Description:	Get first element.
	 */
	elem1 = src->elem.next;
	if (elem1 != &src->elem)
	{
		/**
		 * Description:	Get the last one.
		 */
		elem2 = src->elem.prev;

		/**
		 * Description:	Move elements.
		 */
		dst->elem.prev->next = elem1;
		elem1->prev		     = dst->elem.prev;
		dst->elem.prev		 = elem2;
		elem2->next		  	 = &(dst->elem);

		/**
		 * Description:	Clear elements.
		 */
		src->elem.prev		 = &src->elem;
		src->elem.next		 = &src->elem;

		/**
		 * Description:	Update elment number.
		 */
		dst->num += src->num;
		src->num  = 0;
	}

	XN_QueueUnlock(src);
	XN_QueueUnlock(dst);
}


/**
 * Function:
 * Input:
 * Output:
 * Return:
 * Description:
 */
static inline int
XN_QueueGetWait(XN_QueHead *head)
{
    pthread_cond_wait(&head->cond, &head->mutex);
    pthread_mutex_unlock(&head->mutex);

    return 0;
}

#if 0
/* timeout unit : millisecond */
static inline int
XN_QueueGetTimeout (XN_QueHead *head, int timeout)
{
	struct timespec abstime;
	struct timeval tv;
	long s;
	int  ret;

	gettimeofday(&tv, NULL);
	s = timeout * 1000 + tv.tv_usec;
	tv.tv_sec += s / 1000000;
	tv.tv_usec = s % 1000000;

	abstime.tv_sec  = tv.tv_sec;
	abstime.tv_nsec = tv.tv_usec * 1000;

	ret = pthread_cond_timedwait(&head->cond, &head->mutex, &abstime);
	pthread_mutex_unlock(&head->mutex);
	if (ret == ETIMEDOUT )
	{
		return -1;
	}

	return 0;
}
#endif



/**
 * Function:
 * Input:
 * Output:
 * Return:
 * Description:
 */
static inline int
XN_QueueWakeup (XN_QueHead *head)
{
	return pthread_cond_broadcast(&head->cond);
}

#define	XN_QueueIterate(em, hd)	for (em=&hd.elem; em!=(XN_QueHandle)&hd; em=em->next)
#define	XN_QueueIterate1(em, hd)	for (em=&hd->elem; em!=(XN_QueHandle)hd; em=em->next)



#endif /* XN_QUEUE_H_ */
