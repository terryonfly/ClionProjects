#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "TCPServer.h"

int running = 1;

int auto_update = 1;

void cs(int n) {
    printf("now dowm = %d\n", n);
    running = 0;
}

int main() {
    printf("=== robot start ===\n");
    signal(SIGINT, cs);// ctrl+c
    signal(SIGTERM, cs);// kill
    tcpserver_init();
    while (running) {
        if (auto_update) {
            unsigned char *msg = "abcdefg";
            tcpserver_send(msg);
        }
        sleep(1);
    }
    tcpserver_release();
    printf("==== robot end ====\n");
    return 0;
}