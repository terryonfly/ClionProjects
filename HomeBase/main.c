#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "TCPServer.h"
#include "cJSON.h"
#include "Sensor.h"

int running = 1;

struct cdc_dev *cdc_d;

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
    cJSON_AddNumberToObject(rootFunc, "temperature", cdc_d->sensor_temperature);
    cJSON_AddNumberToObject(rootFunc, "humidity", cdc_d->sensor_humidity);
    cJSON_AddNumberToObject(rootFunc, "pressure", cdc_d->sensor_pressure);
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
    while ((cdc_d = cdc_dev_open()) == NULL) {
        printf("Retry connect to cdc\n");
        sleep(1);
    }
    while (running) {
        cdc_dev_read_sensor(cdc_d);
        if (cdc_d->updated) {
            save_to_database();
            update_to_tcp();
            cdc_d->updated = 0;
        }
        usleep(1000);
    }
    cdc_dev_close(cdc_d);
    tcpserver_release();
    printf("==== robot end ====\n");
    return 0;
}