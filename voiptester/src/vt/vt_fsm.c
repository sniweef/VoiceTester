/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_fsm.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		18Nov14
 *
 * history 	\arg
 */

#include <pthread.h>
#include <base/list.h>
#include <base/common.h>

#include <vt/vt_send.h>
#include <vt/vt_event.h>
#include <vt/vt_msgQueue.h>
#include <vt/vt_fsm.h>
#include <vt/vt_timer.h>
#include <vt/vt_action.h>
#include <vt/vt_config.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define VT_FSM_CTX_END {VT_EVENT_MAX, VT_STATE_MAX, VT_STATE_MAX, NULL, NULL, NULL, -1}
#define VT_INPUT_NUM_MAX    10
#define VT_DIGIT_TIMEOUT    (struct timeval){1, 500*1000}       /*1.5 ms*/

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef int (*EVENT_HANDLER)(int endpt, VT_EVENT_CONTENT *pEvent);
typedef int (*FAILED_HANDLER)(int endpt,VT_EVENT_CONTENT *pEvent);
typedef int (*ACTION_HANDLER)(const char *pData);
typedef void (*ACTION_DATA_SETER)(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent);
typedef int (*NUM_HANDLER)(int endpt, char *pNumber);

typedef struct _VT_FSM_CTX
{
    VT_EVENT_TYPE       evenType;           /*A event incoming*/
    VT_STATE            curState;           /*The current state of endpt. Set it to VT_STATE_MAX means that ignores current state.*/
    VT_STATE            newState;           /*The new state after succefully executing actionHandler and eventHandler*/
                                            /*Set it to VT_STATE_MAX means that current state will not be modified.*/
    EVENT_HANDLER       eventHandler;       /*Handling event*/
    FAILED_HANDLER      failedHandler;      /*Executing if actionHandler or eventHandler fails.*/
    ACTION_DATA_SETER   actionDataSeter;    /*Set the action data*/
    //ACTION_HANDLER  actionHandler;        /*Executing action*/
    int                 actionIndex;        /*The index of l_allActions array in vt_action.c. Dynamically set while load action from .vc_action file*/
} VT_FSM_CTX;

typedef struct _VT_DYNAMIC_FSM_CTX
{
    VT_FSM_CTX fsmCtx;
    //struct _VT_DYNAMIC_FSM *pNext;
    struct list_head list;
} VT_DYNAMIC_FSM_CTX;

typedef struct _VT_STATE_STR_MAP
{
    VT_STATE state;
    const char *pStateStr;
} VT_STATE_STR_MAP;

typedef struct _VT_INPUT_NUM
{
    char number[VT_INPUT_NUM_MAX];
    int numberIndex;
    int isDone;
    //pthread_mutex_t mutex;
} VT_INPUT_NUM;

typedef struct _VT_ENDPT_STATUS  /**/
{
    //unsigned long long callNumber;  /*The number user want to call*/
    int fxsState;
    int openChannel;
    int destIP;
    int destPort;
    int destCtrport;
    int sessionFd;
    unsigned long long destNumber;  /*Once the session is created, it stores the number of the other side.*/
    int digitTimerId;

    VT_INPUT_NUM inputNumber;       /*The number user input*/
} VT_ENDPT_STATUS;

typedef struct _VT_NUM_HAND_MAP
{
    char *pNumber;
    NUM_HANDLER numHandler;
    //struct list_head list;
} VT_NUM_HANDLER_MAP;

typedef struct _VT_KEY_ACTION_MAP
{
    const char *pKey;
    int actionIndex;
    struct list_head list;
} VT_KEY_ACTION_MAP;
/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/
LOCAL int dummyEventHandler(int endpt,VT_EVENT_CONTENT *pEvent);
LOCAL int dummyFailedHandler(int endpt,VT_EVENT_CONTENT *pEvent);
LOCAL void dummyADS(int endpt,char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent);

LOCAL int getEndptByNum(unsigned long long number);
LOCAL int getEndpt(VT_EVENT_CONTENT *pEvent);
LOCAL VT_FSM_CTX *searchFsmCtx(VT_EVENT_TYPE eventType, VT_STATE curState, int ignoreStateMax);

LOCAL void initCallMsg(CALL_MSG *pCallMsg, int endpt, VT_EVENT_TYPE eventType, unsigned long long destNumber);
LOCAL int sendCallMsg(int sockfd, int endpt, VT_EVENT_TYPE eventType, unsigned long long destNumber);

LOCAL void endptADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent);
LOCAL int hookOnCalling(int endpt, VT_EVENT_CONTENT *pEvent);
LOCAL int hookOnTalking(int endpt, VT_EVENT_CONTENT *pEvent);

