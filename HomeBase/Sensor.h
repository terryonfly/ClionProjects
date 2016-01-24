//
// Created by Terry on 16/1/20.
//

#ifndef HOMEBASE_SENSOR_H
#define HOMEBASE_SENSOR_H

#define MAX_REV_CDC_LEN 1024

struct cdc_dev {
    int fd;
    unsigned char rev_content[MAX_REV_CDC_LEN];
    int rev_content_index;

    float sensor_temperature;
    float sensor_humidity;
    float sensor_pressure;

    int updated;
};

struct cdc_dev *cdc_dev_open();

void cdc_dev_close(struct cdc_dev *cdc_d);

void cdc_dev_read_sensor(struct cdc_dev *cdc_d);

void sensor_content_decode(struct cdc_dev *cdc_d, unsigned char *buf);

void sensor_data_decode(struct cdc_dev *cdc_d, unsigned char *buf, size_t len);

#endif //HOMEBASE_SENSOR_H
