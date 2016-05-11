/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file     vt_action.c
 * brief
 * details
 * NOTE
 *
 * author   Huang Zhigeng
 * version
 * date     15Oct14
 *
 * history  \arg
 */
#include <semaphore.h>
#include <iniparser.h>

#include <cli/cli_cmdHandler.h>
#include <cli/cli_cmdmap.h>

#include <base/common.h>
#include <base/voip_comm.h>
#include <vt/vt_send.h>
#include <vt/vt_action.h>
#include <vt/vt_config.h>
#include <vt/vt_fsm.h>
#include <vt/vt_event.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define DBG_ACTION(actionName, pData)   CX_LOGL(CX_DBG, "Running action of " actionName "(Data: %s)...", pData ? pData : "NULL")
#define ACTION_END       {VOIP_DSPMSG_CMDMAX, NULL, NULL, NULL}
#define ACTION_STR_END  {NULL, NULL}
#define ACTION_NUM          9

#define CMD_NAME_IN_CFG     "CmdType"
#define DATA_NAME_IN_CFG    "StaticData"
#define EVENT_NAME_IN_CFG   "Event"
#define CUR_STATE_IN_CFG    "CurState"
#define KEY_ACTION_PREFIX   "Key"

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef int (*HANDLER)(const char * pData);

typedef struct _VT_ACTION_E
{
    VOIP_DSPMSG_CMD cmdType;
    //int (*fpFunc)(const char * pData);      /*pData = pInfoNeed + pExtra*/
    HANDLER fpFunc;
    const char * pInfoNeed;                 /*Most handler need endpt argument.*/
    const char * pExtra;
} VT_ACTION_E;

typedef VT_ACTION_E * VT_ACTION;

typedef enum _DBG_MODE
{
    NO_DBG = 0,
    BREAK_DBG,  /*Run to next breakpoint*/
    STEP_DBG,   /*Run to next message*/
} DBG_MODE;

typedef struct _ACTION_CLI_SYNC /*Hold some variable which help synchronization between action and debug CLI */
{
    pthread_mutex_t mutex;
    sem_t actionSem;
    sem_t cliSem;
    int waitingActionNumber;
    int waitCliNumber;
} ACTION_CLI_SYNC;
//
//typedef struct _VT_ACTION_T
//{
//    VT_ACTION *pAction;
//    const char **pActionsName;
//    int nactions;
//} VT_ACTION;
/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/
LOCAL int sendAllChannelDefault(const char * pData);
LOCAL int actionGo(VT_ACTION_E *action);
LOCAL inline void prepareHandlerData(VT_ACTION_E *action, char * pEndpt, const char * pData);
LOCAL BOOL isBreakpoint(VOIP_DSPMSG_CMD cmd);

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
//sem_t g_cmdDoneSem; /*Use it to wake up the debug CLI.*/
LOCAL ACTION_CLI_SYNC l_actionCliSync = {PTHREAD_MUTEX_INITIALIZER};    /*The other member of ACTION_CLI_SYNC will be inited at vt_initDebug function*/
//LOCAL sem_t l_actionWaitSem;    /*Its value means how many thread are waiting while executing action.*/
LOCAL DBG_MODE l_debugMode = NO_DBG;
//LOCAL sem_t l_debugSem;
LOCAL VOIP_DSPMSG_CMD * l_breakpoints;
LOCAL int l_numberOfBreakpoint = 0;
//int currentPoint = 0;

LOCAL dictionary *l_actionDict = NULL;      /*store the actions which are loaded from file*/

LOCAL VT_ACTION_E l_initServerAction[] =
{
    {VOIP_DSPMSG_CMDMAX,    cli_connectHandler,         NULL, NULL},       /*[IP [port]]*/
    {VOIP_SLIC_CONTROL,     cli_sendSlicLocaleHandler,  NULL, NULL},      /*locale*/
    {VOIP_DSP_DEFAULT,      cli_sendDspDefaultHandler,  NULL, NULL},      /*locale*/
    {VOIP_DSP_START,        cli_sendDspStartHandler,    NULL, NULL},        /*NULL*/
    {VOIP_CHANNEL_DEFAULT,  sendAllChannelDefault,  NULL, NULL},      /*NULL*/
    ACTION_END
};