LOCAL void hookOffBeCalledADS(int endpt, char *pDataOut, size_t len, const VT_EVENT_CONTENT *pEvent);
LOCAL int hookOffBeCalled(int endpt, VT_EVENT_CONTENT *pEvent);

LOCAL int hookFlashTalking(int endpt, VT_EVENT_CONTENT *pEvent);
LOCAL int hookFlashAny(int endpt, VT_EVENT_CONTENT *pEvent);

LOCAL int dfmtStartHookOff(int endpt, VT_EVENT_CONTENT *pEvent);
LOCAL int dfmtEndDtmfStart(int endpt, VT_EVENT_CONTENT *pEvent);

//LOCAL int callNumberFH(int endpt, VT_EVENT_CONTENT *pEvent);
LOCAL int numDoneDtmfStart(int endpt, VT_EVENT_CONTENT *pEvent);
LOCAL int numErrDtmfStart(int endpt, VT_EVENT_CONTENT *pEvent);

LOCAL void beCalledHookOnADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent);
LOCAL int beCalledHookOn(int endpt, VT_EVENT_CONTENT *pEvent);
//LOCAL int callingAny(int endpt, VT_EVENT_CONTENT *pEvent);

LOCAL void callAcceptCallingADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent);
LOCAL int rejectCall(int endpt, VT_EVENT_CONTENT *pEvent);

LOCAL int closeFd(int endpt, VT_EVENT_CONTENT *pEvent);
//LOCAL int callDownTalking(int endpt, VT_EVENT_CONTENT *pEvent);

//LOCAL int callRejectCalling(int endpt, VT_EVENT_CONTENT *pEvent);
LOCAL int runKeyAction(int endpt, char *pNumber);
LOCAL int callNumber(int endpt, char *pNumber);
/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
LOCAL VT_ENDPT_STATUS l_endptsStatus[VT_ENDPT_MAX];

LOCAL VT_FSM_CTX l_fsmCtxs[] =
{
    /*Action is dynamically loaded from the action file.*/
    {VT_EVENT_HOOK_ON, VT_STATE_CALLING, VT_STATE_HOOK_ON, hookOnCalling, dummyFailedHandler, endptADS, VT_ON_HOOK_INX},
    {VT_EVENT_HOOK_ON, VT_STATE_TALKING, VT_STATE_HOOK_ON, hookOnTalking, dummyFailedHandler, endptADS, VT_ON_HOOK_INX}, //todo: create a new action
    /*VT_STATE_MAX will match other states that are different from above*/
    {VT_EVENT_HOOK_ON, VT_STATE_MAX, VT_STATE_HOOK_ON, dummyEventHandler, dummyFailedHandler, endptADS, VT_ON_HOOK_INX},

    {VT_EVENT_HOOK_OFF, VT_STATE_BE_CALLED, VT_STATE_TALKING, hookOffBeCalled, dummyFailedHandler, hookOffBeCalledADS, VT_ACCEPT_CALL_INX},
    {VT_EVENT_HOOK_OFF, VT_STATE_MAX, VT_STATE_HOOK_OFF, dummyEventHandler, dummyFailedHandler, endptADS, VT_OFF_HOOK_INX},

    {VT_EVENT_HOOK_FLASH, VT_STATE_TALKING, VT_STATE_MAX, hookFlashTalking, dummyFailedHandler, endptADS, -1},
    {VT_EVENT_HOOK_FLASH, VT_STATE_MAX, VT_STATE_HOOK_OFF, hookFlashAny, dummyFailedHandler, endptADS, -1},

    {VT_EVENT_DTMF_START, VT_STATE_HOOK_OFF, VT_STATE_DTMF_START, dfmtStartHookOff, dummyFailedHandler, endptADS, -1}, //todo: can create a new action
    {VT_EVENT_DTMF_END, VT_STATE_DTMF_START, VT_STATE_DTMF_START, dfmtEndDtmfStart, dummyFailedHandler, dummyADS, -1},

    {VT_EVENT_NUM_DONE, VT_STATE_DTMF_START, VT_STATE_MAX, numDoneDtmfStart, dummyFailedHandler, endptADS, -1},
    {VT_EVENT_NUM_CALLED, VT_STATE_DTMF_START, VT_STATE_CALLING, dummyEventHandler, dummyFailedHandler, endptADS, VT_CALLING_INX},
    {VT_EVENT_NUM_CALLED, VT_STATE_MAX, VT_STATE_MAX, closeFd, dummyFailedHandler, endptADS, VT_CALLING_FAILED_INX},
    {VT_EVENT_NUM_ERR,  VT_STATE_DTMF_START, VT_STATE_CALLING_FAILED, numErrDtmfStart, dummyFailedHandler, endptADS, VT_CALLING_FAILED_INX},

    {VT_EVENT_CALLING, VT_STATE_HOOK_ON, VT_STATE_BE_CALLED, beCalledHookOn, rejectCall, beCalledHookOnADS, VT_BE_CALLED_INX},
    {VT_EVENT_CALLING, VT_STATE_MAX, VT_STATE_MAX, rejectCall, dummyFailedHandler, dummyADS, -1},

    {VT_EVENT_CALL_ACCEPT, VT_STATE_CALLING, VT_STATE_TALKING, dummyEventHandler, rejectCall, callAcceptCallingADS, VT_TALKING_INX},

    /*When the other side want to down or reject our session, we only need to close the socket fd.*/
    {VT_EVENT_CALL_DOWN, VT_STATE_BE_CALLED, VT_STATE_CALL_DOWN, closeFd, dummyFailedHandler, endptADS, VT_MISS_CALL_INX},
    {VT_EVENT_CALL_DOWN, VT_STATE_TALKING, VT_STATE_CALL_DOWN, closeFd, dummyFailedHandler, endptADS, VT_BE_ON_HOOK_INX},

    {VT_EVENT_CALL_REJECT, VT_STATE_CALLING, VT_STATE_CALL_REJECT, closeFd, dummyFailedHandler, endptADS, VT_CALLING_FAILED_INX},

    VT_FSM_CTX_END
};

