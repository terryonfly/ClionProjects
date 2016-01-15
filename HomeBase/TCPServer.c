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
#define MAXRECVLEN 1024

int connectfd = -1;

pthread_t thread_id;
int thread_running;

#define MAX_REV_CONTENT_LEN 1024 * 1024
unsigned char rev_content[MAX_REV_CONTENT_LEN];
int rev_content_index;

extern int auto_update;

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
    unsigned char buf[MAXRECVLEN];
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
    while(thread_running){
        if ((connectfd = accept(listenfd, (struct sockaddr *)&client, &addrlen)) == -1) {
            perror("accept() error. \n");
            return;
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %ld.%d\n", inet_ntoa(client.sin_addr), htons(client.sin_port), tv.tv_sec, tv.tv_usec);

        int read_len;
        while(thread_running)
        {
        	read_len = recv(connectfd, buf, MAXRECVLEN, 0);
            if (read_len > 0) {
            	tcpserver_data_decode(buf, read_len);
            } else {
                close(connectfd);
                connectfd = -1;
                break;
            }
        }
		printf("connection did closed.\n");
    }
    close(listenfd);
}

void tcpserver_data_decode(unsigned char *buf, size_t len) {
    int i;
    for (i = 0; i < len; i ++) {
		if (buf[i] == '\n') {
			rev_content[rev_content_index] = "\0";
			rev_content_index ++;
			if (rev_content_index > MAX_REV_CONTENT_LEN) rev_content_index = 0;
			tcpserver_content_decode(rev_content);
			rev_content_index = 0;
		} else {
			rev_content[rev_content_index] = buf[i];
			rev_content_index ++;
			if (rev_content_index > MAX_REV_CONTENT_LEN) rev_content_index = 0;
		}
    }
}

void tcpserver_content_decode(unsigned char *buf)
{
    cJSON *jsonRoot = cJSON_Parse(buf);
	cJSON *rootFunc = cJSON_GetObjectItem(jsonRoot, "ctrl");
	cJSON *ctrlVal;
	if ((ctrlVal = cJSON_GetObjectItem(rootFunc, "auto_update")) != NULL) {
		auto_update = ctrlVal->valueint;
	}
}

int tcpserver_send(unsigned char *buf)
{
	if (connectfd == -1) return -1;
	if (send(connectfd, buf, strlen(buf), 0) == -1) {
		perror("send failure\n");
		return -1;
	}
	return 0;
}

