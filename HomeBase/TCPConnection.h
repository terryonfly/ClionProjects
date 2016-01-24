//
// Created by Terry on 16/1/19.
//

#ifndef HOMEBASE_TCPCONNECTION_H
#define HOMEBASE_TCPCONNECTION_H

#define MAX_REV_CONTENT_LEN 1024 * 1024

struct tcp_connection {
    pthread_t thread_id;
    int thread_running;
    int connectfd;
    unsigned char rev_content[MAX_REV_CONTENT_LEN];
    int rev_content_index;
    int auto_update;
    int bad_rev;
    int sync_finished;
};

struct tcp_connection *tcpconnection_init(int fd);

void tcpconnection_release(struct tcp_connection *connection_dev);

void *tcpconnection_run(void *arg);

void tcpconnection_data_decode(struct tcp_connection *connection_dev, unsigned char *buf, size_t len);

void tcpconnection_content_decode(struct tcp_connection *connection_dev, unsigned char *buf);

void tcpconnection_sync_history(struct tcp_connection *connection_dev);

int tcpconnection_send(struct tcp_connection *connection_dev, unsigned char *buf);

#endif //HOMEBASE_TCPCONNECTION_H
