/*  Copyright(c) 2009-2011 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		voip_comm.h
 * brief	common header for voip server & client
 * details
 *
 * author	Mark Lee
 * version	1.0.0
 * date		19Jul11
 *
 * history 	\arg
 */

#ifndef _VOIP_COMM_H_
/*
 * brief	prevent recursive including
 */
#define _VOIP_COMM_H_

#include <stdint.h>

#include "voip_locale.h"

#define BOOL	int
#define TRUE	1
#define FALSE	0
#define BUFLEN_128	128

#ifdef UNKNOWN
#undef UNKNOWN
#endif /* UNKNOWN */
/*
 * brief	unknown
 */
//#define UNKNOWN			((uint32_t)-1)
#define UNKNOWN			(-1)


/*
 * brief	socket between CM and DSP
 */
#define CMDSP_SOCK_PATH     "/var/tmp/sock_CM_DSP"

#if VDEBUG
#define DBG_SIG
#endif /* VDEBUG */

#ifndef NUM_FXS_CHANNELS
/*
 * brief	number of FxS port
 */
#define NUM_FXS_CHANNELS		2
#endif /* NUM_FXS_CHANNELS */

#ifdef INCLUDE_PSTN
/*
 * brief	FXO channels
 */
#define NUM_FXO_CHANNELS        1

#ifdef INCLUDE_PSTN_LIFELINE
/*
 * brief	which FXS connected to the relay
 */
#define FXO_RELAY_FXS				1
#endif /* INCLUDE_PSTN_LIFELINE */

#ifdef RALINK
/*
 * brief	pstn state get file name
 */
#define PSTN_CHECK_PATH "/proc/pstn_check_stats"
#endif /* RALINK */

#else /* INCLUDE_PSTN */
/*
 * brief	FXO channels
 */
#define NUM_FXO_CHANNELS        0
#endif /* INCLUDE_PSTN */
/*
 * brief	DTMF MAKE TIME 100ms
 */
#define DTMF_SEND_MAKE			100
/*
 * brief	DTMF BREAK TIME 100ms
 */
#define DTMF_SEND_BREAK			100
/*
 * brief	total FXS & FXO channels
 */
#define MAX_ENDPT               (NUM_FXS_CHANNELS + NUM_FXO_CHANNELS)
/*
 * brief	maximum connection for DSP
 *			ch0/2 for line0,so even the FXS_CHANNELS is 1
 *			we still need at least 4 channels
 */
#define MAX_CNX					(2 * 2 + NUM_FXO_CHANNELS )
/*
 * brief	host address length
 */
#define MAX_HOSTADDR_LEN                    64
/*
 * brief	maximum endpoint
 */
#define MAX_NUM_ACTIVELINES                 MAX_ENDPT
/*
 * brief	maximum user id length
 */
#define MAX_USERID_LEN                      40
/*
 * brief	maximum user name length
 */
#define MAX_USERNAME_LEN                    40
/*
 * brief	maximum user passwd length
 */
#define MAX_USERPASSWD_LEN                  40
/*
 * brief	Max size of dialstring allowed in
 */
#define MAX_PSTN_DIALSTRING_LEN				32
/*
 * brief	maximum ring times
 */
#define MAX_RING_TIMES						10
/*
 * brief	compound h&l to a DWORD
 */
#define COMPOUND_DWORD(h, l) (((uint32_t)l & (uint32_t)0xffff) | ((uint32_t)h << (uint32_t)16))
/*
 * brief	compound h&l to a WORD
 */
#define COMPOUND_WORD(h, l) (((uint16_t)l & (uint16_t)0xff) | ((uint16_t)h << (uint32_t)8))
/*
 * brief	compound h&l to a BYTE
 */
#define COMPOUND_BYTE(h, l) (((uint8_t)l & (uint8_t)0x0f) | ((uint8_t)h << (uint8_t)4))
/*
 * brief	high word of an uint32_t
 */
#define HIGHWORD(x)		((uint32_t)x >> (uint32_t)16)
/*
 * brief	low word of an uint32_t
 */
#define LOWWORD(x)		((uint32_t)x & (uint32_t)0xffff)

/*
 * brief	high byte of an uint16_t
 */
