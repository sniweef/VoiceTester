/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_fsm.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		18Nov14
 *
 * history 	\arg
 */

#ifndef __VT_FSM_H__
#define __VT_FSM_H__


/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef enum _VT_STATE
{
    VT_STATE_BE_CALLED = 0,
    VT_STATE_CALL_DOWN,
    VT_STATE_CALL_REJECT,
    VT_STATE_CALLING,
    VT_STATE_CALLING_FAILED,
    VT_STATE_DTMF_START,
    VT_STATE_HOOK_OFF,
    VT_STATE_HOOK_ON,
    VT_STATE_TALKING,

    VT_STATE_MAX,
    /*VOIP_CALLING = VOIP_DSPMSG_CMDMAX + 1,
    VOIP_BECALLED,
    VOIP_TALKING,
    VOIP_CALL_DOWN,
    VOIP_CALL_REJECT,
    VOIP_CALL_FAILED,*/
} VT_STATE;
/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
const char *vt_getStateStr(VT_STATE state);
VT_STATE vt_stateStrToInt(const char *pStateStr);
void vt_registerKeyAction(const char *pKey, int actionIndex);
void vt_registerAction(VT_EVENT_TYPE eventType, VT_STATE curState, int actionIndex);
void *vt_processEvent(void *pArg);
int vt_msgSession();
void vt_setOpenChannel(int endpt, int channel);
int vt_getOpenChannel(int endpt);
#endif // __VT_FSM_H__
