//
// Created by Terry on 15/12/12.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <netdb.h> /* netdb is necessary for struct hostent */
#include <pthread.h>
#include <errno.h>

#include "tcpclient.h"

#define PORT 7777 /* server port */
#define MAXDATASIZE 1024
int sock_fd = -1;

pthread_t thread_id;
int thread_running;

#define CMD_DATA_CONTENT 0x80
#define CMD_DATA_HEADER 0x81
#define CMD_DATA_FOOTER 0x82
#define MAX_REV_CONTENT_LEN 1024
unsigned char rev_content[MAX_REV_CONTENT_LEN];
int rev_content_index;
int rev_is_cmd = 1;
unsigned char rev_current_cmd;

#define MAX_SEND_DATA_LEN 1024
unsigned char send_data[MAX_SEND_DATA_LEN];

#define MAX_EAGAIN_TIMES 3 * 1000000

float rotate_a = 1.0;
float rotate_x = 0.0;
float rotate_y = 0.0;
float rotate_z = 0.0;

float accel_x = 0.0;
float accel_y = 1.0;
float accel_z = 0.0;

float magnet_x = 0.0;
float magnet_y = 0.0;
float magnet_z = 1.0;

float left_angle = 0.0;
float right_angle = 0.0;
float left_power = 0.0;
float right_power = 0.0;

float status_x = 0.0;
float status_y = 0.0;
float status_z = 0.0;
float status_w = 0.0;

float status_x_array[MAX_PLOT_LEN];
int status_x_current_index = 0;
float status_y_array[MAX_PLOT_LEN];
int status_y_current_index = 0;
float status_z_array[MAX_PLOT_LEN];
int status_z_current_index = 0;
float status_w_array[MAX_PLOT_LEN];
int status_w_current_index = 0;

int tcpclient_init(void) {
    int ret;
    thread_running = 1;
    ret = pthread_create(&thread_id, NULL, (void *)tcpclient_run, NULL);
    if (ret != 0) {
        perror("Create pthread error!\n");
        return -1;
    }
    memset(status_x_array, 0, sizeof(status_x_array));
    memset(status_y_array, 0, sizeof(status_y_array));
    memset(status_z_array, 0, sizeof(status_z_array));
    memset(status_w_array, 0, sizeof(status_w_array));
    return 0;
}

void tcpclient_release(void) {
    printf("Release pthread\n");
    thread_running = 0;
    pthread_join(thread_id, NULL);
}

void tcpclient_run(void) {
    int eagain_times = 0;
    int retry_connect_times = 0;
    while (thread_running) {
        int num; /* files descriptors */
        unsigned char buf[MAXDATASIZE]; /* buf will store received text */
        struct hostent *he; /* structure that will get information about remote host */
        struct sockaddr_in server;

        if ((he = gethostbyname("192.168.102.64")) == NULL) {
            perror("Gethostbyname() error ");
            return;
        }

        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket() error ");
            return;
        }
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr = *((struct in_addr *)he->h_addr);
        printf("Try connect[%ds] : ", retry_connect_times);
        fflush(stdout);
        sleep(1);
        if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
            retry_connect_times ++;
            perror("");
        } else {
            retry_connect_times = 0;
            printf("Successes\n");
            fcntl(sock_fd, F_SETFL, O_NONBLOCK);
            while (thread_running) {
                if ((num = recv(sock_fd, buf, MAXDATASIZE, 0)) <= 0) {
                    if (num == 0) {
                        perror("Connection has closed ");
                        break;
                    }
                    if (num == -1) {
                        if (errno == ENOTCONN) {
                            perror("Connection goes error ");
                            break;
                        }
                        if (errno == EAGAIN) {
                            eagain_times ++;
                            if (eagain_times > MAX_EAGAIN_TIMES) {
                                eagain_times = 0;
                                perror("Read data timeout ");
                                break;
                            }
                            usleep(1);
                        }
                    }
                }
                if (num > 0) {
                    eagain_times = 0;
                    tcpclient_data_decode(buf, num);
                }
            }
            fcntl( sock_fd, F_SETFL, 0 );
        }
        close(sock_fd);
        sock_fd = -1;
    }
}

void tcpclient_data_decode(unsigned char *buf, size_t len) {
    int i;
    for (i = 0; i < len; i ++) {
        if (rev_is_cmd) {
            if (buf[i] == CMD_DATA_CONTENT || buf[i] == CMD_DATA_HEADER || buf[i] == CMD_DATA_FOOTER) {
                rev_current_cmd = buf[i];
                rev_is_cmd = 0;
            }
        } else {
            switch (rev_current_cmd) {
                case CMD_DATA_CONTENT:
                    rev_content[rev_content_index] = buf[i];
                    rev_content_index ++;
                    if (rev_content_index > MAX_REV_CONTENT_LEN) rev_content_index = 0;
                    break;
                case CMD_DATA_HEADER:
                    // buf[i] is header data
                    rev_content_index = 0;
                    break;
                case CMD_DATA_FOOTER:
                    // buf[i] is footer data
                    tcpclient_content_decode(rev_content, rev_content_index);
                    content_data_updated();
                    rev_content_index = 0;
                    break;

                default:
                    break;
            }
            rev_is_cmd = 1;
        }
    }
}

void tcpclient_content_decode(unsigned char *buf, size_t len)
{
    unsigned char i;
    unsigned char* px = buf;
    void *pf;

    pf = &rotate_a;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &rotate_x;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &rotate_y;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &rotate_z;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &accel_x;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &accel_y;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &accel_z;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &magnet_x;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &magnet_y;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &magnet_z;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &left_angle;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &right_angle;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &left_power;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &right_power;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &status_x;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &status_y;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &status_z;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }

    pf = &status_w;
    for(i = 0; i < 4; i ++) {
        *((unsigned char*)pf+i) = *(px++);
    }
}

void content_data_updated() {
    status_x_array[status_x_current_index] = status_x;
    status_x_current_index ++;
    if (status_x_current_index >= MAX_PLOT_LEN) status_x_current_index = 0;

    status_y_array[status_y_current_index] = status_y;
    status_y_current_index ++;
    if (status_y_current_index >= MAX_PLOT_LEN) status_y_current_index = 0;

    status_z_array[status_z_current_index] = status_z;
    status_z_current_index ++;
    if (status_z_current_index >= MAX_PLOT_LEN) status_z_current_index = 0;

    status_w_array[status_w_current_index] = status_w;
    status_w_current_index ++;
    if (status_w_current_index >= MAX_PLOT_LEN) status_w_current_index = 0;
}

int tcpclient_send(unsigned char *buf, size_t len)
{
    if (sock_fd == -1) return -1;
    if (len > MAX_SEND_DATA_LEN / 2) return -1;
    int set_i = 0;
    int i, send_len;
    // Header
    send_data[set_i] = CMD_DATA_HEADER;
    set_i ++;
    send_data[set_i] = 0x01;
    set_i ++;
    // Content
    for (i = 0; i < len; i ++) {
        send_data[set_i] = CMD_DATA_CONTENT;
        set_i ++;
        send_data[set_i] = buf[i];
        set_i ++;
    }
    // Footer
    send_data[set_i] = CMD_DATA_FOOTER;
    set_i ++;
    send_data[set_i] = 0x82;
    set_i ++;
    if ((send_len = send(sock_fd, send_data, set_i, 0)) == -1) {
        printf("send_len = %d\n", send_len);
        perror("send() failure ");
        return -1;
    }
    return 0;
}