LOCAL VT_STATE_STR_MAP l_stateStrMap[] =
{
    STR(VT_STATE_BE_CALLED),
    STR(VT_STATE_CALL_DOWN),
    STR(VT_STATE_CALL_REJECT),
    STR(VT_STATE_CALLING),
    STR(VT_STATE_CALLING_FAILED),
    STR(VT_STATE_DTMF_START),
    STR(VT_STATE_HOOK_OFF),
    STR(VT_STATE_HOOK_ON),
    STR(VT_STATE_TALKING),

    STR(VT_STATE_MAX),
};

LOCAL VT_NUM_HANDLER_MAP l_numHandMap[] =
{
    /*Can use it to convert number by some rule*/
    {"*", runKeyAction},    /*Number begin with * correspond to user specified action*/
    {"", callNumber},   /*Put this at last*/
};

LOCAL VT_DYNAMIC_FSM_CTX l_dynamicFsmHead = {VT_FSM_CTX_END, LIST_HEAD_INIT(l_dynamicFsmHead.list)};

LOCAL VT_KEY_ACTION_MAP l_keyActionMap = {"", -1, LIST_HEAD_INIT(l_keyActionMap.list)};

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL int dummyEventHandler(int endpt, VT_EVENT_CONTENT *pEvent)
{
    return VT_SUCCESS;
}
LOCAL int dummyFailedHandler(int endpt, VT_EVENT_CONTENT *pEvent)
{
    CX_LOGL(CX_ERR, "Process event: %s in state: %s failed.", vt_getEventStr(pEvent->eventType),
            vt_getStateStr(l_endptsStatus[endpt].fxsState));
    return VT_SUCCESS;
}
LOCAL void dummyADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent)
{
    pDataOut[0] = '\0';
}
LOCAL VT_FSM_CTX *searchFsmCtx(VT_EVENT_TYPE eventType, VT_STATE curState, int ignoreStateMax)
{
    int index = 0;

    while (l_fsmCtxs[index].evenType != VT_EVENT_MAX)
    {
        if (eventType == l_fsmCtxs[index].evenType)
        {
            if ( curState == l_fsmCtxs[index].curState)
            {
                break;
            }
            else if (!ignoreStateMax && l_fsmCtxs[index].curState == VT_STATE_MAX)
            {
                break;
            }
        }

        index++;
    }
    return &l_fsmCtxs[index];
}

LOCAL VT_FSM_CTX *searchDynamicFsmCtx(VT_EVENT_TYPE eventType, VT_STATE curState)
{
    VT_DYNAMIC_FSM_CTX *pFsmIndex;

    list_for_each_entry(pFsmIndex, &l_dynamicFsmHead.list, list)
    {
        if (pFsmIndex->fsmCtx.evenType == eventType && pFsmIndex->fsmCtx.curState == curState)
        {
            break;
        }
    }

    return &pFsmIndex->fsmCtx;
}


LOCAL int getEndptByNum(unsigned long long number)
{
    int index;

    for (index = 0; index < VT_ENDPT_MAX; index++)
    {
        if (g_vtConfig.endptInfo[index].number == number)
        {
            return index;
        }
    }
    return VT_FAILED;
}
LOCAL int getEndpt(VT_EVENT_CONTENT *pEvent)
{
    int32_t endpt;

    if (pEvent->eventType < VT_EVENT_CALLING)
    {
        /*DSP Event*/
        memcpy(&endpt, pEvent->pData, sizeof(endpt));
        //endpt = ntohl(endpt);
    }
    else
    {
        VT_CALL_EVENT *pCallEvent = (VT_CALL_EVENT *)(pEvent->pData);
        endpt = getEndptByNum(pCallEvent->callMsg.destNumber);
    }

    return endpt;
}

