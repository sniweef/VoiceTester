/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		cli_cmdHandler.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		13Oct14
 *
 * history 	\arg
 */
#ifndef __CLI_CMDHANDLER_H__
#define __CLI_CMDHANDLER_H__

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
int cli_actionHandler(const char *pData);
int cli_actionInitServerHandler(const char *pData);

int cli_connectHandler(const char *pData);
int cli_shutdownHandler(const char *pData);

int cli_sendHandler(const char *pData);

int cli_exitHandler(const char *pData);
int cli_breakHandler(const char *pData);
int cli_unbreakHandler(const char *pData);
int cli_debugHandler(const char *pData);

int cli_msgNewHandler(const char *pData);
int cli_msgRefreshHandler(const char *pData);
int cli_msgAllHandler(const char *pData);
int cli_msgClearHandler(const char *pData);
int cli_msgSessionHandler(const char *pData);

int cli_reloadHandler(const char *pData);
int cli_reactionHandler(const char *pData);

int cli_sendChannelDefaultHandler(const char *pData);
int cli_sendChannelOpenHandler(const char *pData);

int cli_sendCidPlayHandler(const char *pData);

int cli_sendDspDefaultHandler(const char *pData);
int cli_sendDspStartHandler(const char *pData);
int cli_sendDspStopHandler(const char *pData);

int cli_sendRingStartHandler(const char *pData);
int cli_sendRingStopHandler(const char *pData);


int cli_sendSlicControlHandler(const char *pData);
int cli_sendSlicLocaleHandler(const char *pData);

int cli_sendTonePlayHandler(const char *pData);

int cli_sendRtpActiveHandler(const char *pData);
int cli_sendRtpDeactiveHandler(const char *pData);


#endif // __CLI_CMDHANDLER_H__
