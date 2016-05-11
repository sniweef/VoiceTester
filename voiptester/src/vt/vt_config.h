/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_config.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		28Oct14
 *
 * history 	\arg
 */

#ifndef __VT_CONFIG_H__
#define __VT_CONFIG_H__

#include <base/common.h>
#include <base/voip_comm.h>

#include <vt/vt_event.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/


/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef struct _VT_CONFIG
{
    uint16_t            serverPort;
    uint16_t            clientListenPort;
    uint32_t            serverIP;

    uint32_t            outerClientIP;
    uint16_t            outerClientListenPort;
    unsigned long long  outClientNumber;

    int                 breakMax;
    int                 dtmfDigitMap[DTMF_DIGIT_AMOUNT];
    ENDPT_MSG           endptInfo[VT_ENDPT_MAX];

    int                 storeEventMax;

    int                 historySize;
} VT_CONFIG;

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
extern VT_CONFIG g_vtConfig;

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
int vt_initConfig(const char *pFilePath);

#endif	/* __VT_CONFIG_H__ */
