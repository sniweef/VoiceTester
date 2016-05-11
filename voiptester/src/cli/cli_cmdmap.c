/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		cli_cmdmap.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		10Oct14
 *
 * history 	\arg
 */
#include <stdlib.h>

#include <base/common.h>
#include <cli/cli_cmdmap.h>
#include <cli/cli_cmdHandler.h>
#include <vt/vt_action.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define CMD_MAP_END    {NULL, NULL, NULL, NULL, NULL, NULL}
#define CMD_ARG_END    {NULL , NULL, NULL, NULL}

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
LOCAL CMD_ARG_T l_localeArgs[] =
{
    {"AU" , "", "Australia", NULL},
    {"BE" , "", "Beligium", NULL},
    {"CN" , "", "China", NULL},
    {"US" , "", "North America", NULL},
    CMD_ARG_END
};


LOCAL CMD_ARG_T l_actionArgMapDef[] =
{
    {"AcceptCall", "ac", "action AcceptCall endpt [remoteIP remotePort remoteCtrlPort].", NULL},
    {"BeCalled", "bc", "action BeCalled endpt number.", NULL},
    {"BeOnHookCall", "bo", "action BeOnHookCall endpt.", NULL},
    {"Calling", "c", "action Calling endpt.", NULL},
    {"CallingFailed", "cf", "action CallingFailed endpt.", NULL},
    {"InitServer", "is", "action InitServer locale.", NULL},
    {"MissCall", "mc", "action MissCall endpt.", NULL},
    {"OffHook", "oh", "action OffHook endpt.", NULL},
    {"OnHook", "oo", "action OnHook endpt.", NULL},
    {"Talking", "t", "action Talking endpt [remoteIP remotePort remoteCtrlPort].", NULL},
    CMD_ARG_END
};

LOCAL CMD_MAP_T l_msgSubcmdMap[] =
{
    {"all", "", "msg all.", cli_msgAllHandler, NULL, NULL},
    {"clear", "", "msg clear.", cli_msgClearHandler, NULL, NULL},
    {"new", "", "msg new.", cli_msgNewHandler, NULL, NULL},
    {"refresh", "", "msg refresh.", cli_msgRefreshHandler, NULL, NULL},
    {"session", "", "msg session.", cli_msgSessionHandler, NULL, NULL},
    CMD_MAP_END
};

LOCAL CMD_MAP_T l_sendSubcmdMap[] =
{
    {"VOIP_CHANNEL_DEFAULT", "cd", "send VOIP_CHANNEL_DEFAULT channel.", cli_sendChannelDefaultHandler, NULL, NULL},
    {"VOIP_CHANNEL_OPEN", "co", "send VOIP_CHANNEL_OPEN endpt channel vadEnable gainSpeaker gainMic intt38Enable.", cli_sendChannelOpenHandler, NULL, NULL},

    {"VOIP_CID_PLAY", "cp", "send VOIP_CID_PLAY endpt number name.", cli_sendCidPlayHandler, NULL, NULL},

    {"VOIP_DSP_DEFAULT", "dd", "send VOIP_DSP_DEFAULT locale. locale argument is required.", cli_sendDspDefaultHandler, NULL, l_localeArgs},
    {"VOIP_DSP_START", "ds", "send VOIP_DSP_START.", cli_sendDspStartHandler, NULL, NULL},
    {"VOIP_DSP_STOP", "dt", "send VOIP_DSP_STOP.", cli_sendDspStopHandler, NULL, NULL},

    {"VOIP_RING_START", "rs", "send VOIP_RING_START endpt type.", cli_sendRingStartHandler, NULL, NULL},
    {"VOIP_RING_STOP", "rt", "send VOIP_RING_STOP endpt.", cli_sendRingStopHandler, NULL, NULL},

    {"VOIP_RTP_ACTIVE", "ra", "send VOIP_RTP_ACTIVE [-d direction -D dtmfrelay -p payloadtype -P ptime] endpt [remoteIP remotePort remoteCtrlPort].",
                                                            cli_sendRtpActiveHandler, NULL, NULL},
    {"VOIP_RTP_DEACTIVE", "rd", "send VOIP_RTP_DEACTIVE endpt.", cli_sendRtpDeactiveHandler, NULL, NULL},

    {"VOIP_SLIC_CONTROL", "sc", "send VOIP_SLIC_CONTROL endpt ctrl.", cli_sendSlicControlHandler, NULL, NULL},
    {"VOIP_SLIC_LOCALE", "sl", "send VOIP_SLIC_LOCALE locale.", cli_sendSlicLocaleHandler, NULL, l_localeArgs},

    {"VOIP_TONE_PLAY", "tp", "send VOIP_TONE_PLAY endpt tone time.", cli_sendTonePlayHandler, NULL, NULL},

    CMD_MAP_END
};

