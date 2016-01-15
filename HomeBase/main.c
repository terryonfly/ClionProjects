#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "TCPServer.h"

int running = 1;

void cs(int n) {
    printf("now dowm!\n");
    running = 0;
}

int main() {
    printf("=== robot start ===\n");
    signal(SIGINT, cs);  //ctrl+c
    signal(SIGTERM, cs);  //kill
    tcpserver_init();
    while (running) {
        unsigned char *msg = "abcdefg";
        tcpserver_send(msg, strlen(msg));
        sleep(1);
    }
    tcpserver_release();
    printf("==== robot end ====\n");
    return 0;
}