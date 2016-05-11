#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <base/common.h>
#include <base/voip_comm.h>
#include <voiphelper/voiphelper_config.h>


#define BACKLOG 5


typedef enum _CALL_MSG_TYPE
{
    CALL_NUMBER = VOIP_DSPMSG_CMDMAX + 4,
    CALL_ACCEPT,
    CALL_DOWN,
    CALL_REJECT,
    CALL_MSG_MAX,
} CALL_MSG_TYPE;
//todo: htonl ntol
typedef struct _CALL_MSG
{
    CALL_MSG_TYPE       callMsgType;
    uint32_t            srcIP;
    uint16_t            srcPort;
    uint16_t            srcCtrport;
    unsigned long long  srcNumber;
    unsigned long long  destNumber;
} CALL_MSG;
typedef struct _SESSION_STATUS  /**/
{
    unsigned long long callNumber;  /*The number user want to call*/
    int fxsState;
    int openChannel;
    int destIP;
    int destPort;
    int destCtrport;
    int connectfd;
    unsigned long long destNumber;  /*Once the session is created, it stores the number of the other side.*/
} SESSION_STATUS;

typedef struct _PAYLOAD_TYPE_MAP
{
    int payloadType;
    char *pEncodeMethod;
} PAYLOAD_TYPE_MAP;

LOCAL void *handleClient(void *pArg);

LOCAL SESSION_STATUS l_sessionStatus;
LOCAL PAYLOAD_TYPE_MAP payloadTypeMap[] =
{
    {0, "PCMU"},
    {8, "PCMA"},
    {-1, "NULL"},
};
pid_t l_sendVoicePid = 0;


LOCAL void initSessionStatus()
{
    l_sessionStatus.callNumber = 0;
    l_sessionStatus.openChannel = -1;
    l_sessionStatus.fxsState = VOIP_HOOK_ON;
    l_sessionStatus.destIP = 0;
    l_sessionStatus.destPort = -1;
    l_sessionStatus.destCtrport = -1;
    l_sessionStatus.connectfd = -1;
}


LOCAL pid_t sendVoice(uint32_t ip, uint16_t port, int payloadType)
{
    pid_t pid = fork();
    int index = 0;

    char ipPortStr[BUFF_MAX] = "";
    char encodeStr[BUFF_MAX] = "";
    char *pEncodeName;

    while (payloadTypeMap[index].payloadType != -1 && payloadTypeMap[index].payloadType != payloadType)
    {
        index++;
    }
    pEncodeName = payloadTypeMap[index].pEncodeMethod;

    if (pid < 0)
    {
        CX_LOGL(CX_FATAL, "fork failed!. errno = %d", errno);
    }
    else if (pid == 0)
    {
        str_strncpy(encodeStr, pEncodeName, BUFF_MAX);
        for (index = 0; index < g_voiphConfig.sendVoiceTimes - 1; index++)
        {
            str_strncat(encodeStr, ",", BUFF_MAX);
            str_strncat(encodeStr, pEncodeName, BUFF_MAX);
        }

        snprintf(ipPortStr, BUFF_MAX, "%d:%u", (int)ip, port);

        CX_LOGL(CX_DBG, "Sub process. Sending voice...");
        execl("./wav2rtp", "./wav2rtp", "-f", "8kulaw.wav", "-t", ipPortStr, "-c", encodeStr, (char *)NULL);
    }

    l_sendVoicePid = pid;
    return pid;
}

int call(unsigned long long callNumber, uint32_t ip, uint16_t port)
{
    CALL_MSG callMsg;
    int fd;
    int nwrite;
    pthread_t pid;

    callMsg.callMsgType = CALL_NUMBER;
    callMsg.destNumber = callNumber;
    callMsg.srcNumber = g_voiphConfig.number;
    callMsg.srcIP = g_voiphConfig.clientIP;
    callMsg.srcPort = g_voiphConfig.dspPort;
    callMsg.srcCtrport = g_voiphConfig.dspCtrlPort;

    CX_LOGL(CX_DBG, "Connect to %x in port %d.", ip, port);
    if ((fd = sock_tcpConnect(NULL, ip, port)) > 0)
    {
        //sleep(1); /*Wait for server accept*/

        l_sessionStatus.connectfd = fd;
        l_sessionStatus.callNumber = l_sessionStatus.destNumber = callNumber;
        nwrite = sock_tcpWrite(fd, &callMsg, sizeof(CALL_MSG));
        CX_LOGL(CX_DBG, "Write CALL_NUMBER to fd: %d. nwrite=%d", fd, nwrite);

        /* When connect to another client, we create a pthread to read information from it.*/
        pthread_create(&pid, NULL, handleClient, &l_sessionStatus.connectfd);

        if (nwrite > 0)
        {
            return 0;
        }
    }
    l_sessionStatus.fxsState = VOIP_CALLING;
    return -1;
}