LOCAL VT_ACTION_E l_offHookActionDef[] =
{
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "4"},     /*endpt ctrl*/
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "2"},     /*endpt ctrl*/
    {VOIP_TONE_PLAY,    cli_sendTonePlayHandler,    NULL, "4 200"},        /*endpt tone time*/
    ACTION_END
};

LOCAL VT_ACTION_E l_onHookActionDef[] =
{
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "3"},     /*endpt ctrl(LED OFF)*/
    {VOIP_TONE_PLAY,    cli_sendTonePlayHandler, NULL, "0 200"},        /*endpt tone time(stop tone play)*/
    ACTION_END
};

LOCAL VT_ACTION_E l_callingActionDef[] =
{
    /* Before calling, client should send VOIP_TONE_PLAY to stop the tone. */
    {VOIP_CHANNEL_OPEN, cli_sendChannelOpenHandler, NULL, "-1 0 0 0 0"},     /*endpt channel vadEnable gainSpeaker gainMic intt38Enable*/
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "2"},     /*endpt ctrl*/
    {VOIP_TONE_PLAY,    cli_sendTonePlayHandler, NULL, "2 100"},        /*endpt tone time(change tone play)*/
    ACTION_END
};

LOCAL VT_ACTION_E l_callingFailedActionDef[] =
{
    {VOIP_TONE_PLAY,    cli_sendTonePlayHandler, NULL, "4 100"},
    ACTION_END
};

LOCAL VT_ACTION_E l_talkingActionDef[] =
{
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "2"},     /*endpt ctrl*/
    {VOIP_TONE_PLAY,    cli_sendTonePlayHandler, NULL, "0 200"},    /*endpt tone time(stop tone)*/
    {VOIP_RTP_ACTIVE,   cli_sendRtpActiveHandler, NULL, NULL},      /*endpt destIP destPort destCtlport*/
    ACTION_END
};

LOCAL VT_ACTION_E l_beCalledActionDef[] =
{
    {VOIP_CHANNEL_OPEN, cli_sendChannelOpenHandler, NULL, "-1 0 0 0 0"},     /*endpt channel vadEnable gainSpeaker gainMic intt38Enable*/
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "4"},     /*endpt ctrl*/
    {VOIP_CID_PLAY,     cli_sendCidPlayHandler, NULL, "hzg"},         /*channel number name.*/
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "2"},     /*endpt ctrl*/
    {VOIP_RING_START,   cli_sendRingStartHandler, NULL, "0"},       /*endpt type*/
    ACTION_END
};

LOCAL VT_ACTION_E l_acceptCallActionDef[] =
{
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "2"},     /*endpt ctrl*/
    {VOIP_RING_STOP,    cli_sendRingStopHandler, NULL, NULL},        /*endpt*/
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "2"},     /*endpt ctrl*/
    {VOIP_TONE_PLAY,    cli_sendTonePlayHandler, NULL, "0 200"},        /*endpt tone time(stop tone)*/
    {VOIP_RTP_ACTIVE,   cli_sendRtpActiveHandler, NULL, NULL},     /*endpt srcIp srcPort destIP destPort destCtlport*/
    ACTION_END
};

LOCAL VT_ACTION_E l_beOnHookActionDef[] =
{
    {VOIP_RTP_DEACTIVE, cli_sendRtpDeactiveHandler, NULL, NULL},       /*endpt*/
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "4"},     /*endpt ctrl*/
    {VOIP_TONE_PLAY,    cli_sendTonePlayHandler, NULL, "1 200"},        /*endpt tone*/
    ACTION_END
};

LOCAL VT_ACTION_E l_missCallActionDef[] =
{
    {VOIP_SLIC_CONTROL, cli_sendSlicControlHandler, NULL, "4"},     /*endpt ctrl*/
    {VOIP_RING_STOP,    cli_sendRingStopHandler, NULL, NULL},       /*endpt*/
    ACTION_END
};

LOCAL VT_ACTION *l_allActions = NULL;
LOCAL const char **l_allActionsName = NULL;

