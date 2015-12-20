#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include "TCPServer.h"

int running = 1;

float a_x = 0.0;
float a_y = 0.0;
float a_z = 0.0;

float y_a = 0.0;
float y_l = 10.0;

void sync_status() {
    unsigned char msg[12];
    int c_i = 0;
    unsigned char *pdata;
    int i;

    y_a += 3;
    if (y_a > 360.0) y_a = 0.0;
    a_x = y_l * cos(y_a * M_PI / 180.0);
    a_z = y_l * sin(y_a * M_PI / 180.0);

    float angle_x = a_x;
    pdata = ((unsigned char *)&angle_x);
    for (i = 0; i < 4; i ++) {
        msg[c_i ++] = *pdata ++;
    }

    a_y += 0.1;
    if (a_y > 360.0) a_y = 0.0;

    float angle_y = a_y;
    pdata = ((unsigned char *)&angle_y);
    for (i = 0; i < 4; i ++) {
        msg[c_i ++] = *pdata ++;
    }

    float angle_z = a_z;
    pdata = ((unsigned char *)&angle_z);
    for (i = 0; i < 4; i ++) {
        msg[c_i ++] = *pdata ++;
    }
    tcpserver_send(msg, 12);
}

void cs(int n) {
    printf("now dowm!\n");
    running = 0;
}

int main() {
    printf("=== robot start ===\n");
    signal(SIGINT, cs);  //ctrl+c
    signal(SIGTERM, cs);  //kill
    tcpserver_init();
    float pwm_persent = 0.0f;
    int bak = 0;
    while (running) {
        sync_status();
        usleep(10*1000);
    }
    tcpserver_release();
    printf("==== robot end ====\n");
    return 0;
}