/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		cli_cmdHandler.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		13Oct14
 *
 * history 	\arg
 */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <base/common.h>
#include <base/voip_comm.h>
#include <cli/cli_cmdmap.h>
#include <cli/cli_main.h>
#include <vt/vt_send.h>
#include <vt/vt_action.h>
#include <vt/vt_msgQueue.h>
#include <vt/vt_event.h>
#include <vt/vt_fsm.h>
#include <vt/vt_config.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
//#define CHECK_NO_ARG(pData)     if (*(pData))  { return CMDERR_INVAIL_ARG;}
#define DEFAULT_REMOTE_PORT     16100
#define DEFAULT_REMOTE_CTRLPORT 16101
#define CHECK_NO_ARG(pData)

#define CHECK_NEXT_INT(pRet, pData, pInt)   \
                if ((pRet = str_getNextInt(pData, pInt)) == NULL)   \
                {                                                   \
                    return CMDERR_INVAIL_ARG;                       \
                }

#define CHECK_NEXT_STR(pRet, pSrc, pDest, len)   \
                if ((pRet = str_getNextStr(pSrc, pDest, len)) == NULL)   \
                {                                                   \
                    return CMDERR_INVAIL_ARG;                       \
                }
/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/
extern CMD_MAP_T g_debugCmdMap[];     /*Declare at cli_cmdmap.c*/

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
LOCAL int l_exitDebugCli = 0;

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/

LOCAL void *actionThread(void *pArg)
{
    usleep(50*1000);   /*Sleep some time to make sure cli wait first*/
    CX_LOGL(CX_DBG, "Starting a new thread to execute action command.");

    /*We must copy data of pArg to new array. Otherwises, pInfoNeed in VT_ACTION_E is just pointing to pArg. When*/
    /*this thread come into waiting state and debug CLI is active again, the memory which pArg point to will be freed!*/
    char buff[BUFF_MAX];
    str_strncpy(buff, pArg, BUFF_MAX);

    cli_execCmd(g_debugCmdMap, buff);

    return NULL;
}
LOCAL char **parseRtpActiveOpt(const char *pData, int *pArgc, VOIP_MEDIA_DIRECTION *pDireciton,
                               VOIP_DTMF_RELAY *pDtmfMethod, int *pPayloadType, int *pPtime)
{
    static struct option s_opts[] =
    {
        {"direction", required_argument, NULL, 'd'},
        {"dtmfmethod", required_argument, NULL, 'D'},
        {"payloadtype", required_argument, NULL, 'p'},
        {"ptime", required_argument, NULL, 'P'},
        {NULL, no_argument, NULL, 0},
    };

    //int argc = 0;
    char ** pArgv = str_dumpArgv("SendRtpActive", pData, pArgc);
    int opt;
    int longIndex;

    if (pArgv == NULL)
    {
        CX_LOGL(CX_ERR, "argv is NULL");
        return NULL;
    }
    optind = 0; /*Reset optind.*/

    while ((opt = getopt_long(*pArgc, pArgv, "d:D:p:P:", s_opts, &longIndex)) != -1)
    {
        //printf("optind=%d, optarg: %s\n", optind, optarg);
        switch (opt)
        {
        case 'd':
            *pDireciton = atoi(optarg);
            break;

        case 'D':
            *pDtmfMethod = atoi(optarg);
            break;

        case 'p':
            *pPayloadType = atoi(optarg);
            break;

        case 'P':
            *pPtime = atoi(optarg);
            break;
        case '?':
            CX_PRINTF(CLRhigh"Option: -d direction -D DtmfRelay -p payloadType -P ptime endpt [remoteIP remotePort, remoteCtrlPort]"CLRnl);
            break;
        default:
            CX_LOGL(CX_ERR, "The option : %c is not support. Skip it.", opt);
            break;
        }
    }

    /*int index;
    for (index =0; index < argc; index++)
    {
        printf("Index %d: %s.\n", index, pArgv[index]);
    }
    printf("optind=%d\n", optind);*/
    CX_LOGL(CX_DBG, "parse end with optind=%d", optind);

    return pArgv;
}
void innerExecCmd(char *pReadline)
{
    pthread_t tid;

    SKIP_WHITE(pReadline);

    if (strncmp(pReadline, "action", 6) == 0)
    {
        /*Because action command may be wait while running, so we must create a new thread to execute the command.*/
        pthread_create(&tid, NULL, actionThread, pReadline);
        vt_cliWaiting();     /*Just wait for action command to finish running or reach a breakpoint*/
    }
    else
    {
        cli_execCmd(g_debugCmdMap, pReadline);
    }
}
/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
/* WRANING: Every handler assume that pData doesnot begin with white space.  */