#define HIGHBYTE(x)		((uint16_t)x >> (uint16_t)8)
/*
 * brief	low byte of an uint16_t
 */
#define LOWBYTE(x)		((uint16_t)x & (uint16_t)0xff)
/*
 * brief	ring pattern
 */
typedef enum _VOIP_RINGPATTERN_
{
	VOIP_RING_L = 0,	/* long pattern */
	VOIP_RING_M_M = 1,	/* med-med pattern */
	VOIP_RING_S_S_M = 2,/* short-short-med */
	VOIP_RING_S_M_S = 3,/* short-med-short */
	VOIP_RING_S_S_S = 4,/* short-short-short */
	VOIP_RING_M_S_S = 5,/* med-short-short */
	VOIP_RING_S = 6,	/* short pattern */
	VOIP_RING_LT = 7,	/* longest pattern */
	VOIP_RING_S_S = 8,	/* short-short pattern */
	VOIP_RING_M_M_S = 9,/* med-med-short pattern,specially for VMWI */
	VOIP_RING_BELLCORE_DR2 = 10, /* */
	VOIP_RING_BELLCORE_DR3 = 11, /* */
	VOIP_RING_MAX = 12,	/* max pattern */

	VOIP_RING_CALLW1 = 13,	/* call waiting tone */
	VOIP_RING_VMWI = 14,	/* VMWI */
} VOIP_RINGPATTERN;

/*
 * brief	RING type for MWI
 */
#define CLID_MWI_RING_TYPE              VOIP_RING_M_M_S
/*
 * brief	map dtmf/tone between CM and DSP/SIP
 */
typedef struct
{
	uint32_t cmIndex;	/* dtmf/tone index of CM */
	uint32_t dspIndex;/* dtmf/tone index of DSP or SIP */
} INDEXMAP;

/*
 * brief	DTMF index for DSP
 */
typedef enum
{
	VOIP_DTMF_0 = 100, /* '0' */
	VOIP_DTMF_1 = 101, /* '1' */
	VOIP_DTMF_2 = 102, /* '2' */
	VOIP_DTMF_3 = 103, /* '3' */
	VOIP_DTMF_4 = 104, /* '4' */
	VOIP_DTMF_5 = 105, /* '5' */
	VOIP_DTMF_6 = 106, /* '6' */
	VOIP_DTMF_7 = 107, /* '7' */
	VOIP_DTMF_8 = 108, /* '8' */
	VOIP_DTMF_9 = 109, /* '9' */
	VOIP_DTMF_STAR = 110, /* '*' */
	VOIP_DTMF_HASH = 111, /* '#' */

	VOIP_DTMF_MAX /* max index */
} VOIP_DTMF_DIGIT;

/*
 * brief	Signals generated
 */
