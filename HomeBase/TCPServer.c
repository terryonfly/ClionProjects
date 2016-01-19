/*
 * TCPServer.c
 *
 *  Created on: 2015年12月10日
 *      Author: terry
 */
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

#include "TCPServer.h"
#include "cJSON.h"

#define PORT 8888

#define BACKLOG 1

pthread_t thread_id;
int thread_running;

struct tcp_connection *connection_dev[1024];

int connection_dev_count = 0;

int tcpserver_init(void) {
	int ret;
	thread_running = 1;
	ret = pthread_create(&thread_id, NULL, (void *)tcpserver_run, NULL);
	if (ret != 0) {
		perror("Create pthread error!\n");
		return -1;
	}
	return 0;
}

void tcpserver_release(void) {
	printf("Release pthread\n");
	thread_running = 0;
	pthread_join(thread_id, NULL);
}

void tcpserver_run(void) {
    int listenfd;  /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;
    /* Create TCP socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        /* handle exception */
        perror("socket() error. Failed to initiate a socket");
        return;
    }

    /* set socket option */
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        /* handle exception */
        perror("Bind() error.");
        return;
    }

    if (listen(listenfd, BACKLOG) == -1) {
        perror("listen() error.\n");
        return;
    }

    addrlen = sizeof(client);

    int connectfd;
    while(thread_running){
        if ((connectfd = accept(listenfd, (struct sockaddr *)&client, &addrlen)) == -1) {
            perror("accept() error. \n");
            return;
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %d.%d\n", inet_ntoa(client.sin_addr), htons(client.sin_port), (int)tv.tv_sec, (int)tv.tv_usec);

        connection_dev[connection_dev_count] = tcpconnection_init(connectfd);
        connection_dev_count ++;
        fix_connection_list();
    }
    int i;
    for (i = 0; i < connection_dev_count; i ++) {
        if (connection_dev[i]->thread_running)
            tcpconnection_release(connection_dev[i]);
    }
    close(listenfd);
}

int tcpserver_send(unsigned char *buf) {
    int i;
    for (i = 0; i < connection_dev_count; i ++) {
        if (connection_dev[i]->thread_running)
            tcpconnection_send(connection_dev[i], buf);
    }
    return 0;
}

void fix_connection_list(void) {
    int i;
    int j = 0;
    for (i = 0; i < connection_dev_count; i ++) {
        if (connection_dev[i]->thread_running) {
            connection_dev[j] = connection_dev[i];
            j ++;
        }
    }
    connection_dev_count = j;
    printf("available connection count = %d\n", connection_dev_count);
}

