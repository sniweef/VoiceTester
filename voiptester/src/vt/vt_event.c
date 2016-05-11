/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_event.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		15Oct14
 *
 * history 	\arg
 */

#include <semaphore.h>

#include <base/common.h>
#include <base/voip_comm.h>

#include <vt/vt_send.h>
#include <vt/vt_action.h>
#include <vt/vt_event.h>
#include <vt/vt_msgQueue.h>
#include <vt/vt_config.h>
#include <vt/vt_fsm.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
/*
 * brief	call manager timer second
 */
#define CM_TIMEOUT_SEC    1
/*
 * brief	call manager timer micro second
 */
#define CM_TIMEOUT_USEC   (500 * 1000)

#define BACKLOG     5

#define CALL_NUM_MAX        9999999999999

#define CHN_MAX_PER_ENDPT   2

#define VT_SESS_FDS_MAX      CHN_MAX_PER_ENDPT * VT_ENDPT_MAX

//#define OUTER_CLIENT_IP     (int32_t)0xC0A80102  /*192.168.1.2*/


/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef struct _VT_EVENT_STR_MAP
{
    VT_EVENT_TYPE evenType;
    const char *pStr;
} VT_EVENT_STR_MAP;

/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
sem_t g_connectedSem;   /*son thread use it to wait for connection success.*/

LOCAL VT_EVENT_STR_MAP l_eventStrMap[] =
{
    STR(VT_EVENT_SLIC_LOCALE),
    STR(VT_EVENT_DSP_START),
    STR(VT_EVENT_HOOK_ON),
    STR(VT_EVENT_HOOK_OFF),
    STR(VT_EVENT_HOOK_FLASH),
    STR(VT_EVENT_DTMF_START),
    STR(VT_EVENT_DTMF_END),

    STR(VT_EVENT_NUM_DONE),
    STR(VT_EVENT_NUM_CALLED),
    STR(VT_EVENT_NUM_ERR),

    STR(VT_EVENT_CALLING),
    STR(VT_EVENT_CALL_ACCEPT),
    STR(VT_EVENT_CALL_DOWN),
    STR(VT_EVENT_CALL_REJECT),
    STR(VT_EVENT_MAX),
};

LOCAL int l_activeSessFds[VT_SESS_FDS_MAX]; //store the fd of active call session
LOCAL pthread_mutex_t l_sessFdsMutex = PTHREAD_MUTEX_INITIALIZER;   //use to access l_activeSessFds

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL VT_EVENT_TYPE dspCmdToEventType(VOIP_DSPMSG_CMD cmd)
{
    return (VT_EVENT_TYPE)cmd;
}
LOCAL int convertDtmfDigit(uint32_t dtmfIndex)
{
    int index = 0;

    for (; index < DTMF_DIGIT_AMOUNT; index++)
    {
        if (g_vtConfig.dtmfDigitMap[index] == dtmfIndex)
        {
            return index;
        }
    }
    return DTMF_DIGIT_AMOUNT;   /* Search failed. Return a impossible number */
}
LOCAL int putDspEvent(int fd)
{
    VOIP_DSPMSG dspmsg;
    bzero(&dspmsg, sizeof(dspmsg));

    int nread = sock_tcpRead(fd, &dspmsg, sizeof(dspmsg));

    if (nread == SOCKERR_IO)
    {
        CX_LOGL(CX_ERR, "Socket(%d) error while reading DSP Event.", fd);
        shutdown(fd, SHUT_RDWR);
        fd = -1;
    }
    else if (nread == SOCKERR_CLOSED)
    {
        CX_LOGL(CX_ERR, "Socket(%d) reading DSP Event has been closed.", fd);
        shutdown(fd, SHUT_RDWR);
        fd = -1;
    }
    else
    {
        dspmsg.cmd = ntohl(dspmsg.cmd);

        if (dspmsg.cmd == VOIP_DTMF_END)
        {
            VOIP_DTMF_MSG dtmfMsg;
            memcpy(&dtmfMsg, dspmsg.buf, sizeof(dtmfMsg));

            dtmfMsg.endpt = ntohl(dtmfMsg.endpt);
            dtmfMsg.dtmf = ntohl(dtmfMsg.dtmf);
            dtmfMsg.dtmf = convertDtmfDigit(dtmfMsg.dtmf);

            vt_putEvent(dspCmdToEventType(dspmsg.cmd), &dtmfMsg, sizeof(dtmfMsg));
        }
        else
        {
            int32_t endpt;
            memcpy(&endpt, dspmsg.buf, sizeof(endpt));
            endpt = ntohl(endpt);
            //memcpy(&endpt, dspmsg.buf, sizeof(endpt));

            vt_putEvent(dspCmdToEventType(dspmsg.cmd), &endpt, sizeof(endpt));
        }


    }
    return fd;
}
LOCAL int putCallEvent(int fd)
{
    CALL_MSG callMsg;
    bzero(&callMsg, sizeof(callMsg));

    int nread = sock_tcpRead(fd, &callMsg, sizeof(callMsg));

    if (nread == SOCKERR_IO)
    {
        CX_LOGL(CX_ERR, "Socket(%d) error while reading Call Event.", fd);
        shutdown(fd, SHUT_RDWR);
        fd = -1;
    }
    else if (nread == SOCKERR_CLOSED)
    {
        CX_LOGL(CX_ERR, "Socket(%d) reading Call Event has been closed.", fd);
        shutdown(fd, SHUT_RDWR);
        fd = -1;
    }
    else
    {
        VT_CALL_EVENT callEvent;
        callEvent.callMsg = callMsg;
        callEvent.fd = fd;
        CX_LOGL(CX_INFO, "Call event:%s fd=%d srcNumber=%llu", vt_getEventStr(callMsg.eventType), fd, callMsg.srcNumber);

        vt_putEvent(callMsg.eventType, &callEvent, sizeof(callEvent));
    }

    return fd;
}