LOCAL void endptADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent)
{
    snprintf(pDataOut, dataLen, "%d", endpt);
}

LOCAL void initCallMsg(CALL_MSG *pCallMsg, int endpt, VT_EVENT_TYPE eventType, unsigned long long destNumber)
{
    if (eventType >= VT_EVENT_MAX)
    {
        CX_LOGL(CX_ERR, "Error VT_EVENT_TYPE: %s", vt_getEventStr(eventType));
        return ;
    }

    pCallMsg->eventType = eventType;
    pCallMsg->srcIP = g_vtConfig.serverIP;
    pCallMsg->srcPort = g_vtConfig.endptInfo[endpt].port;
    pCallMsg->srcCtrport = g_vtConfig.endptInfo[endpt].ctrlport;
    pCallMsg->srcNumber = g_vtConfig.endptInfo[endpt].number;
    pCallMsg->destNumber = destNumber;
}
LOCAL int sendCallMsg(int sockfd, int endpt, VT_EVENT_TYPE eventType, unsigned long long destNumber)
{
    CALL_MSG callMsg;
    int nwrite;

    initCallMsg(&callMsg, endpt, eventType, destNumber);

    nwrite = sock_tcpWrite(sockfd, &callMsg, sizeof(callMsg));

    CX_LOGL(CX_DBG, "Fd(%d) Send call message: %s with return value: %d. From number: %llu to number: %llu.", sockfd, vt_getEventStr(eventType),
            nwrite, callMsg.srcNumber, callMsg.destNumber);

    return nwrite;
}

LOCAL int hookOnCalling(int endpt, VT_EVENT_CONTENT *pEvent)
{
    int result = sendCallMsg(l_endptsStatus[endpt].sessionFd, endpt, VT_EVENT_CALL_DOWN, l_endptsStatus[endpt].destNumber);

    return result > 0 ? VT_SUCCESS : VT_FAILED;
}
LOCAL int hookOnTalking(int endpt, VT_EVENT_CONTENT *pEvent)
{
    int result = sendCallMsg(l_endptsStatus[endpt].sessionFd, endpt, VT_EVENT_CALL_DOWN, l_endptsStatus[endpt].destNumber);

    vt_sendRtpDeactive(endpt);   /*TODO: can add it into action*/

    return result > 0 ? VT_SUCCESS : VT_FAILED;
}
LOCAL void hookOffBeCalledADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent)
{
    snprintf(pDataOut, dataLen, "%d %x %d %d", endpt, l_endptsStatus[endpt].destIP, l_endptsStatus[endpt].destPort,
                 l_endptsStatus[endpt].destCtrport);
}

LOCAL int hookOffBeCalled(int endpt, VT_EVENT_CONTENT *pEvent)
{
    int result = sendCallMsg(l_endptsStatus[endpt].sessionFd, endpt, VT_EVENT_CALL_ACCEPT, l_endptsStatus[endpt].destNumber);

    return result > 0 ? VT_SUCCESS : VT_FAILED;
}

LOCAL int hookFlashTalking(int endpt, VT_EVENT_CONTENT *pEvent)
{
    return VT_SUCCESS;
}
LOCAL int hookFlashAny(int endpt, VT_EVENT_CONTENT *pEvent)
{
    vt_putEvent(VT_EVENT_HOOK_ON, pEvent->pData, pEvent->dataLen);
    vt_putEvent(VT_EVENT_HOOK_OFF, pEvent->pData, pEvent->dataLen);

    return VT_SUCCESS;
}

