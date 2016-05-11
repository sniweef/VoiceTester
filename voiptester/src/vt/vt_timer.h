/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_timer.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		24Nov14
 *
 * history 	\arg
 */
#ifndef __VT_TIMER_H__
#define __VT_TIMER_H__

#include <time.h>
/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
typedef void (*TIMEOUT_HANDLER)(int data);

typedef struct _VT_TIMER
{
    int timerId;
    struct timeval finalTime;   /*When this timer times out*/
    TIMEOUT_HANDLER handler;
    int data;
    struct _VT_TIMER *pNext;
} VT_TIMER;

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
GLOBAL int vt_registerTimer(struct timeval *pTimeout, TIMEOUT_HANDLER handler, int data);
GLOBAL void vt_updateTimer(int timerId, struct timeval *pTimeout);
GLOBAL void vt_notifyTimer();
GLOBAL void vt_getLatestTimer(struct timeval *pTimeout);

#endif	/* __VT_TIMER_H__ */
