/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_event.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		15Oct14
 *
 * history 	\arg
 */

#ifndef __vt_event_H__
#define __vt_event_H__


#include <base/common.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef enum _VT_EVENT_TYPE
{
    VT_EVENT_SLIC_LOCALE    = VOIP_SLIC_LOCALE,
    VT_EVENT_DSP_START      = VOIP_DSP_START,
    VT_EVENT_HOOK_ON        = VOIP_HOOK_ON,
    VT_EVENT_HOOK_OFF       = VOIP_HOOK_OFF,
    VT_EVENT_HOOK_FLASH     = VOIP_HOOK_FLASH,
    VT_EVENT_DTMF_START     = VOIP_DTMF_START,
    VT_EVENT_DTMF_END       = VOIP_DTMF_END,

    VT_EVENT_NUM_DONE       = VOIP_DSPMSG_CMDMAX + 1, /*User has done inputing number.*/
    VT_EVENT_NUM_CALLED,
    VT_EVENT_NUM_ERR,        /*User input wrong number.*/

    VT_EVENT_CALLING,
    VT_EVENT_CALL_ACCEPT,
    VT_EVENT_CALL_DOWN,
    VT_EVENT_CALL_REJECT,
    VT_EVENT_MAX,
} VT_EVENT_TYPE;

typedef enum _CALL_MSG_TYPE
{
    CALL_NUMBER = 0,
    CALL_ACCEPT = 1,
    CALL_REJECT = 2,
    CALL_DOWN   = 3,
    CALL_MSG_MAX,
} CALL_MSG_TYPE;

//todo: htonl ntol
typedef struct _CALL_MSG
{
    //CALL_MSG_TYPE       callMsgType;
    VT_EVENT_TYPE       eventType;
    uint32_t            srcIP;
    uint16_t            srcPort;
    uint16_t            srcCtrport;
    unsigned long long  srcNumber;
    unsigned long long  destNumber;
} CALL_MSG;

typedef struct _ENDPT_MSG
{
    //int endpt;
    uint16_t port;
    uint16_t ctrlport;
    unsigned long long number;
} ENDPT_MSG;

typedef struct _VT_CALL_EVENT
{
    CALL_MSG callMsg;
    int fd;
} VT_CALL_EVENT;

typedef struct _VT_EVENT_CONTENT
{
    VT_EVENT_TYPE eventType;
   // char buf[BUFLEN_VT_EVENT];
    void *pData;
    size_t dataLen;
} VT_EVENT_CONTENT;
/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
void *vt_listenEvent(void *pArg);
void vt_registerSessionFd(int fd);
void vt_unregisterSessionFd(int fd);
const char *vt_getEventStr(VT_EVENT_TYPE eventType);
VT_EVENT_TYPE vt_eventStrToInt(const char *pEventStr);
void vt_freeEvent(VT_EVENT_CONTENT *pEvent);
#endif // __vt_event_H__