int cli_actionHandler(const char *pData)
{
    if (pData)
    {
        SKIP_WHITE(pData);
        if (strncmp("InitServer", pData, 10) == 0)
        {
            REACH_NEXT_WHITE(pData);
            return vt_actionInitServer(pData);
        }

        int actionIndex = vt_getActionIdxByStr(pData);

        REACH_NEXT_WHITE(pData);
        SKIP_WHITE(pData);
        return vt_runAction(actionIndex, pData);
    }

    return CMDERR_INVAIL_ARG;
}
int cli_actionInitServerHandler(const char *pData)
{
    /* When want to specific the IP and port of server, modify this function and the parameter of actionInitserver*/
    return vt_actionInitServer(pData);
}

int cli_connectHandler(const char *pData)
{
    CX_LOGL(CX_DUMP, "Connect cmd handler. Args:%s.", pData);
    int execResult = CMDERR_OK;

    if (pData && *pData)
    {
        char ipStr[IP_STR_MAX_LEN];
        uint16_t port = 0;
        int index = 0;
        int isDigit;

        while (*pData && !WHITESPACE(*pData))
        {
            ipStr[index++] = *pData++;
        }
        ipStr[index] = '\0';

        SKIP_WHITE(pData);

        if (*pData)
        {
            isDigit = str_isDigit(pData);
            if (isDigit)
            {
                port = (uint16_t)atoi(pData);
                execResult = vt_connectToServer(ipStr, port);
            }
            else
            {
                execResult = CMDERR_INVAIL_ARG;
            }
        }
        else
        {
            execResult = vt_connectToServer(ipStr, 0);
        }

    }
    else
    {
        execResult = vt_connectToServer(NULL, 0);
    }

    if (execResult > 0)
    {
        printf("Connect to server success. fd=%d.\n", execResult);
    }

    return execResult > 0 ? CMDERR_OK : CMDERR_FAILED;
}
int cli_shutdownHandler(const char *pData)
{
    vt_shutdownSocket();
    return CMDERR_OK;
}

int cli_exitHandler(const char *pData)
{
    l_exitDebugCli = 1;
    return CMDERR_OK;
}
int cli_breakHandler(const char *pData)
{
    VOIP_DSPMSG_CMD cmd = vt_getCmdByTxt(pData);

    if (cmd == UNKNOWN)
    {
        return CMDERR_INVAIL_ARG;
    }
    vt_breakAt(cmd);
    return CMDERR_OK;
}
int cli_unbreakHandler(const char *pData)
{
    VOIP_DSPMSG_CMD cmd = vt_getCmdByTxt(pData);

    if (cmd == UNKNOWN)
    {
        return CMDERR_INVAIL_ARG;
    }
    vt_unbreakAt(cmd);
    return CMDERR_OK;
}
int cli_debugHandler(const char *pData)
{
    cli_setLocalCmdMap(g_debugCmdMap);

    vt_initDebug();

    HIST_ENTRY *pPreHistory;
    char *pReadline;
    l_exitDebugCli = 0; /*When user input the command exit, this variable will be modify to 1*/

    while (!l_exitDebugCli && (pReadline = readline(CLRhigh"Debug mode >  "CLRn)) != NULL)
    {
        /*When we run command: next, debug mode will be changed to step debug mode. Then if we enter command: action *, it will stop at once althought*/
        /*we donot set any breakpoint! This behaviour is abnormal. So I reset the debug mode to BREAK_DBG before any command running.*/
        vt_resetDebugMode();

        if (*pReadline)
        {
            add_history(pReadline);

            innerExecCmd(pReadline);

            free(pReadline);
        }
        else
        {
            pPreHistory = previous_history();
            if (pPreHistory)
            {
                pReadline = pPreHistory->line;
                CX_LOGL(CX_INFO, "No input in debug mode. Executing last command: %s", pReadline);

                innerExecCmd(pReadline);
            }
        }

        pReadline = NULL;
    }

    vt_exitDebug();
    return CMDERR_OK;
}
int cli_sendHandler(const char *pData)
{
    return CMDERR_INVAIL_ARG;
}

