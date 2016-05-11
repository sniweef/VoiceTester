/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt_send.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		13Oct14
 *
 * history 	\arg
 */

#ifndef __VT_SEND_H__
#define __VT_SEND_H__

#include <base/voip_comm.h>

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
VOIP_DSPMSG_CMD vt_getCmdByTxt(const char *pMsgText);
char * vt_cmTxtMapById(int id);

int vt_shutdownSocket();
int vt_connectToServer(char *pIPStr, uint16_t port);
int vt_getVoIPSock();

int vt_sendSlicLocale(const char *localeAlpha2);
int vt_sendSlicControl(int endpt, VOIP_SLICCTRL ctrl);

int vt_sendDspDefault(const char *localeAlpha2);
int vt_sendDspStart();
int vt_sendDspStop();

int vt_sendRingStart(int endpt, int type);
int vt_sendRingStop(int endpt);

int vt_sendRtpActive(int endpt, int32_t remoteAddr, int16_t remotePort, int16_t remoteCtlport,
                        VOIP_MEDIA_DIRECTION direction, VOIP_DTMF_RELAY dtmfMethod, int payloadTye, int ptime);
int vt_sendRtpDeactive(int endpt);

int vt_sendChannelDefault(int channel);
int vt_sendChannelOpen(int endpt, int channel, int vadEnable, int gainSpeaker, int gainMic, int t38Enable);

int vt_sendTonePlay(int endpt, EPSIG tone, int time);
int vt_sendCidPlay(int channel, const char num[], const char name[]);

#endif // __VT_SEND_H__