LOCAL VT_ACTION l_allActionsDef[] =
{
    [VT_ACCEPT_CALL_INX]      = l_acceptCallActionDef,
    [VT_BE_CALLED_INX]        = l_beCalledActionDef,
    [VT_BE_ON_HOOK_INX]       = l_beOnHookActionDef,
    [VT_CALLING_INX]          = l_callingActionDef,
    [VT_CALLING_FAILED_INX]   = l_callingFailedActionDef,
    [VT_MISS_CALL_INX]        = l_missCallActionDef,
    [VT_OFF_HOOK_INX]         = l_offHookActionDef,
    [VT_ON_HOOK_INX]          = l_onHookActionDef,
    [VT_TALKING_INX]          = l_talkingActionDef,
    NULL
};
LOCAL const char *l_allActionsNameDef[] =
{
    [VT_ACCEPT_CALL_INX]      = "AcceptCall",
    [VT_BE_CALLED_INX]        = "BeCalled",
    [VT_BE_ON_HOOK_INX]       = "BeOnHook",
    [VT_CALLING_INX]          = "Calling",
    [VT_CALLING_FAILED_INX]   = "CallingFailed",
    [VT_MISS_CALL_INX]        = "MissCall",
    [VT_OFF_HOOK_INX]         = "OnHook",
    [VT_ON_HOOK_INX]          = "OffHook",
    [VT_TALKING_INX]          = "Talking",
    NULL
};

LOCAL VOIP_DSPMSG_CMD l_needAllDataCmd[] =  /*The cmds that need all arguments of action function*/
{
    VOIP_CID_PLAY,
    VOIP_RTP_ACTIVE,
    VOIP_DSPMSG_CMDMAX, /*end, no other usage.*/
};

/*LOCAL const char * actionsStr[] =
{
    [VT_ACCEPT_CALL_INX]    = "AcceptCall",
    [VT_BE_CALLED_INX]      = "BeCalled",
    [VT_BE_ON_HOOK_INX]     = "BeOnHook",
    [VT_CALLING]            = "Calling",
    [VT_CALLING_FAILED_INX] = "CallingFailed",
    [VT_MISS_CALL_INX]      = "MissCall",
    [VT_OFF_HOOK_INX]       = "OffHook",
    [VT_ON_HOOK_INX]        = "OnHook",
    [VT_]
};*/
/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL int sendAllChannelDefault(const char * pData)
{
    int index = 0;
    int result = 0;

    for (; index < MAX_CNX; index++)
    {
        result = vt_sendChannelDefault(index);

        if (result == VT_FAILED)
        {
            break;
        }
    }

    return result;
}
LOCAL void enterWaitState(const char *pBuff)
{
    printf(CLRbold"The argument of next handler is %s."CLRnl, pBuff);

    pthread_mutex_lock(&l_actionCliSync.mutex);
    if (l_actionCliSync.waitCliNumber > 0)
    {
        sem_post(&l_actionCliSync.cliSem);
    }
    l_actionCliSync.waitingActionNumber++;
    pthread_mutex_unlock(&l_actionCliSync.mutex);

    sem_wait(&l_actionCliSync.actionSem);

    pthread_mutex_lock(&l_actionCliSync.mutex);
    l_actionCliSync.waitingActionNumber--;
    pthread_mutex_unlock(&l_actionCliSync.mutex);
}
LOCAL int actionGo(VT_ACTION action)
{
    int result = 0;
    char buff[BUFF_MAX];

    while (action->fpFunc)
    {
        buff[0] = '\0'; /*if action->pInfoNeed is NULL, this operation will make sure str_strncat success.*/
        str_strncpy(buff, action->pInfoNeed, BUFF_MAX);

        if (action->pInfoNeed)
        {
            str_strncat(buff, " ", BUFF_MAX);
        }
        str_strncat(buff, action->pExtra, BUFF_MAX);

        /*Decide whether to wait or not*/
        if (l_debugMode == BREAK_DBG && isBreakpoint(action->cmdType))
        {
            CX_PRINTF(CLRbold"Reach breakpoint: %s. Waiting ..."CLRnl, vt_cmTxtMapById(action->cmdType));
            enterWaitState(buff);
            CX_PRINTF(CLRbold"Prior debug mode :BREAK_DBG. Continue after breakpoint."CLRnl);
        }
        else if (l_debugMode == STEP_DBG)
        {
            CX_PRINTF(CLRbold"In step debug mode: %s. Waiting ..."CLRnl, vt_cmTxtMapById(action->cmdType));
            enterWaitState(buff);
            CX_PRINTF(CLRbold"Prior debug mode :STEP_DBG. Executing next handler."CLRnl);
        }

        result = action->fpFunc(buff);

        if (result == CMDERR_FAILED || result == CMDERR_INVAIL_ARG)
        {
            break;
        }

        action++;
    }

    if (result != CMDERR_OK)
    {
        CX_LOGL(CX_ERR, "actionGo failed with result : %d.", result);
    }
    else
    {
        CX_LOGL(CX_DBG, "actionGo done successfully.");
    }

    /*When we finish, the CLI may be waiting, we should wake it up.*/
    pthread_mutex_lock(&l_actionCliSync.mutex);
    if (l_actionCliSync.waitCliNumber > 0)
    {
        sem_post(&l_actionCliSync.cliSem);
    }
    pthread_mutex_unlock(&l_actionCliSync.mutex);

    return result;
}
LOCAL BOOL isBreakpoint(VOIP_DSPMSG_CMD cmd)
{
    BOOL result = FALSE;
    int index;

    for (index = 0; index < l_numberOfBreakpoint; index++)
    {
        /*CX_LOGL(CX_INFO, "index=%d, breakpoint=%s, target=%s", index, vt_cmTxtMapById(l_breakpoints[index]),
                vt_cmTxtMapById(cmd));*/
        if (l_breakpoints[index] == cmd)
        {
            result = TRUE;
        }
    }

    return result;
}
LOCAL int handlerNeedAllArgs(VOIP_DSPMSG_CMD cmd)
{
    VOIP_DSPMSG_CMD cmdIndex;
    int index = 0;

    while ((cmdIndex = l_needAllDataCmd[index++]) != VOIP_DSPMSG_CMDMAX)
    {
        if (cmdIndex == cmd)
        {
            return 1;
        }
    }
    return 0;
}

