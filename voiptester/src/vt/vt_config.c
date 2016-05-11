/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_config.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		28Oct14
 *
 * history 	\arg
 */
#include <stdio.h>

#include <iniparser.h>

#include <base/debug.h>

#include <vt/vt_config.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define DEFAULT_KEY0_DIGIT    48
#define DEFAULT_BREAK_MAX     10
#define DEFAULT_HISTORY_SIZE        50
#define DEFAULT_EVENT_QUEUE_SIZE    10

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
VT_CONFIG g_vtConfig;

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL void loadDefaultConfig()
{
    g_vtConfig.serverIP = VOIP_SERVER_IP_INT;
    g_vtConfig.serverPort = VOIP_SERVER_PORT;
    g_vtConfig.clientListenPort = CLIENT_PORT;

    g_vtConfig.outerClientIP = OUTER_CLIENT_IP;
    g_vtConfig.outerClientListenPort = OUTER_CLIENT_PORT;
    g_vtConfig.outClientNumber = OUTER_CLIENT_NUM;

    int index = 0;

    for (index = 0; index < DTMF_DIGIT_AMOUNT - 2; index++)
    {
        g_vtConfig.dtmfDigitMap[index] = DEFAULT_KEY0_DIGIT + index;
    }
    g_vtConfig.dtmfDigitMap[10] = 42;    /*Key **/
    g_vtConfig.dtmfDigitMap[11] = 35;    /*Key #*/

    g_vtConfig.endptInfo[0] = (ENDPT_MSG){11000, 11001, 123};
    g_vtConfig.endptInfo[1] = (ENDPT_MSG){12000, 12001, 456};

    g_vtConfig.breakMax = DEFAULT_BREAK_MAX;

    g_vtConfig.storeEventMax = DEFAULT_EVENT_QUEUE_SIZE;
}

/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
int vt_initConfig(const char *pFilePath)
{
    dictionary *pIniDict;

    if ((pIniDict = iniparser_load(pFilePath)) == NULL)
    {
        CX_LOGL(CX_WARN, "Loading ini file: %s failed! Will load default config.", pFilePath);
        loadDefaultConfig();
        return VT_FAILED;
    }

    //iniparser_dump(pIniDict, stderr);

    char *pIPStr = iniparser_getstring(pIniDict, "VoIPServer:IP", VOIP_SERVER_IP_STR);
    if (inet_pton(AF_INET, pIPStr, &g_vtConfig.serverIP) != 1)
    {
        g_vtConfig.serverIP = VOIP_SERVER_IP_INT;
        CX_LOGL(CX_ERR, "inet_pton: IP %s is incorrect. Modify it to %x", pIPStr, VOIP_SERVER_IP_INT);
        return VT_FAILED;
    }

    g_vtConfig.serverIP = ntohl(g_vtConfig.serverIP);
    g_vtConfig.serverPort = iniparser_getint(pIniDict, "VoIPServer:Port", VOIP_SERVER_PORT);
    g_vtConfig.clientListenPort = iniparser_getint(pIniDict, "VoIPClient:ListenPort", CLIENT_PORT);

    pIPStr = iniparser_getstring(pIniDict, "OuterVoIPClient:IP", OUTER_CLIENT_IP_STR);
    if (inet_pton(AF_INET, pIPStr, &g_vtConfig.outerClientIP) != 1)
    {
        g_vtConfig.outerClientIP = OUTER_CLIENT_IP;
        CX_LOGL(CX_ERR, "inet_pton: IP %s is incorrect.", pIPStr);
        return VT_FAILED;
    }

    g_vtConfig.outerClientIP = ntohl(g_vtConfig.outerClientIP);
    g_vtConfig.outerClientListenPort = iniparser_getint(pIniDict, "OuterVoIPClient:ListenPort", OUTER_CLIENT_PORT);
    g_vtConfig.outClientNumber = iniparser_getint(pIniDict, "OuterVoIPClient:Number", OUTER_CLIENT_NUM);

    g_vtConfig.breakMax = iniparser_getint(pIniDict, "Debug:BreakMax", DEFAULT_BREAK_MAX);

    int index;
    char keyname[BUFF_MAX];

    for (index = 0; index < DTMF_DIGIT_AMOUNT; index++)
    {
        snprintf(keyname, BUFF_MAX, "DTMFDigitMap:Key%d", index);

        g_vtConfig.dtmfDigitMap[index] = iniparser_getint(pIniDict, keyname, 0);
    }

    for (index = 0; index < VT_ENDPT_MAX; index++)
    {
        snprintf(keyname, BUFF_MAX, "Endpt%d:Port", index);
        g_vtConfig.endptInfo[index].port = iniparser_getint(pIniDict, keyname, 0);

        snprintf(keyname, BUFF_MAX, "Endpt%d:CtrlPort", index);
        g_vtConfig.endptInfo[index].ctrlport = iniparser_getint(pIniDict, keyname, 0);

        snprintf(keyname, BUFF_MAX, "Endpt%d:Number", index);
        g_vtConfig.endptInfo[index].number = iniparser_getint(pIniDict, keyname, 0);
    }

    g_vtConfig.storeEventMax = iniparser_getint(pIniDict, "EventQueue:EventMax", DEFAULT_EVENT_QUEUE_SIZE);

    g_vtConfig.historySize = iniparser_getint(pIniDict, "Readline:HistorySize", DEFAULT_HISTORY_SIZE);

    iniparser_freedict(pIniDict);

    return VT_SUCCESS;
}
