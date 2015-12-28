//
// Created by Terry on 15/12/12.
//

#ifndef IOTROBOTCTRL_TCPCLIENT_H
#define IOTROBOTCTRL_TCPCLIENT_H

int tcpclient_init(void);

void tcpclient_release(void);

void tcpclient_run(void);

void tcpclient_data_decode(unsigned char *buf, size_t len);

void tcpclient_content_decode(unsigned char *buf, size_t len);

int tcpclient_send(unsigned char *buf, size_t len);

float get_rotate_a(void);

float get_rotate_x(void);

float get_rotate_y(void);

float get_rotate_z(void);

#endif //IOTROBOTCTRL_TCPCLIENT_H