LOCAL void prepareHandlerData(VT_ACTION_E *action, char * pEndpt, const char * pData)
{
    int index = 0;
    const char *pDataBak = pData;

    if (pEndpt)
    {
        int index;

        for (index = 0; index < ENDPT_STRLEN_MAX && pData[index]; index++)
        {
            pEndpt[index] = pData[index];
        }

        pEndpt[ENDPT_STRLEN_MAX - 1] = '\0';
        pData = pEndpt;
    }

    while (action[index].fpFunc)
    {
        if (handlerNeedAllArgs(action[index].cmdType))
        {
            action[index].pInfoNeed = pDataBak;
        }
        else
        {
            action[index].pInfoNeed = pData;
        }

        index++;
    }
}

LOCAL void wakeActionThenWaitIfNeed()
{
    pthread_mutex_lock(&l_actionCliSync.mutex);
    if (l_actionCliSync.waitingActionNumber > 0)
    {
        CX_LOGL(CX_INFO, "%d actions are waiting. Wake it up.", l_actionCliSync.waitingActionNumber);
        sem_post(&l_actionCliSync.actionSem);
        l_actionCliSync.waitCliNumber++;    /*We should do this before unlock*/
        pthread_mutex_unlock(&l_actionCliSync.mutex);

        sem_wait(&l_actionCliSync.cliSem);

        /*Though there is only one thread which may modify l_actionCliSync.waitCliNumber, the lock/unlock operation*/
        /*is necessary because some other thread may read the wrong waitCliNumber and then post cliSem incorrectly.*/
        pthread_mutex_lock(&l_actionCliSync.mutex);
        l_actionCliSync.waitCliNumber--;
        pthread_mutex_unlock(&l_actionCliSync.mutex);
    }
    else
    {
        pthread_mutex_unlock(&l_actionCliSync.mutex);
    }
}
LOCAL void loadDefaultAction()
{
    l_allActions = l_allActionsDef;
    l_allActionsName = l_allActionsNameDef;
}

LOCAL HANDLER getHandlerByCmd(VOIP_DSPMSG_CMD cmd)
{
    switch (cmd)
    {
    case VOIP_DSP_DEFAULT:
        return cli_sendDspDefaultHandler;

    case VOIP_SLIC_CONTROL:
        return cli_sendSlicControlHandler;

    case VOIP_DSP_START:
        return cli_sendDspStartHandler;

    case VOIP_DSP_STOP:
        return cli_sendDspStopHandler;

    case VOIP_CID_PLAY:
        return cli_sendCidPlayHandler;

    case VOIP_RING_START:
        return cli_sendRingStartHandler;

    case VOIP_RING_STOP:
        return cli_sendRingStopHandler;

    case VOIP_CHANNEL_DEFAULT:
        return cli_sendChannelDefaultHandler;

    case VOIP_CHANNEL_OPEN:
        return cli_sendChannelOpenHandler;

    case VOIP_RTP_ACTIVE:
        return cli_sendRtpActiveHandler;

    case VOIP_RTP_DEACTIVE:
        return cli_sendRtpDeactiveHandler;

    case VOIP_TONE_PLAY:
        return cli_sendTonePlayHandler;

    case VOIP_TONE_STOP:
        return cli_sendTonePlayHandler;

    default:
        return NULL;
    }
}

