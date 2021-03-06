#ifndef __VOIPHELPER_CONFIG_H__
#define __VOIPHELPER_CONFIG_H__

#include <stdint.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define DEFAULT_CFG_PATH    ".vch_config"

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef struct _CONFIG
{
    uint16_t listenPort;
    uint16_t dspPort;
    uint16_t dspCtrlPort;

    uint16_t sendVoiceTimes;

    uint32_t clientIP;
    unsigned long long number;
} CONFIG;

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
extern CONFIG g_voiphConfig;

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
int voiph_initConfig(const char *pFilePath);

#endif // __VOIPHELPER_CONFIG_H__
