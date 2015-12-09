#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> /* netdb is necessary for struct hostent */

int running = 1;

#define PORT 7777 /* server port */
#define MAXDATASIZE 1024
int sock_fd = -1;

#define CMD_DATA_CONTENT 0x80
#define CMD_DATA_HEADER 0x81
#define CMD_DATA_FOOTER 0x82
#define MAX_REV_CONTENT_LEN 1024
unsigned char rev_content[MAX_REV_CONTENT_LEN];
int rev_content_index;
int rev_is_cmd = 1;
unsigned char rev_current_cmd;

void tcpclient_data_decode(unsigned char *buf, size_t len) {
    int i, k;
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
                    for (k = 0; k < rev_content_index; k ++)
                        printf("%02x ", rev_content[k]);
                    printf("\n");

                    rev_content_index = 0;
                    break;

                default:
                    break;
            }
            rev_is_cmd = 1;
        }
    }
}

#define MAX_SEND_DATA_LEN 1024
unsigned char send_data[MAX_SEND_DATA_LEN];

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
        perror("send failure\n");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int num; /* files descriptors */
    unsigned char buf[MAXDATASIZE]; /* buf will store received text */
    struct hostent *he; /* structure that will get information about remote host */
    struct sockaddr_in server;

    if ((he = gethostbyname("192.168.101.84")) == NULL) {
        perror("gethostbyname() error\n");
        return 1;
    }

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error\n");
        return 1;
    }
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *)he->h_addr);
    while (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("try connect() error\n");
        sleep(3);
    }
    printf("connect() successes\n");

    while (running) {
        if ((num = recv(sock_fd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv() error\n");
        }
        tcpclient_data_decode(buf, num);

        int i;
        unsigned char msg[12];
        for (i = 0; i < 12; i ++) {
            msg[i] = 12 - i;
        }
        tcpclient_send(msg, 12);
    }
    close(sock_fd);
    sock_fd = -1;
    return 0;
}