/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_action.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		15Oct14
 *
 * history 	\arg
 */
#ifndef __VT_ACTION_H__
#define __VT_ACTION_H__

#include <base/voip_comm.h>
/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define VT_ACCEPT_CALL_INX  0
#define VT_BE_CALLED_INX    1
#define VT_BE_ON_HOOK_INX   2
#define VT_CALLING_INX      3
#define VT_CALLING_FAILED_INX   4
#define VT_MISS_CALL_INX    5
#define VT_OFF_HOOK_INX     6
#define VT_ON_HOOK_INX      7
#define VT_TALKING_INX      8

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/

int vt_actionInitServer(const char *pData);
const char *vt_getActionStr(int actionIndex);
int vt_getActionIdxByStr(const char *pData);
void vt_resetDebugMode();
void vt_cliWaiting();
void vt_initDebug();
void vt_exitDebug();
void vt_allbreak();
void vt_breakAt(VOIP_DSPMSG_CMD cmd);
void vt_unbreakAt(VOIP_DSPMSG_CMD cmd);
void vt_next();
void vt_run();

int vt_loadAction(const char *pActionFilePath);
int vt_runAction(int actionIndex, const char *pData);
#endif // __VT_ACTION_H__