LOCAL int dfmtStartHookOff(int endpt, VT_EVENT_CONTENT *pEvent)
{
    //pthread_mutex_lock(&l_endptsStatus[endpt].inputNumber.mutex);
    l_endptsStatus[endpt].inputNumber.numberIndex = 0;
    l_endptsStatus[endpt].inputNumber.isDone = 0;
    //pthread_mutex_unlock(&l_endptsStatus[endpt].inputNumber.mutex);
    vt_sendTonePlay(endpt, 0, 200);

    return VT_SUCCESS;
}
LOCAL int numberAvailable(VT_INPUT_NUM *pInputNum)
{
    VT_KEY_ACTION_MAP *pKeyActionIndex;
    int len;

    list_for_each_entry(pKeyActionIndex, &l_keyActionMap.list, list)
    {
        len = strlen(pKeyActionIndex->pKey);
        if (strncasecmp(pKeyActionIndex->pKey, pInputNum->number, len) == 0)
        {
            return 1;
        }
    }

    return str_isDigit(pInputNum->number);
}
LOCAL void digitTimeoutHandler(int endpt)
{
    CX_LOGL(CX_DBG, "Input digit timeout. Put event: VT_EVENT_NUM_DONE");

    if (numberAvailable(&l_endptsStatus[endpt].inputNumber))
    {
        vt_putEvent(VT_EVENT_NUM_DONE, &endpt, sizeof(endpt));
    }
    else
    {
        CX_LOGL(CX_ERR, "Number: %s illegal!", l_endptsStatus[endpt].inputNumber.number);
        vt_putEvent(VT_EVENT_NUM_ERR, &endpt, sizeof(endpt));
    }

    l_endptsStatus[endpt].digitTimerId = -1;
}
LOCAL int dfmtEndDtmfStart(int endpt, VT_EVENT_CONTENT *pEvent)
{
    //pthread_mutex_lock(&l_endptsStatus[endpt].inputNumber.mutex);

    int numberIndex = l_endptsStatus[endpt].inputNumber.numberIndex;
    VOIP_DTMF_MSG *pDtmfMsg = (VOIP_DTMF_MSG *)pEvent->pData;

    if (numberIndex >= VT_INPUT_NUM_MAX)
    {
        CX_LOGL(CX_ERR, "Input number too much. At most %d char.", VT_INPUT_NUM_MAX);
        //pthread_mutex_unlock(&l_endptsStatus[endpt].inputNumber.mutex);
        return VT_FAILED;
    }

    if (l_endptsStatus[endpt].inputNumber.isDone)
    {
        CX_LOGL(CX_DBG, "Input number done. Ignore this event: %s", vt_getEventStr(pEvent->eventType));
        //pthread_mutex_unlock(&l_endptsStatus[endpt].inputNumber.mutex);
        return VT_FAILED;
    }

    if (pDtmfMsg->dtmf <= 9)
    {
        l_endptsStatus[endpt].inputNumber.number[numberIndex] = '0' + pDtmfMsg->dtmf;
    }
    else if (pDtmfMsg->dtmf == 10)
    {
        l_endptsStatus[endpt].inputNumber.number[numberIndex]  = '*';
    }
    else if (pDtmfMsg->dtmf == 11)
    {
        l_endptsStatus[endpt].inputNumber.number[numberIndex] = '#';
    }
    else
    {
        CX_LOGL(CX_ERR, "Error dtmf digit: %d.", pDtmfMsg->dtmf);
    }

    ++(l_endptsStatus[endpt].inputNumber.numberIndex);

    //pthread_mutex_unlock(&l_endptsStatus[endpt].inputNumber.mutex);

    if (l_endptsStatus[endpt].digitTimerId < 0)
    {
        l_endptsStatus[endpt].digitTimerId = vt_registerTimer(&VT_DIGIT_TIMEOUT, digitTimeoutHandler, endpt);
    }
    else
    {
        vt_updateTimer(l_endptsStatus[endpt].digitTimerId, &VT_DIGIT_TIMEOUT);
    }

    return VT_SUCCESS;
}

LOCAL void getIPPortByNum(unsigned long long number, int32_t *pIp, int16_t *pPort)
{
    int index;

    for (index = 0; index < VT_ENDPT_MAX; index++)
    {
        if (g_vtConfig.endptInfo[index].number == number)
        {
            *pIp = CLIENT_IP;
            *pPort = g_vtConfig.clientListenPort;
            return ;
        }
    }

    /*remote number*/
    *pIp = g_vtConfig.outerClientIP;
    *pPort = g_vtConfig.outerClientListenPort;
}
LOCAL int runKeyAction(int endpt, char *pNumber)
{
    VT_KEY_ACTION_MAP *pKeyActionIdx;
    char endptStr[ENDPT_STRLEN_MAX];
    CX_LOGL(CX_DBG, "run key action");

    snprintf(endptStr, ENDPT_STRLEN_MAX, "%d", endpt);

    list_for_each_entry(pKeyActionIdx, &l_keyActionMap.list, list)
    {
        if (strcmp(pNumber, pKeyActionIdx->pKey) == 0)
        {
            return vt_runAction(pKeyActionIdx->actionIndex, endptStr);
        }
    }

    CX_LOGL(CX_DBG, "Cannot match number: %s in key action map.", pNumber);
    return VT_FAILED;
}

