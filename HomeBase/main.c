#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "TCPServer.h"
#include "cJSON.h"
#include "Sensor.h"
#include "DataBase.h"

int running = 1;

struct cdc_dev *cdc_d;

void save_to_database() {
    printf("temp:%14.10fdegC hum:%14.10f%% pres:%14.10fkPa\n",
           cdc_d->sensor_temperature,
           cdc_d->sensor_humidity,
           cdc_d->sensor_pressure);
    char *sql_format = "insert into `sensors`.`weather` ( `id`, `temperature`, `humidity`, `pressure`, `time`) values ( NULL, '%f', '%f', '%f', Now());";
    char *sql = malloc(512);
    sprintf(sql, sql_format, cdc_d->sensor_temperature, cdc_d->sensor_humidity, cdc_d->sensor_pressure);
    printf("sql = %s\n", sql);
    database_insert(sql);
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

#define TEST

int main() {
    printf("=== robot start ===\n");
    signal(SIGINT, cs);// ctrl+c
    signal(SIGTERM, cs);// kill
    database_init();
    tcpserver_init();
#ifndef TEST
    while ((cdc_d = cdc_dev_open()) == NULL) {
        printf("Retry connect to cdc\n");
        sleep(1);
    }
#else
    cdc_d = malloc(sizeof(struct cdc_dev));
    memset(cdc_d, 0, sizeof(sizeof(struct cdc_dev)));
#endif
    while (running) {
#ifndef TEST
        cdc_dev_read_sensor(cdc_d);
#else
        cdc_d->updated = 1;
        cdc_d->sensor_temperature += 0.1;
        cdc_d->sensor_humidity -= 0.1;
        cdc_d->sensor_pressure += 0.3;
        usleep(999 * 1000);
#endif
        if (cdc_d->updated) {
            save_to_database();
            update_to_tcp();
            cdc_d->updated = 0;
        }
        usleep(1000);
    }
    cdc_dev_close(cdc_d);
    tcpserver_release();
    database_release();
    printf("==== robot end ====\n");
    return 0;
}