int rejectCall()
{
    int sockfd = l_sessionStatus.connectfd;
    int nread;
    CALL_MSG callMsg;

    callMsg.callMsgType = CALL_REJECT;
    callMsg.srcNumber = g_voiphConfig.number;
    callMsg.destNumber = l_sessionStatus.destNumber;
    callMsg.srcIP = g_voiphConfig.clientIP;
    callMsg.srcPort = g_voiphConfig.dspPort;
    callMsg.srcCtrport = g_voiphConfig.dspCtrlPort;

    nread = sock_tcpWrite(sockfd, &callMsg, sizeof(CALL_MSG));
    CX_LOGL(CX_DBG, "reject a call from %llu to %llu with result %d ", callMsg.destNumber, callMsg.srcNumber, nread);

    kill(l_sendVoicePid, SIGQUIT);
    l_sessionStatus.fxsState = VOIP_HOOK_ON;
    return nread;
}
int acceptCall()
{
    int sockfd = l_sessionStatus.connectfd;
    int nread;
    CALL_MSG callMsg;

    callMsg.callMsgType = CALL_ACCEPT;
    callMsg.srcNumber = g_voiphConfig.number;
    callMsg.destNumber = l_sessionStatus.destNumber;
    callMsg.srcIP = g_voiphConfig.clientIP;
    callMsg.srcPort = g_voiphConfig.dspPort;
    callMsg.srcCtrport = g_voiphConfig.dspCtrlPort;

    nread = sock_tcpWrite(sockfd, &callMsg, sizeof(CALL_MSG));
    CX_LOGL(CX_DBG, "tell another client that we accept a call with result %d ", nread);

    l_sessionStatus.fxsState = VOIP_TALKING;

    sleep(1);
    sendVoice(l_sessionStatus.destIP, l_sessionStatus.destPort, 0);
    return nread;
}
int downCall()
{
    int sockfd = l_sessionStatus.connectfd;
    int nread;
    CALL_MSG callMsg;

    callMsg.callMsgType = CALL_DOWN;
    callMsg.srcNumber = g_voiphConfig.number;
    callMsg.destNumber = l_sessionStatus.destNumber;
    callMsg.srcIP = g_voiphConfig.clientIP;
    callMsg.srcPort = g_voiphConfig.dspPort;
    callMsg.srcCtrport = g_voiphConfig.dspCtrlPort;

    nread = sock_tcpWrite(sockfd, &callMsg, sizeof(CALL_MSG));
    CX_LOGL(CX_DBG, "down a call(number=%llu) with result %d ", callMsg.destNumber, nread);

    kill(l_sendVoicePid, SIGQUIT);
    l_sessionStatus.fxsState = VOIP_HOOK_ON;
    return nread;
}