LOCAL int callNumber(int endpt, char *pNumber)
{
    int32_t ip;
    int16_t port;
    unsigned long long number = strtoll(pNumber, NULL, 10);

    getIPPortByNum(number, &ip, &port);

    int fd;
    int nwrite;

    CX_LOGL(CX_DBG, "Connect to %x in port %d to call %llu", ip, port, number);
    if ((fd = sock_tcpConnect(NULL, ip, port)) > 0)
    {
        vt_registerSessionFd(fd);

        l_endptsStatus[endpt].sessionFd = fd;
        l_endptsStatus[endpt].destNumber = number;
        nwrite = sendCallMsg(fd, endpt, VT_EVENT_CALLING, number);
        CX_LOGL(CX_DBG, "Write VT_EVENT_CALLING to fd: %d. nwrite=%d", fd, nwrite);

        if (nwrite > 0)
        {
            vt_putEvent(VT_EVENT_NUM_CALLED, &endpt, sizeof(endpt));
            return VT_SUCCESS;
        }
    }

    vt_putEvent(VT_EVENT_NUM_ERR, &endpt, sizeof(endpt));
    return VT_FAILED;
}
//LOCAL int callNumberFH(int endpt, VT_EVENT_CONTENT *pEvent)
//{
//
//    return VT_SUCCESS;
//}

LOCAL NUM_HANDLER getNumHandler(char *pNumber)
{
    VT_NUM_HANDLER_MAP *numHandIndex = l_numHandMap;

    while (numHandIndex->pNumber[0] != '\0')
    {
        if (strncasecmp(pNumber, numHandIndex->pNumber, strlen(numHandIndex->pNumber)) == 0)
        {
            break;
        }

        numHandIndex++;
    }
    return numHandIndex->numHandler;
}
LOCAL int numDoneDtmfStart(int endpt, VT_EVENT_CONTENT *pEvent)
{
    //getIPPortByNum(l_endptsStatus[endpt].)
    char *pNumber = l_endptsStatus[endpt].inputNumber.number;

    NUM_HANDLER numHandler = getNumHandler(pNumber);

    return numHandler(endpt, pNumber);
}
LOCAL int numErrDtmfStart(int endpt, VT_EVENT_CONTENT *pEvent)
{
    return VT_SUCCESS;
}

LOCAL void beCalledHookOnADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent)
{
    VT_CALL_EVENT *pCallEvent = (VT_CALL_EVENT *)pEvent->pData;

    snprintf(pDataOut, dataLen, "%d %llu", endpt, pCallEvent->callMsg.srcNumber);
}

LOCAL int beCalledHookOn(int endpt, VT_EVENT_CONTENT *pEvent)
{
    VT_CALL_EVENT *pCallEvent = (VT_CALL_EVENT *)pEvent->pData;

    l_endptsStatus[endpt].sessionFd = pCallEvent->fd;

    l_endptsStatus[endpt].destIP = pCallEvent->callMsg.srcIP;
    l_endptsStatus[endpt].destPort = pCallEvent->callMsg.srcPort;
    l_endptsStatus[endpt].destCtrport = pCallEvent->callMsg.srcCtrport;
    l_endptsStatus[endpt].destNumber = pCallEvent->callMsg.srcNumber;

    return VT_SUCCESS;
}
//LOCAL int callingAny(int endpt, VT_EVENT_CONTENT *pEvent)
//{
//    VT_CALL_EVENT *pCallEvent = (VT_CALL_EVENT *)pEvent->pData;
//
//    //l_endptsStatus[endpt].sessionFd = pCallEvent->fd;
//
//    return sendCallMsg(pCallEvent->fd, endpt, VT_EVENT_CALL_REJECT) > 0 ? VT_SUCCESS : VT_FAILED;
//}

LOCAL void callAcceptCallingADS(int endpt, char *pDataOut, size_t dataLen, const VT_EVENT_CONTENT *pEvent)
{
    CALL_MSG *pCallMsg = &((VT_CALL_EVENT *)pEvent->pData)->callMsg;

    snprintf(pDataOut, dataLen, "%d %x %d %d", endpt, pCallMsg->srcIP, pCallMsg->srcPort, pCallMsg->srcCtrport);
}
LOCAL int rejectCall(int endpt, VT_EVENT_CONTENT *pEvent)
{
    VT_CALL_EVENT *pCallEvent = (VT_CALL_EVENT *)(pEvent->pData);

    int nread = sendCallMsg(pCallEvent->fd, endpt, VT_EVENT_CALL_REJECT, pCallEvent->callMsg.srcNumber);

    return nread > 0 ? VT_SUCCESS : VT_FAILED;
}

LOCAL int closeFd(int endpt, VT_EVENT_CONTENT *pEvent)
{
    VT_CALL_EVENT *pCallEvent = (VT_CALL_EVENT *)pEvent->pData;

    vt_unregisterSessionFd(pCallEvent->fd);
    shutdown(pCallEvent->fd, SHUT_RDWR);

    return VT_SUCCESS;
}
//LOCAL int callDownTalking(int endpt, VT_EVENT_CONTENT *pEvent)
//{
//    return VT_SUCCESS;
//}
//
//LOCAL int callRejectCalling(int endpt, VT_EVENT_CONTENT *pEvent)
//{
//    return VT_SUCCESS;
//}

