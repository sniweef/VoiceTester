/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		vt.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		13Oct14
 *
 * history 	\arg
 */
#include <string.h>
#include <semaphore.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>

#include <base/common.h>
#include <base/voip_comm.h>
#include <cli/cli_cmdmap.h>
#include <vt/vt_send.h>
#include <vt/vt_action.h>
#include <vt/vt_event.h>
#include <vt/vt_msgQueue.h>
#include <vt/vt_fsm.h>
#include <vt/vt_config.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/


/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
typedef struct _CMTXTMAP
{
    int id;    /* index */
	char *txt; /* visible string */
} CMTXTMAP;

typedef struct _SERVER_STATES
{
    int fxoState;
	int dspStart;
} SERVER_STATES;

typedef struct _CONN_STATUS /*Status of connection to VOIP server*/
{
    //int started;
	int sockFd;             /*The socket which connect to VoIP Server. */
	pthread_t tid;        /*The id of thread which is reading message from server.*/
	pthread_t tid2;
} CONN_STATUS;
/**************************************************************************************************/
/*                                           EXTERN_PROTOTYPES                                    */
/**************************************************************************************************/
extern sem_t g_connectedSem;

/**************************************************************************************************/
/*                                           LOCAL_PROTOTYPES                                     */
/**************************************************************************************************/
LOCAL int sendMsgToServer(VOIP_DSPMSG_CMD cmd, void *buf, int len);
LOCAL int sendLocaleInternal(const char *pLocale, VOIP_DSPMSG_CMD cmd, BOOL isWait);
LOCAL void getSystime(struct tm *p_local);

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/
LOCAL SERVER_STATES l_serverState = {-1, 0};

LOCAL CONN_STATUS l_connStatus = {-1, 0, 0};
/*
 * brief	VOIP_DSPMSG_CMD and pjsua cmd
 */
LOCAL const CMTXTMAP l_srvMsgMap[] =
{
	STR(VOIP_DSP_DEFAULT),		/* client->server init DSP with specified country */
	STR(VOIP_SLIC_LOCALE),		/* client->server change the locale for SLIC */
								/* server->client inform client that SLIC has been initialised */
	STR(VOIP_DSP_START),			/* client->server start DSP */
								/* server->client DSP status */
	STR(VOIP_DSP_STOP),			/* client->server stop DSP */
	STR(VOIP_CID_PLAY),			/* client->server play CID for FxS */
								/* server->client FxO detected CID */
	STR(VOIP_RING_START),		/* client->server ring FxS */
								/* server->client FxO detected RING */
	STR(VOIP_RING_STOP),			/* client->server stop ringing of FxS */
								/* server->client ringing of FxO finished */
	STR(VOIP_CHANNEL_DEFAULT),	/* client->server init channel */
	STR(VOIP_CHANNEL_OPEN),		/* client->server open a channel */
	STR(VOIP_CHANNEL_UPDATE),	/* client->server update a channel */
	STR(VOIP_RTP_ACTIVE),		/* client->server active RTP */
	STR(VOIP_RTP_UPDATE),		/* client->server update RTP */
	STR(VOIP_RTP_DEACTIVE),		/* client->server de-active RTP */
	STR(VOIP_TONE_PLAY),		/* client->server play tone for FxS */
								/* server->client FxO detected a tone */
	STR(VOIP_TONE_STOP),		/* client->server stop tone for FxS */
	STR(VOIP_THREE_WAY_START),	/* client->server start 3-way conference */
	STR(VOIP_THREE_WAY_STOP),	/* client->server stop 3-way conference */
	STR(VOIP_HOOK_ON),			/* client->server HOOKON FxO */
								/* server->client FxS/FxO detected HOOKON */
	STR(VOIP_HOOK_OFF),			/* client->server HOOKOFF FxO */
								/* server->client FxS/FxO detected HOOKOFF */
	STR(VOIP_HOOK_FLASH),		/* client<->server HOOKFLASH FxO */
								/* server->client FxS detected HOOKFLASH */
	STR(VOIP_DTMF_END),			/* server->client FxS/FxO detected DTMF */
	STR(VOIP_FAX_START),		/* server->client DSP detected FAX start */
	STR(VOIP_FAXMODEM_END),		/* server->client DSP detected FAXMODEM end */
	STR(VOIP_SLIC_CONTROL),		/* client->server control LEDs of FxS/FxO and relay */

    STR(VOIP_DTMF_START),       /* HuangZhigeng@9Oct2014, add, server->client FxS/FxO detected DTMF */

	STR(VOIP_DSPMSG_CMDMAX),	/* max command */

	{ UNKNOWN, "*unknown*" },
};