int cli_msgNewHandler(const char *pData)
{
    CHECK_NO_ARG(pData);

    vt_msgNew();
    return CMDERR_OK;
}
int cli_msgRefreshHandler(const char *pData)
{
    CHECK_NO_ARG(pData);

    vt_msgRefresh();
    return CMDERR_OK;
}
int cli_msgAllHandler(const char *pData)
{
    CHECK_NO_ARG(pData);

    vt_msgAll();
    return CMDERR_OK;
}
int cli_msgClearHandler(const char *pData)
{
    CHECK_NO_ARG(pData);

    return vt_msgClear();
}
int cli_msgSessionHandler(const char *pData)
{
    CHECK_NO_ARG(pData);

    return vt_msgSession();
}
int cli_reloadHandler(const char *pData)
{
    if (!pData || *pData == '\0')
    {
        pData = DEFAULT_CFG_PATH;
    }
    return vt_initConfig(pData);
}

int cli_reactionHandler(const char *pData)
{
    if (!pData || *pData == '\0')
    {
        pData = DEFAULT_ACTION_PATH;
    }
    return vt_loadAction(pData);
}

int cli_sendSlicLocaleHandler(const char *pData)
{
    //TODO
    if (!pData || *pData == '\0')
    {
        pData = "CN";
    }
    vt_sendSlicLocale(pData);
    return CMDERR_OK;
}
int cli_sendDspDefaultHandler(const char *pData)
{
    if (!pData || *pData == '\0')
    {
        pData = "CN";
    }
    vt_sendDspDefault(pData);
    return CMDERR_OK;
}
int cli_sendDspStartHandler(const char *pData)
{
    CHECK_NO_ARG(pData);

    vt_sendDspStart();
    return CMDERR_OK;
}
int cli_sendDspStopHandler(const char *pData)
{
    CHECK_NO_ARG(pData);

    vt_sendDspStop();
    return CMDERR_OK;
}
int cli_sendChannelDefaultHandler(const char *pData)
{
    if (!str_isDigit(pData))
    {
        return CMDERR_INVAIL_ARG;
    }

    int channel = atoi(pData);

    if (channel > VT_CHANNEL_MAX)
    {
        return CMDERR_INVAIL_ARG;
    }
    return vt_sendChannelDefault(channel);
}

int cli_sendRingStartHandler(const char *pData)
{
    int endpt, type;
    const char *pRet;

    if (!pData || *pData == '\0')
    {
        endpt = type = 0;
    }
    else
    {

        CHECK_NEXT_INT(pRet, pData, &endpt);

        CHECK_NEXT_INT(pRet, pRet, &type);

        /*if ((pRet = str_getNextInt(pData, &endpt)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pRet = str_getNextInt(pRet, &type)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }*/
    }

    return vt_sendRingStart(endpt, type);
}
int cli_sendRingStopHandler(const char *pData)
{
    int endpt;
    if (!pData || *pData == '\0')
    {
        endpt = 0;
    }
    else
    {
        CHECK_NEXT_INT(pData, pData, &endpt);
        /*if (str_getNextInt(pData, &endpt) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }*/
    }

    return vt_sendRingStop(endpt);
}
int cli_sendSlicControlHandler(const char *pData)
{
    int endpt, ctrl;

    if (!pData || *pData == '\0')
    {
        endpt = ctrl = 0;
    }
    else
    {
        CHECK_NEXT_INT(pData, pData, &endpt);

        CHECK_NEXT_INT(pData, pData, &ctrl);
        /*if ((pData = str_getNextInt(pData, &endpt)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, &ctrl)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }*/
    }

    return vt_sendSlicControl(endpt, ctrl);
}
int cli_sendTonePlayHandler(const char *pData)
{
    int endpt;
    EPSIG tone;
    int time;

    if (!pData || *pData == '\0')
    {
        endpt = 0;
        tone = EPSIG_BUSY;
        time = 100;
    }
    else
    {
        CHECK_NEXT_INT(pData, pData, &endpt);
        CHECK_NEXT_INT(pData, pData, (int*)&tone);
        CHECK_NEXT_INT(pData, pData, &time);

        /*if ((pData = str_getNextInt(pData, &endpt)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, (int *)&tone)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, &time)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }*/
    }

    return vt_sendTonePlay(endpt, tone, time);
}
int cli_sendCidPlayHandler(const char *pData)
{
    int endpt;
    char number[BUFF_MAX];
    char name[BUFF_MAX];

    if (!pData || *pData == '\0')
    {
        endpt = 0;
        str_strncpy(number, "12345678", BUFF_MAX);
        str_strncpy(name, "hzg", BUFF_MAX);
    }
    else
    {

        /*if ((pData = str_getNextInt(pData, &endpt)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextStr(pData, number, BUFF_MAX)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextStr(pData, name, BUFF_MAX)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }*/

        CHECK_NEXT_INT(pData, pData, &endpt);
        CHECK_NEXT_STR(pData, pData, number, BUFF_MAX);
        CHECK_NEXT_STR(pData, pData, name, BUFF_MAX);

        if (!str_isDigit(number))
        {
            return CMDERR_INVAIL_ARG;
        }
    }

    return vt_sendCidPlay(endpt, number, name);
}
int cli_sendChannelOpenHandler(const char *pData)
{
    int endpt, channel, vadEnable, gainSpeaker, gainMic, t38Enable;

    if (!pData || *pData == '\0')
    {
        endpt = channel = vadEnable = gainMic = gainSpeaker = t38Enable = 0;
    }
    else
    {
        CHECK_NEXT_INT(pData, pData, &endpt);
        CHECK_NEXT_INT(pData, pData, &channel);
        CHECK_NEXT_INT(pData, pData, &vadEnable);
        CHECK_NEXT_INT(pData, pData, &gainSpeaker);
        CHECK_NEXT_INT(pData, pData, &gainMic);
        CHECK_NEXT_INT(pData, pData, &t38Enable);

        /*if ((pData = str_getNextInt(pData, &endpt)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, &channel)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, &vadEnable)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, &gainSpeaker)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, &gainMic)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }
        if ((pData = str_getNextInt(pData, &t38Enable)) == NULL)
        {
            return CMDERR_INVAIL_ARG;
        }*/
    }

    return vt_sendChannelOpen(endpt, channel, vadEnable, gainSpeaker, gainMic, t38Enable);
}