LOCAL void parseHandler(dictionary *pActionDict, char *pSecName, VT_ACTION action, int n)
{
    char buf[BUFF_MAX];

    snprintf(buf, BUFF_MAX, "%s:%s%d", pSecName, CMD_NAME_IN_CFG, n+1);
    action[n].cmdType = vt_getCmdByTxt(iniparser_getstring(pActionDict, buf, NULL));

    action[n].fpFunc = getHandlerByCmd(action[n].cmdType);
    action[n].pInfoNeed = NULL;

    snprintf(buf, BUFF_MAX, "%s:%s%d", pSecName, DATA_NAME_IN_CFG, n+1);
    action[n].pExtra = iniparser_getstring(pActionDict, buf, NULL);

    //CX_LOGL(CX_INFO, "%s: Add Handler: %s %s.\n", pSecName, vt_cmTxtMapById(action[n].cmdType), action[n].pExtra);

}
LOCAL int getNhandlers(dictionary *pActionDict, const char  *pSecName)
{
    char buf[BUFF_MAX];
    int index;

    for (index = 0; ; index++)
    {
        snprintf(buf, BUFF_MAX, "%s:%s%d", pSecName, CMD_NAME_IN_CFG, index+1);

        if (iniparser_getstring(pActionDict, buf, NULL) == NULL)
        {
            break;
        }
    }
    return index;
}

LOCAL void attachActionToFSM(dictionary *pActionDict, char *pSecName, int actionIndex)
{
    char buf[BUFF_MAX];
    char *pEventStr, *pCurStateStr;
    VT_EVENT_TYPE eventType;
    VT_STATE curState;
    int index;

    for (index = 0; ; index++)
    {
        snprintf(buf, BUFF_MAX, "%s:%s%d", pSecName, EVENT_NAME_IN_CFG, index+1);
        pEventStr = iniparser_getstring(pActionDict, buf, NULL);

        if (pEventStr == NULL)
        {
            break;
        }
        eventType = vt_eventStrToInt(pEventStr);

        snprintf(buf, BUFF_MAX, "%s:%s%d", pSecName, CUR_STATE_IN_CFG, index+1);
        pCurStateStr = iniparser_getstring(pActionDict, buf, NULL);
        curState = vt_stateStrToInt(pCurStateStr);

        vt_registerAction(eventType, curState, actionIndex);
    }

}

LOCAL int parseAction(dictionary *pActionDict, char *pSecName, int actionIndex)
{
   int nhandlers = getNhandlers(pActionDict, pSecName);
    //char key[BUFF_MAX];

    if (nhandlers > 0)
    {
        l_allActions[actionIndex] = calloc(nhandlers+1, sizeof(VT_ACTION_E));

        int index;
        for (index = 0; index < nhandlers + 1; index++)
        {
            //NOTE: The last handler will be fill with NULL
            parseHandler(pActionDict, pSecName, l_allActions[actionIndex], index);
        }

        if (str_caseStartWith(pSecName, KEY_ACTION_PREFIX))
        {
            vt_registerKeyAction(pSecName+strlen(KEY_ACTION_PREFIX), actionIndex);
        }
        else
        {
            /*attach the action to VT_FSM_CTX array*/
            attachActionToFSM(l_actionDict, pSecName, actionIndex);
        }

        cli_addActionArg(actionIndex, pSecName);
    }
    else
    {
        CX_LOGL(CX_ERR, "nhandlers <= 0.");
        return VT_FAILED;
    }

    return VT_SUCCESS;
}