/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/
LOCAL void initConnStatus()
{
    if (l_connStatus.sockFd != -1)
    {
        CX_LOGL(CX_DBG, "Shutdown previous socket. fd=%d", l_connStatus.sockFd);
        CX_LOGL(CX_INFO, "Shutdown return %d.", shutdown(l_connStatus.sockFd, SHUT_RDWR));
        l_connStatus.sockFd = -1;
    }

    //vt_initSessionStatus();
}


/*
 * fn		int sendMsgToServer(VOIP_DSPMSG_CMD cmd, void *buf, int len)
 * brief	send message to dsp
 *
 * param[in]	cmd : such as DSP_START, DSP_CLOSE etc.
 * param[in]    buf : detail message
 * param[in]    len : message buffer lenght
 *
 * return	msg sending result
 * retval	CMM_OK success , other failed
 */
LOCAL int sendMsgToServer(VOIP_DSPMSG_CMD cmd, void *buf, int len)
{
	VOIP_DSPMSG msg;
	int n;

	memset(&msg, 0, sizeof(msg));
	assert(len < (int)sizeof(msg.buf));
	assert(/* cmd >= 0 && */ cmd < VOIP_DSPMSG_CMDMAX);
	msg.cmd = htonl(cmd);
	if ( len )
	{
		assert(buf);
		memcpy(msg.buf, buf, len);
	}

    int sockFd = vt_getVoIPSock();

	if ( sockFd >= 0 )
	{
		n = sock_tcpWrite(sockFd, &msg, sizeof(msg));
		if (SOCKERR_IO == n)
		{
			CX_LOGL(CX_ERR, "write error on fd %d\n", sockFd);
		}
		else if (SOCKERR_CLOSED == n)
		{
			CX_LOGL(CX_ERR, "fd %d has been closed.\n", sockFd);
		}
		else
		{
			CX_LOGL(CX_DUMP, "wrote to server succeed, msg type %d:%s.\n", cmd,
				vt_cmTxtMapById(cmd));

			return VT_SUCCESS;
		}
	}
	else
	{
		CX_LOGL(CX_ERR, "socket not conneted, ignore this msg.\n" );
	}

	return VT_FAILED;
}
LOCAL int sendLocaleInternal(const char *pLocale, VOIP_DSPMSG_CMD cmd, BOOL isWait)
{
    uint8_t found;
    int countryInt = 0;
    int netData;
    int ret;

    ret = oalVoice_mapAlpha2toVrg(pLocale, &countryInt, &found, 2);

    if (ret != 0)
    {
        CX_LOGL(CX_ERR, "unknow region = %s.", pLocale);
        return VT_FAILED;
    }

    netData = htonl(countryInt);

    //vt_msgRefresh(); /*Refresh message to make sure the next message is what we need.*/
    ret = sendMsgToServer(cmd, &netData, sizeof(netData));

    if(ret == VT_SUCCESS && isWait)
    {
        //vt_waitMsg(cmd);
    }
    return ret;
}
/*
 * fn		static void voip_getSystime(struct tm *p_local)
 * brief	get correct local time
 *
 * param[out]	p_local : local time pointer
 */
LOCAL void getSystime(struct tm *p_local)
{
	time_t calendar_time;

	calendar_time = time(NULL);

	if (-1 != calendar_time)
	{
		localtime_r(&calendar_time, p_local);
	}
}

