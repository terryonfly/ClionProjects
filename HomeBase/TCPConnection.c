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
#include "DataBase.h"

#define MAXRECVLEN 1024 * 1024

void fix_connection_list(void);

struct tcp_connection *tcpconnection_init(int fd) {
    int ret;
    struct tcp_connection *connection_dev = malloc(sizeof(struct tcp_connection));
    connection_dev->connectfd = fd;
    connection_dev->thread_running = 1;
    ret = pthread_create(&connection_dev->thread_id, NULL, (void *)tcpconnection_run, (void *)connection_dev);
    if (ret != 0) {
        perror("Create pthread error!\n");
        return NULL;
    }
    return connection_dev;
}

void tcpconnection_release(struct tcp_connection *connection_dev) {
    printf("Release pthread\n");
    connection_dev->thread_running = 0;
    pthread_join(connection_dev->thread_id, NULL);
}

void *tcpconnection_run(void *arg) {
    struct tcp_connection *connection_dev = (struct tcp_connection *)arg;
    tcpconnection_sync_history(connection_dev);

    unsigned char *buf = malloc(MAXRECVLEN);
    int read_len;
    while(connection_dev->thread_running) {
        read_len = recv(connection_dev->connectfd, buf, MAXRECVLEN, 0);
        if (read_len >= MAXRECVLEN) {
            connection_dev->bad_rev = 1;
            printf("Rev bad\n");
            continue;
        } else if (read_len > 0) {
            connection_dev->bad_rev = 0;
            tcpconnection_data_decode(connection_dev, buf, read_len);
        } else {
            connection_dev->thread_running = 0;
            break;
        }
    }
    free(buf);
    close(connection_dev->connectfd);
    printf("connection did closed.\n");
    return NULL;
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

void tcpconnection_content_decode(struct tcp_connection *connection_dev, unsigned char *buf) {
    cJSON *jsonRoot = cJSON_Parse((char *)buf);
    cJSON *rootFunc = cJSON_GetObjectItem(jsonRoot, "ctrl");
    cJSON *ctrlVal;
    if ((ctrlVal = cJSON_GetObjectItem(rootFunc, "auto_update")) != NULL) {
        connection_dev->auto_update = ctrlVal->valueint;
    }
}

#define HISTORY_LEN (60 * 60)

void tcpconnection_sync_history(struct tcp_connection *connection_dev) {
    float *history_temp = malloc(sizeof(float) * HISTORY_LEN);
    float *history_humi = malloc(sizeof(float) * HISTORY_LEN);
    float *history_pres = malloc(sizeof(float) * HISTORY_LEN);
    int ret, i;
    int page = 0;
    int page_size = 1000;
    while ((ret = database_get_history(history_temp, history_humi, history_pres, HISTORY_LEN, "2016-01-21 15:00", "2016-01-21 16:00", page, page_size)) != 0) {
        if (ret > 0) {
            page ++;
        } else {
            for (i = 0; i < ret; i ++) {
                cJSON *jsonRoot = cJSON_CreateObject();
                cJSON *rootFunc;
                rootFunc = cJSON_CreateObject();
                cJSON_AddNumberToObject(rootFunc, "temperature", history_temp[i]);
                cJSON_AddNumberToObject(rootFunc, "humidity", history_humi[i]);
                cJSON_AddNumberToObject(rootFunc, "pressure", history_pres[i]);
                cJSON_AddItemToObject(jsonRoot, "auto_data", rootFunc);

                unsigned char *jsonBuffer = (unsigned char *)cJSON_Print(jsonRoot);
                cJSON_Delete(jsonRoot);
                unsigned char *jsonBufferFormat = join_chars(jsonBuffer, (unsigned char *)"\r");
                tcpconnection_send(connection_dev, jsonBufferFormat);
            }
        }
    }
    connection_dev->sync_finished = 1;
}

int tcpconnection_send(struct tcp_connection *connection_dev, unsigned char *buf) {
    if (connection_dev->sync_finished != 1) {
        // add to tmp
        return 0;
    }
    if (connection_dev->connectfd == -1) return -1;
    if (send(connection_dev->connectfd, buf, strlen((const char *)buf), 0) == -1) {
        perror("send failure\n");
        return -1;
    }
    return 0;
}