LOCAL void *handleClient(void *pArg)
{
    CALL_MSG callMsg;
    int nread;
    int connectfd = *(int *)pArg;
    //char buf[BUFF_MAX];
    int finishCall = 0;

    CX_LOGL(CX_INFO, "Enter handleClient thread. arg=%d", connectfd);

    while (!finishCall)
    {
        nread = sock_tcpRead(connectfd, &callMsg, sizeof(CALL_MSG));

        if (nread > 0)
        {
            CX_LOGL(CX_DBG, "Receive call msg. Type=%d, srcIP=%x, srcPort=%d, srcCtrport=%d, srcNumber=%llu, destNumber=%llu",
                (int)callMsg.callMsgType, callMsg.srcIP, callMsg.srcPort, callMsg.srcCtrport, callMsg.srcNumber, callMsg.destNumber);

            switch (callMsg.callMsgType)
            {
            case CALL_NUMBER:
                if (l_sessionStatus.fxsState == VOIP_HOOK_ON)
                {
                   // snprintf(buf, BUFF_MAX, "%d %llu", endpt, callMsg.srcNumber);
                    //if (actionBeCalled(buf) == VOIPC_SUCCESS)

                    {
                        CX_LOGL(CX_DBG, "be called. chang state");
                        l_sessionStatus.fxsState = VOIP_BECALLED;
                        l_sessionStatus.destIP = callMsg.srcIP;
                        l_sessionStatus.destPort = callMsg.srcPort;
                        l_sessionStatus.destCtrport = callMsg.srcCtrport;
                        l_sessionStatus.destNumber = callMsg.srcNumber;
                        l_sessionStatus.connectfd = connectfd;

                    }
                    if (callMsg.destNumber != g_voiphConfig.number)
                    {
                        rejectCall();
                        finishCall = 1;
                    }
                }
                else
                {
                    //CX_LOGL(CX_WARN, "is busy.");

                    //rejectCall(connectfd);
                }
                break;

            case CALL_ACCEPT:   /* The other client accept the call. */
                /*snprintf(buf, BUFF_MAX, "%d %d %d %d %d %d", endpt, VOIP_SERVER_IP_INT, l_endptInfo[endpt].port,
                         callMsg.srcIP, callMsg.srcPort, callMsg.srcCtrport);*/
                /*if (actionTalking(buf) != VOIPC_SUCCESS)
                {
                    CX_LOGL(CX_ERR, "Accept call failed!");
                    rejectCall(connectfd, &callMsg);
                }
                else*/
                {
                    sendVoice(callMsg.srcIP, callMsg.srcPort, 0);
                    l_sessionStatus.fxsState = VOIP_TALKING;
                }
                CX_LOGL(CX_DBG, "The other client accept the call. Now fxsStateis %d(%d)", l_sessionStatus.fxsState, l_sessionStatus.fxsState);
                break;

            case CALL_DOWN:
                //snprintf(buf, BUFF_MAX, "%d", endpt);


                //actionMissCall(buf);
                l_sessionStatus.fxsState = VOIP_HOOK_ON;


                finishCall = 1;
                CX_LOGL(CX_DBG, "The other client down the call!");
                break;

            case CALL_REJECT:
                //snprintf(buf, BUFF_MAX, "%d", endpt);
                //actionCallingFailed(buf);
                l_sessionStatus.fxsState = VOIP_HOOK_ON;
                CX_LOGL(CX_DBG, "The call is being rejected!!");

                finishCall = 1;
                break;

            default:
                break;
            }
        }
        else if (nread == SOCKERR_CLOSED)
        {
            CX_LOGL(CX_WARN, "fd %d closed by client.", connectfd);
            break;
        }
        else if (nread == SOCKERR_IO)
        {
            CX_LOGL(CX_ERR, "read error on fd %d\n", connectfd);
            break;
        }
    }
    close(connectfd);
    kill(l_sendVoicePid, SIGQUIT);
    CX_LOGL(CX_DBG, "client(fd:%d) has done.", connectfd);
    close(connectfd);
    return NULL;
}


void *handleClientConn(void *pArg)
{
    int listenfd = -1;
	int connectfd = -1;
	pthread_t clienttid = -1;

    CX_LOGL(CX_DBG, "enter handleClientConn thread.");

    if ((listenfd = sock_tcpServer(g_voiphConfig.listenPort, BACKLOG)) == SOCKERR_IO)
    {
        //TODO: recreate tcpserver
        CX_LOGL(CX_ERR, "Listen failed at port %d.", g_voiphConfig.listenPort);
        return NULL;
    }

    initSessionStatus();

    while (1)
    {
        /* A new client connect. */
        while ((connectfd = accept(listenfd, NULL, NULL)) < 0)
        {
            CX_LOGL(CX_ERR, "Accept failed at fd: %d. Redo...", connectfd);
            close(listenfd);
            listenfd = listen(g_voiphConfig.listenPort, BACKLOG);
            continue;
        }

        pthread_create(&clienttid, NULL, handleClient, &connectfd);

        CX_LOGL(CX_DBG, "Accept client. fd : %d. handle pthread: %lu", connectfd, (unsigned long)clienttid);

    }

    return NULL;
}

