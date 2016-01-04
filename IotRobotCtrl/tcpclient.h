//
// Created by Terry on 15/12/12.
//

#ifndef IOTROBOTCTRL_TCPCLIENT_H
#define IOTROBOTCTRL_TCPCLIENT_H

extern float rotate_a;
extern float rotate_x;
extern float rotate_y;
extern float rotate_z;

extern float accel_x;
extern float accel_y;
extern float accel_z;

extern float magnet_x;
extern float magnet_y;
extern float magnet_z;

extern float left_angle;
extern float right_angle;
extern float left_power;
extern float right_power;

int tcpclient_init(void);

void tcpclient_release(void);

void tcpclient_run(void);

void tcpclient_data_decode(unsigned char *buf, size_t len);

void tcpclient_content_decode(unsigned char *buf, size_t len);

int tcpclient_send(unsigned char *buf, size_t len);

#endif //IOTROBOTCTRL_TCPCLIENT_H
