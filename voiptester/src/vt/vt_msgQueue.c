/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_msgQueue.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		27Oct14
 *
 * history 	\arg
 */
#include <semaphore.h>
#include <pthread.h>

#include <base/common.h>
#include <base/voip_comm.h>
#include <vt/vt_config.h>
#include <vt/vt_send.h>
#include <vt/vt_msgQueue.h>


/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define VT_QUEUE_SIZE   (g_vtConfig.storeEventMax + 1)

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/


/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/


/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/
LOCAL int printMsg(int msgNumber, int start);

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
LOCAL VT_EVENT_QUEUE l_eventQueue = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NULL, 0, 0, 0};
sem_t g_newMsgSem;      /*Will be post when get new message from server.*/

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
/*LOCAL int getMsgWait(VOIP_DSPMSG *pMsg)
{
    while (l_eventQueue.nread == l_eventQueue.end)
    {
        CX_LOGL(CX_DBG, "msgQueue has no new message. waiting...");
        sem_wait(&g_newMsgSem);
        CX_LOGL(CX_DBG, "waiting for new message done.");
    }

    memcpy(pMsg, &l_eventQueue.msgQueue[l_eventQueue.nread], sizeof(VOIP_DSPMSG));
    CIRCLE_ADD(l_eventQueue.nread);

    CX_LOGL(CX_DBG, "getMsgWait done. return message: %s(%d)", vt_cmTxtMapById(pMsg->cmd), pMsg->cmd);

    return ABS(l_eventQueue.end - l_eventQueue.nread);
}*/


LOCAL int printMsg(int msgNumber, int start)
{
    int index = 0;
    //VOIP_DSPMSG_CMD cmd;
    VT_EVENT_TYPE eventType;
    //int cmdIndex = 0;
    printf("Message: start=%d, end=%d, nread=%d\n", l_eventQueue.start, l_eventQueue.end, l_eventQueue.nread);

    if (msgNumber == 0)
    {
        printf(CLRhigh"There is no required message."CLRnl);
    }
    else
    {
        //CX_LOGL(CX_INFO, "printf %d msg starting from %d", msgNumber, start);
        for (index = 0; index < msgNumber; index++)
        {
            eventType = l_eventQueue.eventQueue[start].eventType;
            //cmdIndex = getCmdIndex(cmd);
            printf(CLRhigh"Message %d:"CLRn" %s(%d)\n", index+1, vt_getEventStr(eventType), eventType);
            CIRCLE_ADD(start, g_vtConfig.storeEventMax);
        }
    }

    return start;
}
LOCAL int isNeighbor(int before, int after)
{
    return before == circlePreValue(after, g_vtConfig.storeEventMax);
}
LOCAL int queueIsFull()
{
    return isNeighbor(l_eventQueue.end, l_eventQueue.nread);
}
LOCAL int queueWasEmpty()   /*Was empty last time when new event was pushed*/
{
    return isNeighbor(l_eventQueue.nread, l_eventQueue.end);
}
LOCAL int queueIsEmpty()
{
    return l_eventQueue.end == l_eventQueue.nread;
}

LOCAL void freeQueue()
{
    int index;
    int nread = l_eventQueue.nread;
    int nleft = (l_eventQueue.end - nread + VT_QUEUE_SIZE) % VT_QUEUE_SIZE;

    for (index = 0; index < nleft; index++)
    {
        /*We donot free the element which start variable points to. It must be freed by calling vt_freeEvent*/
        free(l_eventQueue.eventQueue[nread].pData);
        CIRCLE_ADD(nread, g_vtConfig.storeEventMax);
    }

}
/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/

void vt_initMsgQueue()
{
    pthread_mutex_lock(&l_eventQueue.mutex);

    if (l_eventQueue.eventQueue)
    {
        /*vt_initMsgQueue may be called several times.*/
        freeQueue();
        free(l_eventQueue.eventQueue);
        l_eventQueue.eventQueue = NULL;
    }

    /**
    * NOTE: g_vtConfig.eventQueueSize means how much event can be stored. But which l_eventQueue.end point to
    *       is always empty.
    */
    l_eventQueue.eventQueue = calloc(VT_QUEUE_SIZE, sizeof(VT_EVENT_CONTENT));
    l_eventQueue.start = l_eventQueue.end = l_eventQueue.nread = 0;

    pthread_mutex_unlock(&l_eventQueue.mutex);

    sem_init(&g_newMsgSem, 0, 0);
}
void vt_putEvent(VT_EVENT_TYPE eventType, void *pBuf, size_t buflen)
{
    assert(l_eventQueue.eventQueue);

    pthread_mutex_lock(&l_eventQueue.mutex);

    while (queueIsFull())
    {
        /*Event queue is full.*/
        CX_LOGL(CX_DBG, "Event queue is full! Waiting for consuming.");
        pthread_cond_wait(&l_eventQueue.cond, &l_eventQueue.mutex);
        CX_LOGL(CX_DBG, "Waiting for consuming done. Test the condition again: %d", queueIsFull());
    }

    VT_EVENT_CONTENT *newEvent = &l_eventQueue.eventQueue[l_eventQueue.end];
    newEvent->eventType = eventType;
    newEvent->pData = calloc(buflen, 1);
    memcpy(newEvent->pData, pBuf, buflen);
    newEvent->dataLen = buflen;

    CIRCLE_ADD(l_eventQueue.end, g_vtConfig.storeEventMax);

    CX_LOGL(CX_DBG, "Put event: %s(%d). nread=%d, end=%d", vt_getEventStr(eventType), eventType,
            l_eventQueue.nread, l_eventQueue.end);

    if (l_eventQueue.start == l_eventQueue.end)
    {
        /*start always point to the first element of queue.*/
        CIRCLE_ADD(l_eventQueue.start, g_vtConfig.storeEventMax);
    }

    if (queueWasEmpty())
    {
        /*Event queue was empty before. Someone may be waiting*/
        //CX_LOGL(CX_DBG, "Wake up someone to consume the new event.");
        pthread_cond_signal(&l_eventQueue.cond);
    }

    pthread_mutex_unlock(&l_eventQueue.mutex);
}