/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
int vt_connectToServer(char *pIPStr, uint16_t port)
{
    if (pIPStr == NULL)
    {
        pIPStr = VOIP_SERVER_IP_STR;
    }
    if (port == 0)
    {
        port = g_vtConfig.serverPort;
    }

    initConnStatus();

    l_connStatus.sockFd = sock_tcpConnect(pIPStr, 0, port);

    if (l_connStatus.sockFd > 0)
    {
        sem_init(&g_connectedSem, 0, 0);
        vt_initMsgQueue();

        if (l_connStatus.tid == 0) /*NOTE: pthread_t is unsigned long int.*/
        {
            pthread_create(&l_connStatus.tid, NULL, vt_listenEvent, &l_connStatus.sockFd);
        }
        if (l_connStatus.tid2 == 0)
        {
            pthread_create(&l_connStatus.tid2, NULL, vt_processEvent, NULL);
        }

        int currentSemValue;
        sem_getvalue(&g_connectedSem, &currentSemValue);

        if (currentSemValue <= 0)
        {
            /*Avoid that g_connectedSem is bigger than 1.*/
            CX_LOGL(CX_INFO, "g_connectedSem value : %d, so post g_connectedSem.", currentSemValue);
            sem_post(&g_connectedSem);
        }
        /*When connecting successfully, server will send VOIP_DSP_START.*/
        //vt_waitMsg(VOIP_DSP_START);
    }

    return l_connStatus.sockFd;
}
int vt_shutdownSocket()
{
    if (l_connStatus.sockFd > 0)
    {
        int ret = shutdown(l_connStatus.sockFd, SHUT_RDWR);
        CX_LOGL(CX_DBG, "socket %d has been close. ret=%d", l_connStatus.sockFd, ret);
        l_connStatus.sockFd = -1;
    }
    else
    {
        CX_LOGL(CX_ERR, "No socket connecting to VoIP Server.");
    }
    return VT_SUCCESS;
}

int vt_getVoIPSock()
{
    return l_connStatus.sockFd;
}

VOIP_DSPMSG_CMD vt_getCmdByTxt(const char *pMsgText)
{
    const CMTXTMAP *pMap = l_srvMsgMap;

    if (pMsgText == NULL)
    {
        return VOIP_DSPMSG_CMDMAX;
    }

	while (pMap->id != (int)UNKNOWN && strcmp(pMsgText, pMap->txt) != 0)
	{
		pMap++;
	}
	return pMap->id;
}
/*
 * fn		char * vt_cmTxtMapById(int id)
 * brief	Translate id to application text string
 *
 * param[in]	id : index
 *
 * return	readable string
 */
char * vt_cmTxtMapById(int id)
{
    const CMTXTMAP *pMap = l_srvMsgMap;
    if (pMap[id].id == id)
    {
        return pMap[id].txt; /*Make function faster.*/
    }

	while (pMap ->id != (int)UNKNOWN && pMap->id != id)
	{
		pMap++;
	}
	return pMap->txt;
}

int vt_sendSlicLocale(const char *pLocale)
{
    return sendLocaleInternal(pLocale, VOIP_SLIC_LOCALE, TRUE);
}
int vt_sendDspDefault(const char *pLocale)
{
    return sendLocaleInternal(pLocale, VOIP_DSP_DEFAULT, FALSE);
}
int vt_sendDspStart()
{
    int netData;
    int ret = 0;

    if (!l_serverState.dspStart)
    {
        //vt_msgRefresh();

        l_serverState.dspStart = 1;
        netData = htonl(1);
        ret = sendMsgToServer(VOIP_DSP_START, &netData, sizeof(netData));
        if (ret == VT_SUCCESS)
        {
            //vt_waitMsg(VOIP_DSP_START);
        }
    }
    else
    {
        CX_LOGL(CX_WARN, "DSP has started. Ingore this message.");
    }

    return ret;
}
int vt_sendDspStop()
{
    int netData;
    int ret = 0;

    if (l_serverState.dspStart == 1)
    {
        l_serverState.dspStart = 0;
        netData = htonl(l_serverState.dspStart);
        ret = sendMsgToServer(VOIP_DSP_STOP, &netData, sizeof(netData));
    }
    else
    {
        CX_LOGL(CX_WARN, "DSP has stopped. Ingore this message.");
    }

    return ret;
}

