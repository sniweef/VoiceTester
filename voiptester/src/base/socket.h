/*  Copyright(c) 2009-2014 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		socket.h
 * brief
 * details
 *
 * author	Huang Zhigeng
 * version
 * date		11Oct14
 *
 * history 	\arg
 */
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdint.h>

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
/*
 * brief	socket error IO
 */
#define SOCKERR_IO          -1
/*
 * brief	socket closed
 */
#define SOCKERR_CLOSED      -2
/*
 * brief	socket operation OK
 */
#define SOCKERR_OK          0

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/
int sock_tcpServer(uint16_t port, int backlog);
/*
 * fn		int voip_serv_listen (const char* name, const int backlog)
 * brief	create unix stream socket and listen on it
 *
 * param[in]	name : bind to the socket
 * param[in]    backlog :  the maximum length the queue of pending connections may grow to
 *
 * return	listen fd
 * retval	new fd if all OK, < 0 on error
 */
int voip_serv_listen (const char* name, const int backlog);
/*
 * fn		int voip_serv_accept (int listenfd)
 * brief	Wait for a client connection to arrive, and accept it
 *
 * param[in]	listenfd : which socket the server is listening to
 *
 * return	accpted fd
 * retval	returns new fd if all OK, < 0 on error
 */
int voip_serv_accept (int listenfd);
/*
 * fn		int voip_cli_connect (const char *name)
 * brief	connect to the server with a Unix domain stream socket
 *
 * param[in]	pIPStr : IP string, if not NULL, the ip parameter is no use.
 * param[in]    ip     : ip integer
 *
 * return	connected fd
 * retval	returns new fd if all OK, < 0 on error
 */
int sock_tcpConnect (const char *pIPStr, uint32_t ip,  uint16_t port);
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
int sock_tcpRead (int fd, void* buff, int count);
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
int sock_tcpWrite (int fd, const void* buff, int count);

#endif // __SOCKET_H__