CMD_MAP_T g_topCmdMap[] =
{
    {"action", "", "action command [arguments]", cli_actionHandler, NULL, l_actionArgMapDef},
    {"connect", "", "connect [IP [port]]. IP and port is optional. Only IP is supported.", cli_connectHandler, NULL, NULL},
    {"debug", "", "debug docs", cli_debugHandler, NULL, NULL},
    {"msg", "", "msg [all/clear/new/refresh].", cli_msgAllHandler, l_msgSubcmdMap, NULL},
    {"reaction", "re", "reload action.", cli_reactionHandler, NULL, NULL},
    {"reload", "", "reload filename", cli_reloadHandler, NULL, NULL},
    {"send", "", "send command [arguments]", cli_sendHandler, l_sendSubcmdMap, NULL},
    {"shutdown", "", "shutdown", cli_shutdownHandler, NULL, NULL},
    CMD_MAP_END
};


LOCAL CMD_ARG_T l_breakArgs[] =
{
    {"VOIP_CHANNEL_DEFAULT", "cd", "VOIP_CHANNEL_DEFAULT", NULL},
    {"VOIP_CHANNEL_OPEN", "co", "VOIP_CHANNEL_OPEN", NULL},

    {"VOIP_CID_PLAY", "cp", "VOIP_CID_PLAY", NULL},

    {"VOIP_DSP_DEFAULT", "dd", "VOIP_DSP_DEFAULT", NULL},
    {"VOIP_DSP_START", "ds", "VOIP_DSP_START", NULL},
    {"VOIP_DSP_STOP", "dt", "VOIP_DSP_STOP", NULL},

    {"VOIP_HOOK_FLASH", "hg", "VOIP_HOOK_FLASH", NULL},
    {"VOIP_HOOK_OFF", "ho", "VOIP_HOOK_OFF", NULL},
    {"VOIP_HOOK_ON", "hn", "VOIP_HOOK_ON", NULL},

    {"VOIP_RING_START", "rs", "VOIP_RING_START", NULL},
    {"VOIP_RING_STOP", "rt", "VOIP_RING_STOP", NULL},

    {"VOIP_RTP_ACTIVE", "ra", "VOIP_RTP_ACTIVE", NULL},
    {"VOIP_RTP_DEACTIVE", "rd", "VOIP_RTP_DEACTIVE",NULL},

    {"VOIP_SLIC_CONTROL", "sc", "VOIP_SLIC_CONTROL", NULL},
    {"VOIP_SLIC_LOCALE", "sl", "VOIP_SLIC_LOCALE", NULL},

    {"VOIP_TONE_PLAY", "tp", "VOIP_TONE_PLAY", NULL},

    CMD_ARG_END
};



CMD_MAP_T g_debugCmdMap[] =
{
    {"action", "", "action command [arguments]", cli_actionHandler, NULL, l_actionArgMapDef},
    {"allbreak", "", "allbreak", (CMD_HANDLER)vt_allbreak, NULL, NULL},
    {"break", "", "break VOIP_DSP_CMD", cli_breakHandler, NULL, l_breakArgs},
    {"exit", "", "eixt", cli_exitHandler, NULL, NULL},
    //{"exit", "exit", NULL, NULL, NULL},
    {"msg", "", "msg [all/clear/new/refresh/session]", cli_msgAllHandler, l_msgSubcmdMap, NULL},
    {"next", "", "next", (CMD_HANDLER)vt_next, NULL, NULL},
    {"run", "", "run", (CMD_HANDLER)vt_run, NULL, NULL},
    {"send", "", "send command [arguments]", cli_sendHandler, l_sendSubcmdMap, NULL},
    {"unbreak", "", "unbreak", cli_unbreakHandler, NULL, l_breakArgs},
    CMD_MAP_END
};
/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/


/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
/*TODO: Below 2 function want to implement dynamically load action arguments from .vc_actionfile.*/
/*      However, after do this, the pDocs and pShortcut is set to none which has bad user experience.*/
/*      They can be defined in .vc_action file.*/
void cli_initActionArgs(int actionArgc)
{
    /*if (l_actionArgMap != NULL && l_actionArgMap != l_actionArgMapDef)
    {
        free(l_actionArgMap);
    }*/
    /*CMD_ARG_T *pNewArgs;
    pNewArgs = g_topCmdMap[0].pSupportedArgs = calloc(actionArgc+1, sizeof(CMD_ARG_T));
    pNewArgs[actionArgc] = (CMD_ARG_T)CMD_ARG_END;

    g_debugCmdMap[0].pSupportedArgs = pNewArgs;*/
}
void cli_addActionArg(int argIndex, char *pArgName)
{
    /*CMD_ARG_T *pActionArgs = g_topCmdMap[0].pSupportedArgs;

    pActionArgs[argIndex].pName = pArgName;
    pActionArgs[argIndex].pDocs = NULL;
    pActionArgs[argIndex].pShortcut = "";
    pActionArgs[argIndex].pSubArgs = NULL;*/
}