typedef enum
{
	EPSIG_NULL = 0,						/* internal use: Null signal */
	EPSIG_BUSY = 1,						/* busy tone */
	EPSIG_NETBUSY = 2,					/* Network busy tone */
	EPSIG_CONF = 3,						/* confirmation tone */
	EPSIG_DIAL = 4,						/* dial tone */
	EPSIG_MSGW = 5,						/* message waiting indicator tone */
	EPSIG_OHWARN = 6,					/* off-hook warning tone */
	EPSIG_REORDER = 7, 					/* reorder tone */
	EPSIG_SPLASH = 8,					/* ring splash tone */
	EPSIG_RINGBACK = 9,					/* ringback tone */
	EPSIG_STUTTER = 10, 				/* stutter dial tone */
	EPSIG_INTRUSION = 11,				/* intrusion tone (Norway) */
	EPSIG_WARNING = 12, 				/* warning tone (Norway) */
	EPSIG_DIAL2 = 13,					/* dial tone 2 (Norway's centrex dial tone) */
	EPSIG_RINGBACK_CUST1 = 14,			/* Custom ringback tone 1 */
	EPSIG_SPECIAL_DIAL = 15,			/* Special dial tone */
	EPSIG_CALL_EXIT = 16,				/** anybody in 3-party or a held call exit */
	EPSIG_CALL_START = 17,				/** a warm call or re-dial call on busy start */
	EPSIG_LAST_TONE = 29,				/* the last tone */

	EPSIG_CALLWT = 30,					/* call waiting tone */
	EPSIG_CALLW1 = 31,					/* call waiting tone 1 */
	EPSIG_CALLW2 = 32,					/* call waiting tone 2 */
	EPSIG_CALLW3 = 33,					/* call waiting tone 3 */
	EPSIG_CALLW4 = 34,					/* call waiting tone 4  */
	EPSIG_SAS_CAS = 35, 				/* combined SAS and CAS tone for CLASS2 */
	EPSIG_SAS_CAS1 = 36,				/* combined SAS and CAS tone for CLASS2 */
	EPSIG_SAS_CAS2 = 37,				/* combined SAS and CAS tone for CLASS2 */
	EPSIG_SAS_CAS3 = 38,				/* combined SAS and CAS tone for CLASS2 */
 	EPSIG_SAS_CAS4 = 39,				/* combined SAS and CAS tone for CLASS2 */

 	EPSIG_RINGING = 40,                 /* enable ringer */
	EPSIG_RING0 = 41,                   /* distinctive ringing 0 */
	EPSIG_RING1 = 42,                   /* distinctive ringing 1 */
	EPSIG_RING2 = 43,                   /* distinctive ringing 2 */
	EPSIG_RING3 = 44,                   /* distinctive ringing 3 */
	EPSIG_RING4 = 45,                   /* distinctive ringing 4 */
	EPSIG_RING5 = 46,                   /* distinctive ringing 5 */
	EPSIG_RING6 = 47,                   /* distinctive ringing 6 */
	EPSIG_RING7 = 48,                   /* distinctive ringing 7 */
	EPSIG_CALLID_RINGING = 49,          /* enable ringer, pending callid */
	EPSIG_CALLID_RING0 = 50,            /* distinctive ringing 0, pending callid */
	EPSIG_CALLID_RING1 = 51,            /* distinctive ringing 1, pending callid */
	EPSIG_CALLID_RING2 = 52,            /* distinctive ringing 2, pending callid */
	EPSIG_CALLID_RING3 = 53,            /* distinctive ringing 3, pending callid */
	EPSIG_CALLID_RING4 = 54,            /* distinctive ringing 4, pending callid */
	EPSIG_CALLID_RING5 = 55,            /* distinctive ringing 5, pending callid */
	EPSIG_CALLID_RING6 = 56,            /* distinctive ringing 6, pending callid */
	EPSIG_CALLID_RING7 = 57,            /* distinctive ringing 7, pending callid */

 	EPSIG_DTMF0 = 60,                   /* DTMF Tone 0 */
	EPSIG_DTMF1 = 61,                   /* DTMF Tone 1 */
	EPSIG_DTMF2 = 62,                   /* DTMF Tone 2 */
	EPSIG_DTMF3 = 63,                   /* DTMF Tone 3 */
	EPSIG_DTMF4 = 64,                   /* DTMF Tone 4 */
	EPSIG_DTMF5 = 65,                   /* DTMF Tone 5 */
	EPSIG_DTMF6 = 66,                   /* DTMF Tone 6 */
	EPSIG_DTMF7 = 67,                   /* DTMF Tone 7 */
	EPSIG_DTMF8 = 68,                   /* DTMF Tone 8 */
	EPSIG_DTMF9 = 69,                   /* DTMF Tone 9 */
	EPSIG_DTMFS = 70,                   /* DTMF Tone * */
	EPSIG_DTMFH = 71,                   /* DTMF Tone # */
	EPSIG_DTMFA = 72,                   /* DTMF Tone A */
	EPSIG_DTMFB = 73,                   /* DTMF Tone B */
	EPSIG_DTMFC = 74,                   /* DTMF Tone C */
	EPSIG_DTMFD = 75,                   /* DTMF Tone D */

  	EPSIG_PSTNCALLSETUP = 80,			/* PSTN call setup */
	EPSIG_PSTN_SEND_DIGITS = 81,		/* PSTN send remaining digits */
	EPSIG_PSTNCALLSETUP_CONNECTONLY = 82,/*no dialtone detection */

	EPSIG_OFFHOOK = 90,                 /* Offhook - seize */
	EPSIG_ONHOOK = 91,                  /* Onhook - release */
	EPSIG_ANSWER = 92,                  /* Offhook - answer */

	/* DAA related signalling */
	EPSIG_DAA_DISABLE = 95,              /* disable DAA */
	EPSIG_DAA_ENABLE = 96,               /* enable DAA */

	EPSIG_LAST = 100                     /* the last signal */

} EPSIG;