int vt_sendChannelDefault(int channel)
{
    int netData;

    netData = htonl(channel);
    return sendMsgToServer(VOIP_CHANNEL_DEFAULT, &netData, sizeof(netData));
}
int vt_sendRingStart(int endpt, int type)
{
    VOIP_RING_MSG ring;
    int ret;

    if (endpt > VT_ENDPT_MAX)
    {
        CX_LOGL(CX_ERR, "endpt too big: %d", endpt);
        return VT_FAILED;
    }

    CX_LOGL(CX_DBG, "sendRingStart start. endpt=%d, type=%d", endpt, type);

    ring.endpt = htonl(endpt);
    ring.pattern = htonl((VOIP_RINGPATTERN)type);
    ring.maxtime = htonl(-1);
    ret = sendMsgToServer(VOIP_RING_START, &ring, sizeof(ring));

    //l_sessionStatus[endpt].fxsState = VOIP_RING_START;
    return ret;
}
int vt_sendRingStop(int endpt)
{
    if (endpt > VT_ENDPT_MAX)
    {
        CX_LOGL(CX_ERR, "endpt too big: %d", endpt);
        return VT_FAILED;
    }

    int netData;
    int ret;

    CX_LOGL(CX_DBG, "sendRingStop start. endpt=%d", endpt);

    netData = htonl(endpt);
    ret = sendMsgToServer(VOIP_RING_STOP, &netData, sizeof(endpt));

    //l_sessionStatus[endpt].fxsState = VOIP_RING_STOP;
    return ret;
}
int vt_sendSlicControl(int endpt, VOIP_SLICCTRL ctrl)
{
    if (endpt > VT_ENDPT_MAX)
    {
        CX_LOGL(CX_ERR, "endpt too big: %d", endpt);
        return VT_FAILED;
    }

    VOIP_SLIC_MSG msg;
    CX_LOGL(CX_DBG, "vt_sendSlicControl start. endpt=%d, ctrl=%d", endpt, ctrl);

    msg.endpt = htonl(endpt);
    msg.ctrl = htonl(ctrl);
    return sendMsgToServer(VOIP_SLIC_CONTROL, &msg, sizeof(msg));
}
int vt_sendTonePlay(int endpt, EPSIG tone, int time)
{
    VOIP_TONE_MSG msg;

    if (endpt > VT_ENDPT_MAX)
    {
        CX_LOGL(CX_ERR, "endpt(%d) too big. max value is %d.", endpt, VT_ENDPT_MAX);
        return VT_FAILED;
    }

    CX_LOGL(CX_DBG, "tone play start. endpt=%d, tone=%d, time=%d", endpt, tone, time);

    msg.dir = 0;
    msg.channel = htonl(endpt);
    msg.tone = htonl(tone);
    msg.time = htonl(time);
    return sendMsgToServer(VOIP_TONE_PLAY, &msg, sizeof(msg));
}
int vt_sendCidPlay(int channel, const char num[], const char name[])
{
    if (channel < 0 || channel > MAX_CNX)
    {
        CX_LOGL(CX_ERR, "channel(%d) not invalid.", channel);
        return VT_FAILED;
    }

    VOIP_CID_MSG msg;

    struct tm localTm = {0, 0, 0, 1, 1, 2014, 0, 0, 0, 0, 0};

    memset(&msg, 0, sizeof(msg));

    getSystime(&localTm);
    sprintf(msg.clid.time,
            "%02d%02d%02d%02d",
            localTm.tm_mon + 1,
            localTm.tm_mday,
            localTm.tm_hour,
            localTm.tm_min);

    msg.channel = htonl(channel);

    str_strncpy(msg.clid.num, num, sizeof(msg.clid.num));
    str_strncpy(msg.clid.name, name, sizeof(msg.clid.name));

    CX_LOGL(CX_DBG, "cid play: channel=%d, number=%s, name=%s", channel, num, name);

    return sendMsgToServer(VOIP_CID_PLAY, &msg, sizeof(msg));
}

int vt_sendChannelOpen(int endpt, int channel, int vadEnable, int gainSpeaker, int gainMic,
                          int t38Enable)
{
    if (channel < 0)
    {
        channel = endpt;
    }

    if (channel < 0 || channel > MAX_CNX)
    {
        CX_LOGL(CX_ERR, "channel %d no correct.", channel);
        return VT_FAILED;
    }
    if (endpt < 0 || endpt > MAX_ENDPT)
    {
        CX_LOGL(CX_ERR, "endpt %d no correct.", endpt);
        return VT_FAILED;
    }

    VOIP_CHANNEL_MSG msg;
    int ret;

    memset(&msg, 0, sizeof(msg));

    CX_LOGL(CX_DBG, "ch %d gainspk %d gainmic %d agc %d agcLoc %d t38 %d rtcpxr %d vad %d ec %d",
                        channel, gainSpeaker, gainMic, 1, 0, t38Enable,  0, vadEnable, 1);

    msg.channel = htonl(channel);
    msg.vadEnable = htonl(vadEnable);
    msg.rxgain = htonl(gainSpeaker);
    msg.txgain = htonl(gainMic);
    msg.agcEnable = htonl(1);
    msg.agcLocation = htonl(0);
    msg.t38Enable = htonl(t38Enable);
    msg.rtcpxrEnable = htonl(0);
    msg.ecEnable = htonl(1);

    ret = sendMsgToServer(VOIP_CHANNEL_OPEN, &msg, sizeof(msg));

    if (ret == VT_SUCCESS)
    {
        vt_setOpenChannel(endpt, channel);
    }
    return ret;
}

