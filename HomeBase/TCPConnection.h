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
};

struct tcp_connection *tcpconnection_init(int fd);

void tcpconnection_release(struct tcp_connection *connection_dev);

int tcpconnection_send(struct tcp_connection *connection_dev, unsigned char *buf);

#endif //HOMEBASE_TCPCONNECTION_H