/*
 * brief	date time
 */
#define CLID_DATETIME_LEN  8
/*
 * brief	CLID type
 */
typedef enum _VOIP_CLID_TYPE_
{
	VOIP_CLID_FSK_I = 0,	/* FSK type I */
	VOIP_CLID_FSK_II = 1,	/* FSK type II */
	VOIP_CLID_DTMF = 2, 	/* DTMF type */
	VOIP_CLID_VMWI = 3,		/* VMWI */

} VOIP_CLID_TYPE;

/*
 * brief	caller ID structure
 */
typedef struct _CLID_STRING
{
	VOIP_CLID_TYPE		type;		/* CLID type */
	VOIP_RINGPATTERN	ringtype;	/* ringing type */
	int		ringtime;				/* maximum ringing times */
	char	time[CLID_DATETIME_LEN + 1];/* date/time,we can use it to set the system time */
	char	name[MAX_USERNAME_LEN];	/* display name */
	char	num[MAX_USERID_LEN];	/* display number */
} CLID_STRING;

/*
 * brief	buffer length of the event from DSP to voip server
 */
#define BUFLEN_VOIP_CM_DSP_EVT  BUFLEN_512
/*
 * brief	buffer length of the msg between server and DSP
 */
#define BUFLEN_VOIP_CM_DSP_MSG  BUFLEN_512
/*
 * brief	buffer length of the msg between server and client
 */
#define BUFLEN_VOIP_CM_CS_MSG  BUFLEN_128
/*
 * brief	RTP connected to network
 */
#define RTP_PEER_NETWORK    ((uint16_t)(~(uint16_t)1))

/*
 * brief	SLIC control
 */
typedef enum VOIP_SLICCTRL
{
	VOIP_RELAY_ON = 0,       /* relay ON */
	VOIP_RELAY_OFF = 1,      /* relay OFF */
	VOIP_LED_ON = 2,         /* LED ON */
	VOIP_LED_OFF = 3,        /* LED OFF */
	VOIP_LED_BLINK_FAST = 4, /* LED blinks fast */
	VOIP_LED_BLINK_MED  = 5, /* LED blinks medium */
	VOIP_LED_BLINK_SLOW = 6, /* LED blinks slowly */
	VOIP_LED_BLINK_VERYSLOW = 7, /* LED blinks very slowly */

	VOIP_LED_RED_ON = 8,         /* LED ON */
	VOIP_LED_RED_OFF = 9,        /* LED OFF */
	VOIP_LED_RED_BLINK_FAST = 10, /* LED blinks fast */
	VOIP_LED_RED_BLINK_MED  = 11, /* LED blinks medium */
	VOIP_LED_RED_BLINK_SLOW = 12, /* LED blinks slowly */
	VOIP_LED_RED_BLINK_VERYSLOW = 13, /* LED blinks very slowly */

	VOIP_LED_ONCE = 0x10000000,
} VOIP_SLICCTRL;

/*
 * brief	media direction
 */
typedef enum VOIP_MEDIA_DIRECTION
{
	VOIP_MEDIA_NONE		= 0, /* none */
	VOIP_MEDIA_SENDONLY	= 1, /* remote to local */
	VOIP_MEDIA_RECVONLY	= 2, /* local to remote */
	VOIP_MEDIA_SENDRECV	= 3  /* send/receive */
} VOIP_MEDIA_DIRECTION;

typedef enum
{
	VOIP_CODEC_PCMU = 0,	/* G.711u */
	VOIP_CODEC_G726_32 = 1,	/* G.726-32 */
	VOIP_CODEC_PCMA = 2,	/* G.711A */
	VOIP_CODEC_G722 = 3,	/* G.722 */
	VOIP_CODEC_G729 = 4,	/* G.729A/B */
	VOIP_CODEC_T38 = 5,		/* T.38 */

	VOIP_CODEC_UNKNOWN
} VOIP_CODEC;

