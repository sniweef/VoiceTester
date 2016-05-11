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
#include <voiphelper/voiphelper_main.h>
#include <voiphelper/voiphelper_config.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define CMD_MAP_END    {NULL, NULL, NULL, NULL, NULL},
#define CMD_ARG_END    {NULL , NULL, NULL},

#define OUTER_IP    (uint32_t)0x7F000001  /*192.168.1.2*/
#define OUTER_PORT  15000

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/
LOCAL int acceptHandler(const char *pData);
LOCAL int callHandler(const char *pData);
LOCAL int downHandler(const char *pData);
LOCAL int rejectHandler(const char *pData);
LOCAL int reloadHandler(const char *pData);
/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

CMD_MAP_T g_topCmdMap[] =
{
    {"accept", "accept", acceptHandler, NULL, NULL},
    {"call", "call number.", callHandler, NULL, NULL},
    {"down", "down docs", downHandler, NULL, NULL},
    //{"init", "init docs", NULL, initSubcmdMap, NULL},
    {"reject", "reject docs.", rejectHandler, NULL, NULL},
    {"reload", "reload docs.", reloadHandler, NULL, NULL},
    CMD_MAP_END
};


/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL int acceptHandler(const char *pData)
{
    return acceptCall();
}
LOCAL int callHandler(const char *pData)
{
    if (!str_isDigit(pData))
    {
        return CMDERR_INVAIL_ARG;
    }
    int number = atoi(pData);
    return call(number, OUTER_IP, OUTER_PORT);
}
LOCAL int downHandler(const char *pData)
{
    return downCall();
}
LOCAL int rejectHandler(const char *pData)
{
    return rejectCall();
}
LOCAL int reloadHandler(const char *pData)
{
    return voiph_initConfig(DEFAULT_CFG_PATH);
}
/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