LOCAL void destroyActions()
{
    if (l_actionDict && l_allActions && l_allActions != l_allActionsDef)
    {
        int index;

        for (index = 0; l_allActions[index] != NULL; index++)
        {
           free(l_allActions[index]);
        }
        free(l_allActions);
        l_allActions = NULL;
        free(l_allActionsName);
        l_allActionsName = NULL;

        CX_LOGL(CX_DBG, "free %d actions", index);

        iniparser_freedict(l_actionDict);
        l_actionDict = NULL;
    }

}
/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
int vt_actionInitServer(const char * pData)
{
    int index = 1;

    /* cli_connectHandler can take NULL argument. */
    while (l_initServerAction[index].fpFunc)
    {
        l_initServerAction[index].pExtra = pData;
        index++;
    }

    DBG_ACTION("initServer", pData);
    return actionGo(l_initServerAction);
}
const char *vt_getActionStr(int actionIndex)
{
    if (actionIndex < 0)
    {
        return NULL;
    }

    return l_allActionsName[actionIndex];
}

int vt_getActionIdxByStr(const char *pData)
{
    int index = 0;

    for (index = 0; l_allActionsName[index]; index++)
    {
        if (strncasecmp(pData, l_allActionsName[index], strlen(l_allActionsName[index])) == 0)
        {
            return index;
        }
    }
    return -1;
}

void vt_initDebug()
{
    int index;

    l_breakpoints = calloc(g_vtConfig.breakMax, sizeof(VOIP_DSPMSG_CMD));

    for (index = 0; index < g_vtConfig.breakMax; index++)
    {
        l_breakpoints[index] = VOIP_DSPMSG_CMDMAX;
    }

    sem_init(&l_actionCliSync.actionSem, 0, 0);
    sem_init(&l_actionCliSync.cliSem, 0, 0);
    l_actionCliSync.waitCliNumber = 0;
    l_actionCliSync.waitingActionNumber = 0;

    l_debugMode = BREAK_DBG;
    l_numberOfBreakpoint = 0;
}
void vt_exitDebug()
{
    l_debugMode = NO_DBG;
    int index = 0;
    int hasWaitingAction = 0;

    pthread_mutex_lock(&l_actionCliSync.mutex);
    for ( ; index < l_actionCliSync.waitingActionNumber; index++)
    {
        sem_post(&l_actionCliSync.actionSem);
        hasWaitingAction = 1;
        l_actionCliSync.waitCliNumber = 1;
    }
    pthread_mutex_unlock(&l_actionCliSync.mutex);

    if (hasWaitingAction)
    {
        /*Some action are waiting. We should not destroy sem before they are waken up.*/
        /*When one action is finished, it will post cliSem if l_actionCliSync.waitCliNumber is bigger than 0.*/
        /*So we use this to make sure the waiting action are all done.*/
        sem_wait(&l_actionCliSync.cliSem);
    }
    sem_destroy(&l_actionCliSync.cliSem);
    sem_destroy(&l_actionCliSync.actionSem);

    free(l_breakpoints);
}
void vt_allbreak()
{
    if (l_numberOfBreakpoint <= 0)
    {
        printf("There is no breakpoints.\n");
    }
    else
    {
        int index;

        for (index = 0; index < l_numberOfBreakpoint; index++)
        {
            printf("Index %d: %s\n", index + 1, vt_cmTxtMapById(l_breakpoints[index]));
        }
    }
}
void vt_breakAt(VOIP_DSPMSG_CMD cmd)
{
    int index;

    if (l_numberOfBreakpoint >= g_vtConfig.breakMax)
    {
        CX_LOGL(CX_ERR, "Breakpoints too much. Only support %d breakpoints!", g_vtConfig.breakMax);
        return ;
    }

    for (index = 0; index < l_numberOfBreakpoint; index++)
    {
        if (l_breakpoints[index] == cmd)
        {
            CX_LOGL(CX_WARN, "breakpoint : %s has existed!", vt_cmTxtMapById(cmd));
            return ;
        }
    }

    l_breakpoints[l_numberOfBreakpoint++] = cmd;
    CX_LOGL(CX_INFO, "Add breakpoints %s at %d. Total number=%d", vt_cmTxtMapById(cmd), index, l_numberOfBreakpoint);
    /*for (index = 0; index < l_numberOfBreakpoint && cmd > l_breakpoints[index]; index++)
    {

    }

    if (index < l_numberOfBreakpoint && cmd == l_breakpoints[index])
    {
        CX_LOGL(CX_WARN, "breakpoint : %s has existed!", vt_cmTxtMapById(cmd));
    }

    else
    {
        //CX_LOGL(CX_INFO, "memmove : index=%d", index);

        if (l_numberOfBreakpoint - index - 1 > 0)
        {
            memmove(l_breakpoints+index+1, l_breakpoints+index, (l_numberOfBreakpoint-index-1)*sizeof(VOIP_DSPMSG_CMD));
        }
        l_breakpoints[index] = cmd;
        l_numberOfBreakpoint++;
        CX_LOGL(CX_INFO, "Add breakpoints %s at %d. Total number=%d", vt_cmTxtMapById(cmd), index, l_numberOfBreakpoint);
    }*/
}
void vt_unbreakAt(VOIP_DSPMSG_CMD cmd)
{
    int index;

    for (index = 0; index < l_numberOfBreakpoint && cmd != l_breakpoints[index]; index++)
    {

    }

    if (index >= l_numberOfBreakpoint)
    {
        CX_LOGL(CX_ERR, "Cannot find breakpoint: %s. Total number = %d", vt_cmTxtMapById(cmd), l_numberOfBreakpoint);
    }
    else
    {
        memmove(l_breakpoints + index, l_breakpoints + index + 1, (l_numberOfBreakpoint - index - 1)*sizeof(VOIP_DSPMSG_CMD));
        l_numberOfBreakpoint--;
        CX_LOGL(CX_INFO, "Find breakpoint: %s at %d. After delete, the number of breakpoints is %d", vt_cmTxtMapById(cmd), index, l_numberOfBreakpoint);
    }
}
void vt_next()
{
    l_debugMode = STEP_DBG;

    wakeActionThenWaitIfNeed();
}
void vt_run()
{
    l_debugMode = BREAK_DBG;

    wakeActionThenWaitIfNeed();
}