/*
 * brief	DTMF relay
 */
typedef enum VOIP_DTMF_RELAY
{
	VOIP_DTMF_NONE		= 0, /* do not relay */
	VOIP_DTMF_2833		= 1, /* RFC2833 */
	VOIP_DTMF_INBAND	= 2  /* inband */
} VOIP_DTMF_RELAY;

/*
 * brief	wan interface type
 */
typedef enum _WAN_INTF_TYPE
{
	PPPOE = 0,			/* PPPoE */
	PPPOA = 1,			/* PPPoA */
	IPOE = 2,			/* IPoE */
	IPOA = 3,			/* IPoA */
	INTF_UNKNOWN = 4,	/* unknown */
} WAN_INTF_TYPE;

/*
 * brief	SIP protocol
 */
typedef enum _SIP_PROT
{
	SIP_UDP = 0,  /* UDP */
	SIP_TCP = 1,  /* TCP */
	SIP_TLS = 2,  /* TCP/TLS */
} SIP_PROT;

/*
 * brief	media structure
 */
typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef struct _VOIP_MEDIA_
{
	UINT16					peer;		/* 0,1,2 for local interface, (uint16_t)(-1) for network */
	BOOL					isActive;	/* activated */
#if defined(CONFIG_IP_MULTIPLE_TABLES) && defined(INCLUDE_DSP_SOCKET_OPEN)
	int						fwmark;		/* flow mark for RTP/RTCP use */
#endif /* defined(CONFIG_IP_MULTIPLE_TABLES) && defined(INCLUDE_DSP_SOCKET_OPEN) */
	WAN_INTF_TYPE 			type;		/* for bandwidth use */
	SIP_PROT				prot;		/* for bandwidth use */
	UINT32					channel;    /* dsp channel */
#ifdef INCLUDE_PSTN
	BOOL					pstnConnect;/* specially for FxS<->FxO */
#endif /* INCLUDE_PSTN */
	VOIP_MEDIA_DIRECTION	direction;  /* rtp direction */
	VOIP_CODEC				voicecodec; /* codec type */
	int						txVoicePayloadtype; /* VOICEtx payload */
	int						rxVoicePayloadtype; /* VOICErx payload */
	int						faxProcessing; /* fax processing */
	int						voiceulptime;    /*up-link ptime. YuChuwei.20130606*/
	int						voicedlptime;	/*down-link ptime. YuChuwei.20130606*/
	VOIP_DTMF_RELAY			dtmfMethod;    /* DTMF relay */
	int						txdtmfpayloadtype; /* DTMFtx payload */
	int						rxdtmfpayloadtype; /* DTMFrx payload */
	UINT32					l_addr;			/* local address */
	UINT16					l_port;			/* local port */
	UINT32					r_addr;			/* remote address */
	UINT16					r_port;			/* remote port */
	UINT16					r_ctlport;		/* remote control port */
	UINT8					tos_rtp;		/* rtp	tos */
	UINT8					tos_rtcp;		/* rtcp tos */

} VOIP_MEDIA;

#ifdef S003_TEST
/*
 * brief	SLIC settings for S003 TEST
 */
typedef struct _VOIP_SLIC_SET_
{
	uint32_t	country;	/* country */
	uint32_t	txFxs;		/* tx gain of FxS */
	uint32_t	rxFxs;		/* rx gain of FxS */
#ifdef INCLUDE_PSTN
	uint32_t	txFxo;		/* tx gain of FxO */
	uint32_t	rxFxo;		/* rx gain of FxO */
#endif /* INCLUDE_PSTN */
} VOIP_SLIC_SET;
#endif /* S003_TEST */

/*
 * brief	msg command
 */
