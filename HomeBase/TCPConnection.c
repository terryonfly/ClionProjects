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
    if (strlen(buf) == 0) return;
    cJSON *jsonRoot = cJSON_Parse((char *)buf);
    if (jsonRoot == NULL) return;
    cJSON *rootFunc = cJSON_GetObjectItem(jsonRoot, "ctrl");
    if (rootFunc == NULL) return;
    cJSON *ctrlVal;
    if ((ctrlVal = cJSON_GetObjectItem(rootFunc, "auto_update")) != NULL) {
        connection_dev->auto_update = ctrlVal->valueint;
    }
}

#define HISTORY_LEN (24 * 60 * 60)

void tcpconnection_sync_history(struct tcp_connection *connection_dev) {
    float *history_temp = malloc(sizeof(float) * HISTORY_LEN);
    float *history_humi = malloc(sizeof(float) * HISTORY_LEN);
    float *history_pres = malloc(sizeof(float) * HISTORY_LEN);
    int ret, i;
    int page = 0;
    int page_size = 60;

    char *time_min = malloc(64);
    char *time_max = malloc(64);
    time_t tCurTime = time(NULL);
    struct tm *ptmCurTime = localtime(&tCurTime);

    sprintf(time_min, "%04d-%02d-%02d %02d:%02d\n",
           ptmCurTime->tm_year+1900, ptmCurTime->tm_mon+1, ptmCurTime->tm_mday - 1, ptmCurTime->tm_hour, ptmCurTime->tm_min);

    sprintf(time_max, "%04d-%02d-%02d %02d:%02d\n",
            ptmCurTime->tm_year+1900, ptmCurTime->tm_mon+1, ptmCurTime->tm_mday, ptmCurTime->tm_hour, ptmCurTime->tm_min);

    while ((ret = database_get_history(history_temp, history_humi, history_pres, HISTORY_LEN, time_min, time_max, page, page_size)) != 0) {
        if (ret > 0) {
            float history_temp_a = 0;
            float history_humi_a = 0;
            float history_pres_a = 0;
            for (i = 0; i < ret; i ++) {
                history_temp_a += history_temp[i];
                history_humi_a += history_humi[i];
                history_pres_a += history_pres[i];
            }
            history_temp_a /= ret;
            history_humi_a /= ret;
            history_pres_a /= ret;
            cJSON *jsonRoot = cJSON_CreateObject();
            cJSON *rootFunc;
            rootFunc = cJSON_CreateObject();
            cJSON_AddNumberToObject(rootFunc, "temperature", history_temp_a);
            cJSON_AddNumberToObject(rootFunc, "humidity", history_humi_a);
            cJSON_AddNumberToObject(rootFunc, "pressure", history_pres_a);
            cJSON_AddItemToObject(jsonRoot, "auto_data", rootFunc);

            unsigned char *jsonBuffer = (unsigned char *)cJSON_Print(jsonRoot);
            cJSON_Delete(jsonRoot);
            unsigned char *jsonBufferFormat = join_chars(jsonBuffer, (unsigned char *)"\r");

            if (connection_dev->connectfd == -1) continue;
            if (send(connection_dev->connectfd, jsonBufferFormat, strlen((const char *)jsonBufferFormat), 0) == -1) {
                perror("send failure\n");
            }
            page ++;
//            if (page >= 24 * 60) break;
        }
        if (!connection_dev->thread_running) break;
    }
    free(time_min);
    free(time_max);
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