int vt_sendRtpActive(int endpt, int32_t remoteAddr, int16_t remotePort, int16_t remoteCtlport,
                        VOIP_MEDIA_DIRECTION direction, VOIP_DTMF_RELAY dtmfMethod, int payloadTye, int ptime)
{

    if (vt_getOpenChannel(endpt) < 0)
    {
        CX_LOGL(CX_INFO, "Will open a channel first.");
        if (vt_sendChannelOpen(endpt, endpt, 0, 0, 0, 0) == VT_FAILED)
        {
            return VT_FAILED;
        }
    }

    int openChannel = vt_getOpenChannel(endpt);

    CX_LOGL(CX_DBG, "RtpActive: endpt=%d reIP=%x rePort=%d reCtlp=%d direct=%d dtmf=%d pt=%d ptime=%d. OpenChannel:%d", endpt,
            remoteAddr, (int)remotePort, (int)remoteCtlport, direction, dtmfMethod, payloadTye, ptime, openChannel);
    CX_LOGL(CX_DBG, "RtpActive: localaddr=%x localPort=%d", g_vtConfig.serverIP, g_vtConfig.endptInfo[endpt].port);

	VOIP_MEDIA voipMedia;
	bzero(&voipMedia, sizeof(VOIP_MEDIA));

	voipMedia.isActive = htonl(1);

	voipMedia.channel = htonl(openChannel);
    voipMedia.direction = htonl(direction);
    voipMedia.txVoicePayloadtype = voipMedia.rxVoicePayloadtype = htonl(payloadTye);
    voipMedia.voiceulptime = voipMedia.voicedlptime = htonl(ptime);
    voipMedia.dtmfMethod = htonl(dtmfMethod);
    voipMedia.l_addr = htonl(g_vtConfig.serverIP);
    voipMedia.l_port = htons(g_vtConfig.endptInfo[endpt].port);             /* Use htons instead of htonl*/
    voipMedia.r_addr = htonl(remoteAddr);
    voipMedia.r_port = htons(remotePort);
    voipMedia.r_ctlport = htons(remoteCtlport);

	return sendMsgToServer(VOIP_RTP_ACTIVE, &voipMedia, sizeof(VOIP_MEDIA));
}
int vt_sendRtpDeactive(int endpt)
{
    if (vt_getOpenChannel(endpt) < 0)
    {
        CX_LOGL(CX_ERR, "The endpt %d has not open a channel!", endpt);
        return VT_FAILED;
    }

    int openChannel = vt_getOpenChannel(endpt);
    CX_LOGL(CX_DBG, "rtp deactive, endpt=%d, channel=%d", endpt, openChannel);

    /*VOIP_MEDIA voipMedia;
	bzero(&voipMedia, sizeof(VOIP_MEDIA));

    //voipMedia.isActive = htonl(0);
	voipMedia.channel = htonl(openChannel);*/

    openChannel = htonl(openChannel);

	return sendMsgToServer(VOIP_RTP_DEACTIVE, &openChannel, sizeof(openChannel));
}
int vt_sendHookFlash()
{
    uint32_t endpt = NUM_FXS_CHANNELS;
    uint32_t netData = htonl(endpt);

    CX_LOGL(CX_WARN, "VOIP_HOOK_FLASH probably not a command but a notify\n");

    sendMsgToServer(VOIP_HOOK_ON, &netData, sizeof(netData));
    usleep(100*1000);
    sendMsgToServer(VOIP_HOOK_OFF, &netData, sizeof(netData));

    return VT_SUCCESS;
}
int vt_sendHookOff()
{
    uint32_t endpt = NUM_FXS_CHANNELS;
    uint32_t netData = htonl(endpt);
    sendMsgToServer(VOIP_HOOK_OFF, &netData, sizeof(netData));

    l_serverState.fxoState = VOIP_HOOK_OFF;
    return VT_SUCCESS;
}
int vt_sendHookOn()
{
    uint32_t endpt = NUM_FXS_CHANNELS;
    uint32_t netData = htonl(endpt);
    sendMsgToServer(VOIP_HOOK_ON, &netData, sizeof(netData));

    l_serverState.fxoState = VOIP_HOOK_ON;
    return VT_SUCCESS;
}