typedef enum _VOIP_DSPMSG_CMD_
{
	VOIP_DSP_DEFAULT = 0,		/* client->server init DSP with specified country */
	VOIP_SLIC_LOCALE = 1,		/* client->server change the locale for SLIC */
								/* server->client inform client that SLIC has been initialised */
	VOIP_DSP_START = 2,			/* client->server start DSP */
								/* server->client DSP status */
	VOIP_DSP_STOP = 3,			/* client->server stop DSP */
	VOIP_CID_PLAY = 4,			/* client->server play CID for FxS */
								/* server->client FxO detected CID */
	VOIP_RING_START = 5,		/* client->server ring FxS */
								/* server->client FxO detected RING */
	VOIP_RING_STOP = 6,			/* client->server stop ringing of FxS */
								/* server->client ringing of FxO finished */
	VOIP_CHANNEL_DEFAULT = 7,	/* client->server init channel */
	VOIP_CHANNEL_OPEN = 8,		/* client->server open a channel */
	VOIP_CHANNEL_UPDATE = 9,	/* client->server update a channel */
	VOIP_RTP_ACTIVE = 10,		/* client->server active RTP */
	VOIP_RTP_UPDATE = 11,		/* client->server update RTP */
	VOIP_RTP_DEACTIVE = 12,		/* client->server de-active RTP */
	VOIP_TONE_PLAY = 13,		/* client->server play tone for FxS */
								/* server->client FxO detected a tone */
	VOIP_TONE_STOP = 14,		/* client->server stop tone for FxS */
	VOIP_THREE_WAY_START = 15,	/* client->server start 3-way conference */
	VOIP_THREE_WAY_STOP = 16,	/* client->server stop 3-way conference */
	VOIP_HOOK_ON = 17,			/* client->server HOOKON FxO */
								/* server->client FxS/FxO detected HOOKON */
	VOIP_HOOK_OFF = 18,			/* client->server HOOKOFF FxO */
								/* server->client FxS/FxO detected HOOKOFF */
	VOIP_HOOK_FLASH = 19,		/* client<->server HOOKFLASH FxO */
								/* server->client FxS detected HOOKFLASH */
	VOIP_DTMF_END = 20,			/* server->client FxS/FxO detected DTMF */
	VOIP_FAX_START = 21,		/* server->client DSP detected FAX start */
	VOIP_FAXMODEM_END = 22,		/* server->client DSP detected FAXMODEM end */
	VOIP_SLIC_CONTROL = 23,		/* client->server control LEDs of FxS/FxO and relay */

	VOIP_DTMF_START = 24,		/* BosaZhong@31May2013, add, server->client FxS/FxO detected DTMF */
#if defined(INCLUDE_PSTN) && defined(MTK) /* Added by Yu Chuwei */
	VOIP_FXO_POL_FWD = 25,		/* server->client FXO connected and Polarity(1) */
	VOIP_FXO_POL_REV = 26,		/* server->client FXO connected and Polarity(0) */
	VOIP_FXO_DIS = 27,			/* server->client FXO disconnected */
	VOIP_FXO_STATE = 28,		/* server->client FXO report state */
	VOIP_FXO_LIU = 29,			/* server->client FXO Line In Use */
	VOIP_FXO_LNIU = 30,			/* server->client FXO Line Not In Use */
#endif /* defined(INCLUDE_PSTN) && defined(MTK) */
	VOIP_DSPMSG_CMDMAX,			/* max command */
} VOIP_DSPMSG_CMD;





/*
 * brief	msg for VOIP_FAX_START
 */
typedef struct _VOIP_FAX_MSG_
{
	uint32_t	endpt;	/* FxS */
	BOOL	sender; /* it's sender */
} VOIP_FAX_MSG;

/*
 * brief	msg for VOIP_DTMF_END
 */
typedef struct _VOIP_DTMF_MSG_
{
	uint32_t  endpt;		/* FxS or FxO */
	uint32_t  dtmf;		/* '0' ~ '9', '*', '#' */
} VOIP_DTMF_MSG;

/*
 * brief	msg for VOIP_CID_PLAY
 */
typedef struct _VOIP_CID_MSG_
{
	uint32_t  channel;		/* FxS or FxO */
	CLID_STRING clid;	/* caller ID */
} VOIP_CID_MSG;

/*
 * brief	tone direction
 */
typedef enum _TONE_DIR_
{
	TONE_LOCAL = 0,		/* to local */
	TONE_REMOTE = 1,	/* to stream */
	TONE_BOTH = 3,		/* to local and stream */
} TONE_DIRECTION;

