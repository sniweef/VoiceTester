/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		common.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		13Oct14
 *
 * history 	\arg
 */
#ifndef __COMMON_H__
#define __COMMON_H__


#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <base/string.h>
#include <base/debug.h>
#include <base/socket.h>
#include <base/voip_comm.h>


/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
#define GLOBAL
#define LOCAL static
#define BUFF_MAX        50

#define ABS(x)  ((x) > 0 ? (x) : -(x))
#define MAX(x, y)  ((x) > (y) ? (x) : (y))

#define DEFAULT_CFG_PATH        ".vc_config"
#define DEFAULT_ACTION_PATH     ".vc_action"

#define IP_STR_MAX_LEN          16
#define VOIP_SERVER_IP_INT      (int32_t)0xC0A80101  /*192.168.1.1*/
#define VOIP_SERVER_IP_STR      "192.168.1.1"
#define VOIP_SERVER_PORT        10002

#define CLIENT_PORT  15000
#define CLIENT_IP   0x7F000001  /*127.0.0.1*/

#define OUTER_CLIENT_IP_STR     "127.0.0.1"
#define OUTER_CLIENT_IP         (uint32_t)0x7F000001
#define OUTER_CLIENT_PORT       16000
#define OUTER_CLIENT_NUM        789

#define VT_SUCCESS           0
#define VT_FAILED            -1

#define VT_CHANNEL_MAX       4
#define VT_ENDPT_MAX         MAX_ENDPT

#define DTMF_DIGIT_AMOUNT   12

#define ENDPT_STRLEN_MAX    2
/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/


/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/


#endif // __COMMON_H__

