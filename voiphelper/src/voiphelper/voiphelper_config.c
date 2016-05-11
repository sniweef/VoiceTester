/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		voipc_config.c
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

#include <base/common.h>

#include <voiphelper/voiphelper_config.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define CLIENT_IP_STR "192.168.1.2"
#define CLIENT_IP     (uint32_t)0xC0A80102  /*192.168.1.2*/
#define CLIENT_PORT   (uint16_t)16000
#define CLIENT_NUM    789

#define CLIENT_DSP_PORT     (uint16_t)16100
#define CLIENT_DSP_CTRLPORT (uint16_t)16101

#define SEND_VOICE_TIMES    3

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
CONFIG g_voiphConfig;

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL void loadDefaultConfig()
{
    g_voiphConfig.clientIP = CLIENT_IP;
    g_voiphConfig.listenPort = CLIENT_PORT;
    g_voiphConfig.number = CLIENT_NUM;
    g_voiphConfig.dspPort = CLIENT_DSP_PORT;
    g_voiphConfig.dspCtrlPort = CLIENT_DSP_CTRLPORT;
    g_voiphConfig.sendVoiceTimes = SEND_VOICE_TIMES;
}

/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
int voiph_initConfig(const char *pFilePath)
{
    dictionary *pIniDict;

    if ((pIniDict = iniparser_load(pFilePath)) == NULL)
    {
        CX_LOGL(CX_WARN, "Loading ini file: %s failed! Will load default config.", pFilePath);
        loadDefaultConfig();
        return -1;
    }

    //iniparser_freedict(pIniDict);
    char *pIPStr = iniparser_getstring(pIniDict, "VoIPClient:IP", CLIENT_IP_STR);
    if (inet_pton(AF_INET, pIPStr, &g_voiphConfig.clientIP) != 1)
    {
        g_voiphConfig.clientIP = CLIENT_IP;
        CX_LOGL(CX_ERR, "inet_pton: IP %s is incorrect. Modify it to %x", pIPStr, CLIENT_IP);
        return -1;
    }
    g_voiphConfig.clientIP = ntohl(g_voiphConfig.clientIP);

    g_voiphConfig.listenPort = iniparser_getint(pIniDict, "VoIPClient:ListenPort", CLIENT_PORT);
    g_voiphConfig.number = iniparser_getint(pIniDict, "VoIPClient:Number", CLIENT_NUM);

    g_voiphConfig.dspPort = iniparser_getint(pIniDict, "VoIPClient:DspPort", CLIENT_DSP_PORT);
    g_voiphConfig.dspCtrlPort = iniparser_getint(pIniDict, "VoIPClient:DspCtrlPort", CLIENT_DSP_CTRLPORT);

    g_voiphConfig.sendVoiceTimes = iniparser_getint(pIniDict, "SendVoice:Times", SEND_VOICE_TIMES);
    return 0;
}
