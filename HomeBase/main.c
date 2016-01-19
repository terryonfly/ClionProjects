#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "TCPServer.h"
#include "cJSON.h"

int running = 1;

float sensor_temperature = 0.0;
float sensor_humidity = 0.0;
float sensor_pressure = 0.0;

void save_to_database() {

}

unsigned char* join_chars(unsigned char *s1, unsigned char *s2)
{
    unsigned char *result = malloc(strlen((const char *)s1) + strlen((const char *)s2) + 1);
    if (result == NULL) return NULL;

    strcpy((char *)result, (char *)s1);
    strcat((char *)result, (char *)s2);

    return result;
}

void update_to_tcp() {
    cJSON *jsonRoot = cJSON_CreateObject();
    cJSON *rootFunc;
    rootFunc = cJSON_CreateObject();
    cJSON_AddNumberToObject(rootFunc, "temperature", sensor_temperature);
    cJSON_AddNumberToObject(rootFunc, "humidity", sensor_humidity);
    cJSON_AddNumberToObject(rootFunc, "pressure", sensor_pressure);
    cJSON_AddItemToObject(jsonRoot, "auto_data", rootFunc);

    unsigned char *jsonBuffer = (unsigned char *)cJSON_Print(jsonRoot);
    cJSON_Delete(jsonRoot);
    unsigned char *jsonBufferFormat = join_chars(jsonBuffer, (unsigned char *)"\r");
    tcpserver_send(jsonBufferFormat);
}

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
        sensor_temperature += 0.1f;
        sensor_humidity -= 0.1f;
        sensor_pressure += 0.2f;
        save_to_database();
//        update_to_tcp();
        sleep(1);
    }
    tcpserver_release();
    printf("==== robot end ====\n");
    return 0;
}