int cli_sendRtpActiveHandler(const char *pData)
{
    int endpt;
    /*int localAddr;
    int localPort;*/
    int remoteAddr;
    int remotePort;
    int remoteCtlport;
    VOIP_MEDIA_DIRECTION direction = VOIP_MEDIA_SENDRECV;
    VOIP_DTMF_RELAY dtmfMethod = VOIP_DTMF_NONE;
    int payloadType = 0;
    int ptime = 20;

    if (!pData || *pData == '\0')
    {
        endpt = 0;
        /*localAddr = g_vtConfig.serverIP;
        localPort = g_vtConfig.endptInfo[0].port;*/
        remoteAddr = g_vtConfig.serverIP;
        remotePort = g_vtConfig.endptInfo[1].port;
        remoteCtlport = g_vtConfig.endptInfo[1].ctrlport;
    }
    else
    {
        int argc = 0;
        char **ppArgvAll = parseRtpActiveOpt(pData, &argc, &direction, &dtmfMethod, &payloadType, &ptime);
        char **ppArgv = NULL;

		if (ppArgvAll)
		{
			/*optind is from getopt module. It represents the index of first argument that is not match in getopt_long function*/
			ppArgv = ppArgvAll + optind;
			CX_LOGL(CX_DBG, "optind=%d", optind);
		}

        if (!ppArgv || !str_isDigit(ppArgv[0]))
        {
            return CMDERR_INVAIL_ARG;
        }
        endpt = atoi(ppArgv[0]);

        if (endpt < 0 || endpt > VT_ENDPT_MAX)
        {
            CX_LOGL(CX_ERR, "endpt %d no correct.", endpt);
            return CMDERR_INVAIL_ARG;
        }

        remoteAddr = (uint32_t)(ppArgv[1] ? strtoll(ppArgv[1], NULL, 16) : g_vtConfig.serverIP);
        remotePort = (uint16_t)((ppArgv[1] && ppArgv[2]) ? strtol(ppArgv[2], NULL, 10) : g_vtConfig.endptInfo[1-endpt].port);
        remoteCtlport = (uint16_t)((ppArgv[1] && ppArgv[2] && ppArgv[3]) ? strtol(ppArgv[3], NULL, 10) : g_vtConfig.endptInfo[1-endpt].ctrlport);
        /*CHECK_NEXT_INT(pData, pData, &localAddr);
        CHECK_NEXT_INT(pData, pData, &localPort);
        CHECK_NEXT_INT(pData, pData, &remoteAddr);
        CHECK_NEXT_INT(pData, pData, &remotePort);
        CHECK_NEXT_INT(pData, pData, &remoteCtlport);*/
        str_freeArgv(argc, ppArgvAll);

    }
    return vt_sendRtpActive(endpt, remoteAddr, remotePort, remoteCtlport, direction, dtmfMethod, payloadType, ptime);
}
int cli_sendRtpDeactiveHandler(const char *pData)
{
    int endpt;

    if (!pData || *pData == '\0')
    {
        endpt = 0;
    }
    else
    {
        CHECK_NEXT_INT(pData, pData, &endpt);
    }

    return vt_sendRtpDeactive(endpt);
}