LOCAL void initSessionStatus()
{
    int index;

    //sem_init(&g_connectedSem, 0, 0);

    for (index = 0; index < VT_ENDPT_MAX; index++)
    {
        l_endptsStatus[index].inputNumber = (VT_INPUT_NUM){"", 0, 0/*, PTHREAD_MUTEX_INITIALIZER*/};
        l_endptsStatus[index].openChannel = -1;
        l_endptsStatus[index].fxsState = VT_STATE_HOOK_ON;
        l_endptsStatus[index].destIP = 0;
        l_endptsStatus[index].destPort = -1;
        l_endptsStatus[index].destCtrport = -1;
        l_endptsStatus[index].sessionFd = -1;
        l_endptsStatus[index].digitTimerId = -1;
    }
}

LOCAL void vt_initFsm()
{
    initSessionStatus();
}

LOCAL int fsmGo(VT_FSM_CTX *pFsmCtx, int endpt, VT_EVENT_CONTENT *pEventContent, int curState)
{
    char actionBuf[BUFF_MAX];

    if (pFsmCtx->evenType == VT_EVENT_MAX)
    {
        return VT_FAILED;
    }
    else
    {
        CX_LOGL(CX_INFO, "Handle event: %s from state: %s to new state: %s.", vt_getEventStr(pEventContent->eventType),
        vt_getStateStr(curState), vt_getStateStr(pFsmCtx->newState));
        pFsmCtx->actionDataSeter(endpt, actionBuf, sizeof(actionBuf), pEventContent);

        if (vt_runAction(pFsmCtx->actionIndex, actionBuf) == VT_SUCCESS)
        {
            if (pFsmCtx->eventHandler(endpt, pEventContent) != VT_SUCCESS)
            {
                pFsmCtx->failedHandler(endpt, pEventContent);
            }
            else
                if (pFsmCtx->newState != VT_STATE_MAX)
                {
                    /*Only if vt_runAction and eventHandler executes successfully, can we change the state.*/
                    /*NOTE: When calling a number, it may fails because of network fault while we will put a new event: VT_EVENT_NUM_ERR into
                            event queue. So if we change the state in this case, VT_EVENT_NUM_ERR will be ignored when processing it.*/
                    l_endptsStatus[endpt].fxsState = pFsmCtx->newState;


                }
        }
        else
        {
            pFsmCtx->failedHandler(endpt, pEventContent);
        }
        return VT_SUCCESS;
    }
}
LOCAL void updateActionIdxByEvent(VT_EVENT_TYPE eventType, int actionIndex)
{
    VT_FSM_CTX *pFsmCtxIndex = l_fsmCtxs;

    CX_LOGL(CX_DBG, "Update the action of all fsm record with same event %s to action %s.", vt_getEventStr(eventType),
                    vt_getActionStr(actionIndex));

    while (pFsmCtxIndex->evenType != VT_EVENT_MAX)
    {
        if (pFsmCtxIndex->evenType == eventType)
        {
            pFsmCtxIndex->actionIndex = actionIndex;
        }
        pFsmCtxIndex++;
    }
}
/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
const char *vt_getStateStr(VT_STATE state)
{
    VT_STATE_STR_MAP *pStateStrMap = l_stateStrMap;

    while (pStateStrMap->state != VT_STATE_MAX)
    {
        if (pStateStrMap->state == state)
        {
            break;
        }

        pStateStrMap++;
    }

    return pStateStrMap->pStateStr;
}
VT_STATE vt_stateStrToInt(const char *pStateStr)
{
    if (pStateStr == NULL)
    {
        return VT_STATE_MAX;
    }

    VT_STATE_STR_MAP *pStateStrMap = l_stateStrMap;

    while (pStateStrMap->state != VT_STATE_MAX)
    {
        if (strcmp(pStateStrMap->pStateStr, pStateStr) == 0)
        {
            break;
        }

        pStateStrMap++;
    }

    return pStateStrMap->state;
}

