/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		socket.c
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		10Oct14
 *
 * history 	\arg
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#include <base/socket.h>
#include <base/debug.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

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

/**************************************************************************************************/
/*                                           LOCAL_FUNCTIONS                                      */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           PUBLIC_FUNCTIONS                                     */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           GLOBAL_FUNCTIONS                                     */
/**************************************************************************************************/
/*
 * fn		int sock_connect (const char *pIPStr, uint16_t port)
 * brief	connect to the server with a TCP stream socket
 *
 * param[in]	name : indicates the server name
 *
 * return	connected fd
 * retval	returns new fd if all OK, < 0 on error
 */
int sock_tcpConnect (const char *pIPStr, uint32_t ip, uint16_t port)
{
    int fd/* , len */;
    struct sockaddr_in in_addr;
	int ret = 0;

    /* create a Unix domain stream socket */
	errno = 0;
    if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		CX_LOGL(CX_ERR, "socket error, errno:%d", errno);
        return(-1);
    }

	/* fill socket address structure w/server's addr */
    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(port);
	in_addr.sin_addr.s_addr = htonl(ip);

	// cxTODO ip
    if(pIPStr && 1 != (ret = inet_pton(AF_INET, pIPStr, &in_addr.sin_addr)))
    {
		CX_LOGL(CX_ERR, "inet_pton error, ret:%d. ip:%s.", ret, pIPStr);
		goto error;
	}
    //len = sizeof(in_addr.sin_family) + sizeof(in_addr.sin_addr) + sizeof(in_addr.sin_port);

	errno = 0;
    if ((ret = connect (fd, (struct sockaddr *) &in_addr, sizeof(in_addr))) < 0)
    {
		CX_LOGL(CX_ERR, "connect error, ret:%d, errno:%d", ret, errno);
		perror("connect");
        goto error;
    }
	fcntl(fd, F_SETFD, FD_CLOEXEC);
    return (fd);

error:
	CX_LOGL(CX_ERR, "sock_tcpConnect error");
    close (fd);
    return(-1);
}

int sock_tcpServer(uint16_t port, int backlog)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0)
    {
        return SOCKERR_IO;
    }
    struct sockaddr_in serverAddr;

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    CX_LOGL(CX_INFO, "bind fd %d in port %d", listenfd, ntohs(serverAddr.sin_port));

    if (bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        CX_LOGL(CX_ERR, "bind error.");
        return SOCKERR_IO;
    }

    CX_LOGL(CX_INFO, "listen fd: %d", listenfd);
    if (listen(listenfd, backlog) < 0)
    {
        CX_LOGL(CX_ERR, "listen error.");
        return SOCKERR_IO;
    }

    return listenfd;
}

/*
 * fn		int voip_sock_read (int fd, void* buff, int count)
 * brief	read count bytes from the fd and save them into buff
 *
 * param[in]	fd : read from
 * param[in]    buff : container
 * param[in]    count : reading size
 *
 * return	socket reading result
 * retval	count:OK,SOCKERR_IO :err,SOCKERR_CLOSED:closed
 */
int sock_tcpRead (int fd, void* buff, int count)
{
    void* pts = buff;
    int status = 0, n;

    if (count <= 0) return SOCKERR_OK;

    while (status != count)
	{
        n = read (fd, pts + status, count - status);

        if (n < 0)
		{
            if (EINTR == errno)
			{
                continue;
            }
            else
            {
                return SOCKERR_IO;
            }
        }

        if (0 == n)
        {
            return SOCKERR_CLOSED;
        }

        status += n;
    }

    return status;
}

/*
 * fn		int voip_sock_write (int fd, void* buff, int count)
 * brief	write count bytes in buff to the fd
 *
 * param[in]	fd : write to
 * param[in]    buff : container
 * param[in]    count : writing size
 *
 * return	socket writing result
 * retval	count:OK,SOCKERR_IO :err,SOCKERR_CLOSED:closed
 */
int sock_tcpWrite (int fd, const void* buff, int count)
{
    const void* pts = buff;
    int status = 0, n;

    if (count < 0) return SOCKERR_OK;

    while (status != count)
	{
        n = write (fd, pts + status, count - status);
        if (n < 0)
		{
            if (EPIPE == errno)
            {
                return SOCKERR_CLOSED;
            }
            else if (EINTR == errno)
			{
                continue;
            }
            else
            {
                return SOCKERR_IO;
            }
        }
        status += n;
    }

    return status;
}

