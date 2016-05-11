/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_msgQueue.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		27Oct14
 *
 * history 	\arg
 */
#ifndef __VT_MSGQUEUE_H__
#define __VT_MSGQUEUE_H__

#include <time.h>

#include <base/voip_comm.h>
#include <vt/vt_event.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define QUEUE_MAX           50

/*max is the max value that i can be*/
#define CIRCLE_ADD(i, max)    do {         \
            if (++(i) > (max)) \
            {                       \
                (i) = 0;              \
            } } while (0)

#define CIRCLE_SUB(i, max)    do {         \
            if (--(i) < 0)          \
            {                       \
                (i) = (max);      \
            } } while (0)

#define BUFLEN_VT_EVENT BUFLEN_VOIP_CM_CS_MSG
/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef struct _EVENT_QUEUE
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    //VOIP_DSPMSG msgQueue[QUEUE_MAX];
    VT_EVENT_CONTENT *eventQueue;

    int start;  /*The first element of queue which may be odd. WARN: The pData of this element may be freed!*/
                /*This variabel is only used to be lookup the previous event.*/
    int end;    /*Point to the last position of queue. Always point to an empty element.*/
    int nread;  /*The next event which needs to be process.*/

    //int size;
} VT_EVENT_QUEUE;

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
void vt_initMsgQueue();
void vt_putEvent(VT_EVENT_TYPE eventType, void *pBuf, size_t buflen);
int vt_getEventUntilTime(VT_EVENT_CONTENT *pEventOut, struct timeval *pLatestTime); /*return 1 if timeout, 0 otherwise*/

int vt_msgAll();
int vt_msgClear();
int vt_msgRefresh();
int vt_msgNew();

static inline int circlePreValue(int curValue, int maxValue)
{
    if (--curValue < 0)
    {
        return maxValue;
    }
    return curValue;
}

#endif // __VT_MSGQUEUE_H__
