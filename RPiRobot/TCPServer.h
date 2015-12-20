//
// Created by terry on 15-12-20.
//

#ifndef RPIROBOT_TCPSERVER_H
#define RPIROBOT_TCPSERVER_H

int tcpserver_init(void);

void tcpserver_release(void);

void tcpserver_run(void);

void tcpserver_data_decode(unsigned char *buf, size_t len);

int tcpserver_send(unsigned char *buf, size_t len);

#endif //RPIROBOT_TCPSERVER_H
