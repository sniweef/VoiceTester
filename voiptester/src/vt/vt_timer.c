/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_timer.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		24Nov14
 *
 * history 	\arg
 */

#include <base/common.h>

#include <vt/vt_timer.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
LOCAL VT_TIMER *l_timers = NULL;    /*The timer list*/

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL VT_TIMER *searchTimerById(int timerId)
{
    VT_TIMER *pTimer = l_timers;

    while (pTimer != NULL && pTimer->timerId != timerId)
    {
        pTimer = pTimer->pNext;
    }

    return pTimer;
}
LOCAL int insertNewTimer(VT_TIMER *pNewTimer)
{
    if (l_timers == NULL)
    {
        l_timers = pNewTimer;
        pNewTimer->pNext = NULL;
        return 0;
    }

    VT_TIMER *pTimer = l_timers;

    while (pTimer->pNext && pTimer->timerId == pTimer->pNext->timerId - 1)
    {
        pTimer = pTimer->pNext;
    }

    pNewTimer->pNext = pTimer->pNext;
    pTimer->pNext = pNewTimer;

    return (pTimer->timerId + 1);
}

/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
GLOBAL int vt_registerTimer(struct timeval *pTimeout, TIMEOUT_HANDLER handler, int data)
{
    VT_TIMER *pTimer;

    if ((pTimer = calloc(1, sizeof(VT_TIMER))) == NULL)
    {
        CX_LOGL(CX_ERR, "Mem fault.");
        return -1;
    }

    pTimer->timerId = insertNewTimer(pTimer);

    struct timeval nowTime;
    gettimeofday(&nowTime, NULL);

    timeradd(&nowTime, pTimeout, &pTimer->finalTime);

    pTimer->handler = handler;
    pTimer->data = data;

    return pTimer->timerId;
}
GLOBAL void vt_updateTimer(int timerId, struct timeval *pTimeout)
{
    VT_TIMER *pTimer = searchTimerById(timerId);

    if (pTimer == NULL)
    {
        CX_LOGL(CX_ERR, "Timer: %d not exists.", timerId);
        return ;
    }
    struct timeval nowTime;
    gettimeofday(&nowTime, NULL);

    timeradd(&nowTime, pTimeout, &pTimer->finalTime);
}
GLOBAL void vt_getLatestTimer(struct timeval *pLatestTime)
{
    timerclear(pLatestTime);

    VT_TIMER *pTimer = l_timers;

    while (pTimer != NULL)
    {
        if (timercmp(&pTimer->finalTime, pLatestTime, <) || !timerisset(pLatestTime))
        {
            *pLatestTime = pTimer->finalTime;
        }

        pTimer = pTimer->pNext;
    }
}
//GLOBAL void vt_notifyTimer(int timersId[], size_t len)
GLOBAL void vt_notifyTimer()
{
    //time_t nowTime = time(NULL);;
    struct timeval nowTime;
    if (gettimeofday(&nowTime, NULL) < 0)
    {
        CX_LOGL(CX_ERR, "Failed while gettimeofday. errno=%d", errno);
        return ;
    }

    VT_TIMER *pTimer = l_timers;
    VT_TIMER *pPrev = l_timers;

    while (pTimer != NULL)
    {
        if (timercmp(&nowTime, &pTimer->finalTime, >=))
        {
            pTimer->handler(pTimer->data);
#if 0
            if (timersId)
            {
                /*Thread which called vt_registerTimer may store the timerId in an array. So when */
                /*one timer is timeout, we must remove the element in this array.*/
                /*This can be done at TIMEOUT_HANDLER if the array is global.*/
                for (index = 0; index < len; index++)
                {
                    if (timersId[index] == pTimer->timerId)
                    {
                        timersId[index] = -1;
                    }
                }
            }
#endif // 0

            if (pTimer == l_timers)
            {
                l_timers = pTimer->pNext;
                free(pTimer);
                pTimer = pPrev = l_timers;
            }
            else
            {
                free(pTimer);
                pTimer = pPrev->pNext;
            }
        }
        else
        {
            pPrev = pTimer;
            pTimer = pTimer->pNext;
        }
    }
}
