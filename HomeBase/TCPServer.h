/*
 * TCPServer.h
 *
 *  Created on: 2015年12月10日
 *      Author: terry
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "TCPConnection.h"

int tcpserver_init(void);

void tcpserver_release(void);

void tcpserver_run(void);

int tcpserver_send(unsigned char *buf);

void fix_connection_list(void);

#endif /* TCPSERVER_H_ */
