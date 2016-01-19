//
// Created by Terry on 16/1/19.
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
#include <pthread.h>
#include <string.h>

#include "TCPConnection.h"
#include "cJSON.h"

#define MAXRECVLEN 1024

void tcpconnection_content_decode(struct tcp_connection *connection_dev, unsigned char *buf) {
    cJSON *jsonRoot = cJSON_Parse((char *)buf);
    cJSON *rootFunc = cJSON_GetObjectItem(jsonRoot, "ctrl");
    cJSON *ctrlVal;
    if ((ctrlVal = cJSON_GetObjectItem(rootFunc, "auto_update")) != NULL) {
        connection_dev->auto_update = ctrlVal->valueint;
    }
}

void tcpconnection_data_decode(struct tcp_connection *connection_dev, unsigned char *buf, size_t len) {
    int i;
    for (i = 0; i < len; i ++) {
        if (buf[i] == '\r') {
            connection_dev->rev_content[connection_dev->rev_content_index] = '\0';
            connection_dev->rev_content_index ++;
            if (connection_dev->rev_content_index > MAX_REV_CONTENT_LEN) connection_dev->rev_content_index = 0;
            tcpconnection_content_decode(connection_dev, connection_dev->rev_content);
            connection_dev->rev_content_index = 0;
        } else {
            connection_dev->rev_content[connection_dev->rev_content_index] = buf[i];
            connection_dev->rev_content_index ++;
            if (connection_dev->rev_content_index > MAX_REV_CONTENT_LEN) connection_dev->rev_content_index = 0;
        }
    }
}

void tcpconnection_run() {
//    unsigned char buf[MAXRECVLEN];
//
//    int read_len;
//    while(connection_dev->thread_running)
//    {
//        read_len = recv(connection_dev->connectfd, buf, MAXRECVLEN, 0);
//        if (read_len > 0) {
//            tcpconnection_data_decode(connection_dev, buf, read_len);
//        } else {
//            close(connection_dev->connectfd);
//            connection_dev->connectfd = -1;
//            connection_dev->thread_running = 0;
//            break;
//        }
//    }
    printf("connection did closed.\n");
}

struct tcp_connection *tcpconnection_init(int fd) {
    int ret;
    struct tcp_connection *connection_dev;
    connection_dev->connectfd = fd;
    connection_dev->thread_running = 1;
    ret = pthread_create(&connection_dev->thread_id, NULL, (void *)tcpconnection_run, NULL);
    if (ret != 0) {
        perror("Create pthread error!\n");
        return (struct tcp_connection *)-1;
    }
    return connection_dev;
}

void tcpconnection_release(struct tcp_connection *connection_dev) {
    printf("Release pthread\n");
    connection_dev->thread_running = 0;
    pthread_join(connection_dev->thread_id, NULL);
}

int tcpconnection_send(struct tcp_connection *connection_dev, unsigned char *buf) {
    if (connection_dev->connectfd == -1) return -1;
    if (send(connection_dev->connectfd, buf, strlen((const char *)buf), 0) == -1) {
        perror("send failure\n");
        return -1;
    }
    return 0;
}