void vt_cliWaiting() /*This function now is called only by action command in debug mode*/
{
    pthread_mutex_lock(&l_actionCliSync.mutex);
    l_actionCliSync.waitCliNumber++;
    pthread_mutex_unlock(&l_actionCliSync.mutex);

    CX_LOGL(CX_DBG, "CLI is waiting...");
    sem_wait(&l_actionCliSync.cliSem);

    pthread_mutex_lock(&l_actionCliSync.mutex);
    l_actionCliSync.waitCliNumber--;
    pthread_mutex_unlock(&l_actionCliSync.mutex);
}
void vt_resetDebugMode() /*This function now is called only before each command running in debug mode.*/
{
    l_debugMode = BREAK_DBG;
}

int vt_loadAction(const char *pActionFilePath)
{
    if (l_actionDict)
    {
        CX_LOGL(CX_WARN, "ActionDict not null. Will destroy prior-alloc actions.");
        destroyActions();
    }

    if ((l_actionDict = iniparser_load(pActionFilePath)) == NULL)
    {
        CX_LOGL(CX_WARN, "Loading ini file: %s failed! Will load default action.", pActionFilePath);
        loadDefaultAction();
        return VT_FAILED;
    }

    int nactions;
    if ((nactions = iniparser_getnsec(l_actionDict)) < ACTION_NUM)
    {
        CX_LOGL(CX_WARN, "Action is not enough!! The tester may behaviour abnormally.");
    }

    if ((l_allActionsName = calloc(nactions+1, sizeof(const char *))) == NULL)
    {
        CX_LOGL(CX_FATAL, "Memory not enough!!");
        iniparser_freedict(l_actionDict);
        l_actionDict = NULL;
        return VT_FAILED;
    }
    l_allActionsName[nactions] = NULL;

    if ((l_allActions = calloc(nactions+1, sizeof(VT_ACTION))) == NULL)
    {
        CX_LOGL(CX_FATAL, "Memory not enough!!");
        iniparser_freedict(l_actionDict);
        l_actionDict = NULL;
        return VT_FAILED;
    }
    l_allActions[nactions] = NULL;

    cli_initActionArgs(nactions);

    int index;
    char *pSecName;

    for (index = 0; index < nactions; index++)
    {
        pSecName = iniparser_getsecname(l_actionDict, index);
        l_allActionsName[index] = pSecName;
        parseAction(l_actionDict, pSecName, index);
    }
    return VT_SUCCESS;
}

int vt_runAction(int actionIndex, const char *pData)
{
    assert(l_allActions);

    if (actionIndex < 0)
    {
        return VT_SUCCESS;
    }

    char endpt[ENDPT_STRLEN_MAX];
    prepareHandlerData(l_allActions[actionIndex], endpt, pData);

    CX_LOGL(CX_DBG, "Running action: %s(%d)", l_allActionsName[actionIndex], actionIndex);
    return actionGo(l_allActions[actionIndex]);
}