/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
const char *vt_getEventStr(VT_EVENT_TYPE eventType)
{
    VT_EVENT_STR_MAP *pEventIndex = l_eventStrMap;

    while (pEventIndex->evenType != VT_EVENT_MAX)
    {
        if (pEventIndex->evenType == eventType)
        {
            return pEventIndex->pStr;
        }

        pEventIndex++;
    }
    return pEventIndex->pStr;
}
VT_EVENT_TYPE vt_eventStrToInt(const char *pEventStr)
{
    if (pEventStr == NULL)
    {
        return VT_EVENT_MAX;
    }

    VT_EVENT_STR_MAP *pEventIndex = l_eventStrMap;

    while (pEventIndex->evenType != VT_EVENT_MAX)
    {
        if (strcmp(pEventIndex->pStr, pEventStr) == 0)
        {
            return pEventIndex->evenType;
        }

        pEventIndex++;
    }
    return pEventIndex->evenType;
}

void vt_registerSessionFd(int fd)
{
    //l_activeSessFds[endpt] =fd;
    //TODO : lock
    int index;

    pthread_mutex_lock(&l_sessFdsMutex);
    for (index = 0; index < VT_SESS_FDS_MAX; index++)
    {
        if (l_activeSessFds[index] < 0)
        {
            l_activeSessFds[index] = fd;
            break;
        }
    }
    pthread_mutex_unlock(&l_sessFdsMutex);
}
void vt_unregisterSessionFd(int fd)
{
    int index;

    pthread_mutex_lock(&l_sessFdsMutex);
    for (index = 0; index < VT_SESS_FDS_MAX; index++)
    {
        if (l_activeSessFds[index] == fd)
        {
            l_activeSessFds[index] = -1;
            break;
        }
    }
    pthread_mutex_unlock(&l_sessFdsMutex);
}
void *vt_listenEvent(void *pArg)
{
    int voIPSock = *(int *)pArg;
    int listenFd = -1;
    fd_set readfds;
    int maxfd;
    struct timeval timeout;
    int selectRet = 0;
    int index;

    for (index = 0; index < VT_SESS_FDS_MAX; index++)
    {
        l_activeSessFds[index] = -1;
    }

    listenFd = sock_tcpServer(g_vtConfig.clientListenPort, BACKLOG);

    while (listenFd < 0)
    {
        CX_LOGL(CX_WARN, "Create server failed. Try again after 1s");
        sleep(1);
        listenFd = sock_tcpServer(g_vtConfig.clientListenPort, BACKLOG);
    }

    while (1)
    {
        FD_ZERO(&readfds);

        if (voIPSock < 0)
        {
            CX_LOGL(CX_DBG, "Wait for connecting to VoIP server...");
            sem_wait(&g_connectedSem);
            CX_LOGL(CX_DBG, "Waiting connection done.");
            voIPSock = vt_getVoIPSock();
        }

        FD_SET(voIPSock, &readfds);
        FD_SET(listenFd, &readfds);
        maxfd = MAX(voIPSock, listenFd);
        timeout = (struct timeval){0, 200};

        pthread_mutex_lock(&l_sessFdsMutex);
        for (index = 0; index < VT_SESS_FDS_MAX; index++)
        {
            if (l_activeSessFds[index] > 0)
            {
                FD_SET(l_activeSessFds[index], &readfds);
                maxfd = MAX(l_activeSessFds[index], maxfd);
            }
        }
        pthread_mutex_unlock(&l_sessFdsMutex);

        selectRet = select(maxfd+1, &readfds, NULL, NULL, &timeout);
        if (selectRet < 0)
        {
            CX_LOGL(CX_ERR, "select error!! errno=%d", errno);
        }
        else if (selectRet == 0)
        {

        }
        else
        {
            if (FD_ISSET(voIPSock, &readfds))
            {
                voIPSock = putDspEvent(voIPSock);
            }

            if (FD_ISSET(listenFd, &readfds))
            {
                /*Some one connect us.*/
                int acceptFd = accept(listenFd, NULL, NULL);

                if (acceptFd < 0)
                {
                    CX_LOGL(CX_ERR, "Accept failed!!");
                }
                /*else if (clientsConnected >= VT_ENDPT_MAX)
                {
                    CX_LOGL(CX_ERR, "Only %d client can connect us. Close this one.", VT_ENDPT_MAX);
                    close(acceptFd);
                }*/
                else
                {
                    CX_LOGL(CX_DBG, "Accept one client. fd = %d", acceptFd);

                    pthread_mutex_lock(&l_sessFdsMutex);
                    for (index = 0; index < VT_SESS_FDS_MAX; index++)
                    {
                        if (l_activeSessFds[index] < 0)
                        {
                            l_activeSessFds[index] = acceptFd;
                            //FD_SET(l_activeSessFds[index], &readfds);
                            //maxfd = MAX(l_activeSessFds[index], maxfd);
                            break;
                        }
                    }
                    pthread_mutex_unlock(&l_sessFdsMutex);

                }
            }

            pthread_mutex_lock(&l_sessFdsMutex);
            for (index = 0; index < VT_SESS_FDS_MAX; index++)
            {
                if (l_activeSessFds[index]  > 0 && FD_ISSET(l_activeSessFds[index], &readfds) > 0)
                {
                    if (putCallEvent(l_activeSessFds[index]) < 0)
                    {
                        l_activeSessFds[index] = -1;
                    }

                }
            }
            pthread_mutex_unlock(&l_sessFdsMutex);

        }

    }
}