int vt_getEventUntilTime(VT_EVENT_CONTENT *pEventOut, struct timeval *pLatestTime)
{
    assert(pEventOut && l_eventQueue.eventQueue);

    struct timespec spectime;
    spectime.tv_sec = pLatestTime->tv_sec;
    spectime.tv_nsec = pLatestTime->tv_usec * 1000;
    //spectime.tv_nsec = 1000000;

    pthread_mutex_lock(&l_eventQueue.mutex);

    while (queueIsEmpty())
    {
        if (timerisset(pLatestTime))
        {
            int ret;
            struct timeval now;
            gettimeofday(&now, NULL);

            CX_LOGL(CX_DBG, "Event queue is empty. Waiting for period: %lds %ldus", spectime.tv_sec-now.tv_sec, spectime.tv_nsec/1000-now.tv_usec);
            ret = pthread_cond_timedwait(&l_eventQueue.cond, &l_eventQueue.mutex, &spectime);

            if (ret == ETIMEDOUT)
            {
                struct timeval after;
                gettimeofday(&after, NULL);
                CX_LOGL(CX_DBG, "pthread_cond_timedwait timeout.Elapsing time: %lds %ldus", after.tv_sec-now.tv_sec, after.tv_usec-now.tv_usec);
                pthread_mutex_unlock(&l_eventQueue.mutex);
                return 1;
            }
        }
        else
        {
            CX_LOGL(CX_DBG, "Event queue is empty. Waiting...");
            pthread_cond_wait(&l_eventQueue.cond, &l_eventQueue.mutex);
            CX_LOGL(CX_DBG, "Waiting new event done. Test the condition again: %d", queueIsEmpty());
        }
    }

    VT_EVENT_CONTENT *pUnreadEvent = &l_eventQueue.eventQueue[l_eventQueue.nread];

    pEventOut->eventType = pUnreadEvent->eventType;
    pEventOut->dataLen = pUnreadEvent->dataLen;
    pEventOut->pData = pUnreadEvent->pData; /*For convenience, we just copy the pointer.*/

    /*We havenot modify l_eventQueue.nread, so now the queue may be full.*/
    if (queueIsFull())
    {
        pthread_cond_signal(&l_eventQueue.cond);
    }

    CIRCLE_ADD(l_eventQueue.nread, g_vtConfig.storeEventMax);

    pthread_mutex_unlock(&l_eventQueue.mutex);

    return 0;
}
void vt_freeEvent(VT_EVENT_CONTENT *pEvent)
{
    assert(pEvent);

    free(pEvent->pData);
    /*We done free pEvent because the content pEvent points to is always a local variable*/
}

int vt_msgClear()
{
    CX_LOGL(CX_DBG, "Clear message queue.");

    pthread_mutex_lock(&l_eventQueue.mutex);

    freeQueue();

    if (queueIsFull())    /*Was full before.*/
    {
        pthread_cond_signal(&l_eventQueue.cond);
    }

    l_eventQueue.start = l_eventQueue.end = l_eventQueue.nread = 0;

    pthread_mutex_unlock(&l_eventQueue.mutex);

    return VT_SUCCESS;
}
int vt_msgNew()
{
    pthread_mutex_lock(&l_eventQueue.mutex);

    int end = l_eventQueue.end;
    int nread= l_eventQueue.nread;
    int msgNumber = (end - nread + VT_QUEUE_SIZE) % VT_QUEUE_SIZE;

    printMsg(msgNumber, l_eventQueue.nread);

    pthread_mutex_unlock(&l_eventQueue.mutex);

    return VT_SUCCESS;
}
int vt_msgAll()
{
    pthread_mutex_lock(&l_eventQueue.mutex);

    int end = l_eventQueue.end;
    int start= l_eventQueue.start;
    int msgNumber = (end - start + VT_QUEUE_SIZE) % VT_QUEUE_SIZE;

    printMsg(msgNumber, l_eventQueue.start);

    pthread_mutex_unlock(&l_eventQueue.mutex);
    return VT_SUCCESS;
}
int vt_msgRefresh()
{
    CX_LOGL(CX_DBG, "Refresh message queue.");
    pthread_mutex_lock(&l_eventQueue.mutex);

    freeQueue();

    if (queueIsFull())    /*Was full before.*/
    {
        pthread_cond_signal(&l_eventQueue.cond);
    }
    l_eventQueue.nread = l_eventQueue.end;

    pthread_mutex_unlock(&l_eventQueue.mutex);
    return VT_SUCCESS;
}
