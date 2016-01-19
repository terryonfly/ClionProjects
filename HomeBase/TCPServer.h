/*
 * TCPServer.h
 *
 *  Created on: 2015年12月10日
 *      Author: terry
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "TCPConnection.h"

struct tcp_connection *connection_dev;

int tcpserver_init(void);

void tcpserver_release(void);

void tcpserver_run(void);

int tcpserver_send(unsigned char *buf);

#endif /* TCPSERVER_H_ */