void vt_registerAction(VT_EVENT_TYPE eventType, VT_STATE curState, int actionIndex)
{
    if (curState == VT_STATE_MAX)
    {
        updateActionIdxByEvent(eventType, actionIndex);
        return ;
    }

    VT_FSM_CTX *pFsmCtx = searchFsmCtx(eventType, curState, 1);

    if (pFsmCtx->evenType != VT_EVENT_MAX)
    {
        CX_LOGL(CX_DBG, "Update action to %s in static fsm table(%s %s).", vt_getActionStr(actionIndex),
                vt_getEventStr(eventType), vt_getStateStr(curState));
        pFsmCtx->actionIndex = actionIndex;
    }
    else
    {
        VT_DYNAMIC_FSM_CTX *pNewFsmCtx = calloc(1, sizeof(VT_DYNAMIC_FSM_CTX));

        CX_LOGL(CX_DBG, "Insert a new node(%s %s %s) into dynamic fsm table.", vt_getActionStr(actionIndex),
                vt_getEventStr(eventType), vt_getStateStr(curState));
        pNewFsmCtx->fsmCtx.evenType = eventType;
        pNewFsmCtx->fsmCtx.curState = curState;
        pNewFsmCtx->fsmCtx.newState = VT_STATE_MAX;
        pNewFsmCtx->fsmCtx.eventHandler = dummyEventHandler;
        pNewFsmCtx->fsmCtx.failedHandler = dummyFailedHandler;
        pNewFsmCtx->fsmCtx.actionDataSeter = endptADS;
        pNewFsmCtx->fsmCtx.actionIndex = actionIndex;
        list_add(&pNewFsmCtx->list, &l_dynamicFsmHead.list);
    }
}

void vt_registerKeyAction(const char *pKey, int actionIndex)
{
    VT_KEY_ACTION_MAP *pNewKeyAction = calloc(1, sizeof(VT_KEY_ACTION_MAP));

    CX_LOGL(CX_DBG, "Register key: %s with action %d", pKey, actionIndex);

    pNewKeyAction->pKey = pKey;
    pNewKeyAction->actionIndex = actionIndex;
    list_add(&pNewKeyAction->list, &l_keyActionMap.list);
}

void *vt_processEvent(void *pArg)
{
    VT_EVENT_CONTENT eventContent;
    VT_FSM_CTX *pFsmCtx = NULL;
    VT_STATE curState;
    int endpt;
    struct timeval latestTime;
    int isTimeout;
    int fsmGoResult = 0;

    vt_initFsm();

    while (1)
    {
        fsmGoResult = 0;
        vt_getLatestTimer(&latestTime);

        bzero(&eventContent, sizeof(eventContent));
        isTimeout = vt_getEventUntilTime(&eventContent, &latestTime);

        vt_notifyTimer();
        if (isTimeout)
        {
            continue;
        }

        endpt = getEndpt(&eventContent);

        if (endpt < 0)
        {
            CX_LOGL(CX_ERR, "endpt incorrect: %d", endpt);
            continue;
        }

        curState = l_endptsStatus[endpt].fxsState;

        pFsmCtx = searchDynamicFsmCtx(eventContent.eventType, curState);
        fsmGoResult += fsmGo(pFsmCtx, endpt, &eventContent, curState);

        pFsmCtx = searchFsmCtx(eventContent.eventType, curState, 0);
        fsmGoResult += fsmGo(pFsmCtx, endpt, &eventContent, curState);

        if (fsmGoResult <= VT_FAILED * 2)
        {
            CX_LOGL(CX_ERR, "Cannot match event: %s in state: %s", vt_getEventStr(eventContent.eventType),
                                                                   vt_getStateStr(curState));
        }
        vt_freeEvent(&eventContent);
    }

}

int vt_msgSession()
{
    int index = 0;

    for (; index < VT_ENDPT_MAX; index++)
    {
        printf("Endpt: %d\n", index);
        printf("\tconnect fd: %d\n", l_endptsStatus[index].sessionFd);
        printf("\tcall number: %s\n", l_endptsStatus[index].inputNumber.number);
        printf("\tdest number: %llu\n", l_endptsStatus[index].destNumber);
        printf("\tdest ip: %x\n", l_endptsStatus[index].destIP);
        printf("\tdest port: %d\n", l_endptsStatus[index].destPort);
        printf("\tdest control port: %d\n", l_endptsStatus[index].destCtrport);
        printf("\tfxs state: %s\n", vt_cmTxtMapById(l_endptsStatus[index].fxsState));
        printf("\topen channel: %d\n", l_endptsStatus[index].openChannel);

    }
    return VT_SUCCESS;
}
/*below function has no use yet.*/
void vt_setOpenChannel(int endpt, int channel)
{
    if (channel < 0 || channel > MAX_CNX)
    {
        CX_LOGL(CX_ERR, "channel %d no correct.", channel);
        return ;
    }
    if (endpt < 0 || endpt > MAX_ENDPT)
    {
        CX_LOGL(CX_ERR, "endpt %d no correct.", endpt);
        return ;
    }

    l_endptsStatus[endpt].openChannel = channel;
}
int vt_getOpenChannel(int endpt)
{
    if (endpt < 0 || endpt > MAX_ENDPT)
    {
        CX_LOGL(CX_ERR, "endpt %d no correct.", endpt);
        return VT_FAILED;
    }
    return l_endptsStatus[endpt].openChannel;
}

int vt_getState(int endpt)
{
    return l_endptsStatus[endpt].fxsState;
}