/*
 * brief	msg for VOIP_TONE_PLAY
 */
typedef struct _VOIP_TONE_MSG_
{
	uint32_t  channel;/* 0~NUM_FXS_CAHNNELS-1 for FxS*/
					/* MAX_CNX - NUM_FXO_CHANNELS for FxO */
					/* or channel (specially for DTMF relay in conf call */
	EPSIG   tone;	/* tone */
	TONE_DIRECTION dir;/* direction */
	uint32_t	time;	/* play time */
} VOIP_TONE_MSG;

/*
 * brief	msg for VOIP_CHANNEL_OPEN
 */
typedef struct _VOIP_CHANNEL_MSG_
{
	uint32_t  channel;	/* dsp channel */
	int		txgain;		/* DSP->FxS/FxO */
	int     rxgain;		/* FxS/FxO->DSP */
	int		agcEnable;	/* AGC control */
	int     agcLocation;/* AGC location:0 - encoder input, 1-decoder output */
	int     dtmfMode;	/* DTMF mode */
	int     t38Enable;	/* T.38 enable */
	int     rtcpxrEnable;/* RTCPXR enable */
	int		vadEnable;	/* VAD enable */
	int		ecEnable;	/* EC enable */
} VOIP_CHANNEL_MSG;

/*
 * brief	msg for VOIP_THREE_WAY_START/VOIP_THREE_WAY_STOP
 */
typedef struct _VOIP_CONF_MSG_
{
	uint32_t   channel;   /* channel */
	uint32_t   mate;      /* mate */
} VOIP_CONF_MSG;

/*
 * brief	msg for VOIP_SLIC_CONTROL
 */
typedef struct _VOIP_SLIC_MSG_
{
	uint32_t   endpt;		/* FxS/FxO */
	VOIP_SLICCTRL ctrl; /* control mode */
} VOIP_SLIC_MSG;

/*
 * brief	msg for VOIP_RING_START/VOIP_RING_STOP
 */
typedef struct _VOIP_RING_MSG_
{
	uint32_t				endpt;		/* FxS */
	VOIP_RINGPATTERN	pattern;	/* ring pattern */
	int					maxtime;	/* ring time */
} VOIP_RING_MSG;

/*
 * brief	msg buffer between voip client & voip server
 */
typedef struct _VOIP_DSPMSG_
{
	VOIP_DSPMSG_CMD cmd;				/* command between server and client */
	char buf[BUFLEN_VOIP_CM_CS_MSG];	/* msg buffer */
} VOIP_DSPMSG;

/*
 * brief	record fxo state. YuChuwei
 */
#ifdef INCLUDE_PSTN
typedef struct _VOIP_FXOSTATE_
{
	int fxoState;
	int pol;
}VOIP_FXOSTATE;
#endif


/*
 * fn		uint32_t voip_convertDtmf( int idx )
 * brief	find the DTMF digit according to the DTMFMAP
 *
 * param[in]	map : pointer to a map
 * param[in]	idx : DTMF index from dsp
 *
 * return	DTMF digit
 * retval	DTMF if succeed, UNKNOWN if failed
 */
uint32_t voip_convertDtmf( const INDEXMAP *map, int idx );
#ifdef RALINK
/*
 * fn		void voip_setAffinity(char *task, uint32_t affinity)
 * brief	set affinity for task
 *
 * param[in]	task : process name
 * param[in]	aff : affinity
 */
void voip_setAffinity(char *task, uint32_t aff);
#endif /* RALINK */
/*
 * fn		static int voip_convertTone( int idx )
 * brief	find the DTMF digit according to the TONEMAP
 *
 * param[in]	idx : TONE index from CM
 *
 * return	tone index for DSP
 * retval	index if succeed, default BUSY_TONE if failed
 */
int voip_convertTone( int idx );

#ifdef DBG_SIG
#include <signal.h>
typedef void (*SIGNAL_HANDLER)( int sig, siginfo_t* siginfo, void* notused );

void signalHandler( int sig, siginfo_t* siginfo, void* notused);
void signalRegisterHandler( SIGNAL_HANDLER handler);
#endif /* DBG_SIG */

#endif /* _VOIP_COMM_H_ */

