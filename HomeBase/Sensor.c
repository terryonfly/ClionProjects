//
// Created by Terry on 16/1/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include <string.h>
#include <signal.h>

#include "Sensor.h"
#include "cJSON.h"

#define SERIAL_DEV "/dev/ttyUSB0"

static int speed_arr[] = { B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1800,
                           B1200, B600, B300, B200, B150, B134, B110, B75, B50};

static int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1800,
                         1200, 600, 300, 200, 150, 134, 110, 75, 50};

// Serial
int open_serial(const char *dev_path) {
    const char *str = dev_path;
    int fd;
    if (str == NULL)
        str = SERIAL_DEV;

    fd = open(str, O_RDWR);
    if (-1 == fd) {
        printf("open %s failed\n", str);
        return -1;
    }
    printf("open %s successed\n", str);

    return fd;
}

int set_speed(int fd, int speed) {
    int i;
    int status;
    int find = 0;
    int ret;
    struct termios opt;

    if (fd <= 0) {
        perror("fd invalid\n");
        return -1;
    }

    ret = tcgetattr (fd, &opt);
    if (ret != 0) {
        perror("tcgetattr failed\n");
        return -1;
    }

    for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
        if (speed == name_arr[i]) {
            find = 1;
            ret = tcflush(fd, TCIOFLUSH);
            if (ret != 0) {
                perror("tcflush failed\n");
                return -1;
            }

            ret = cfsetispeed(&opt, speed_arr[i]);
            if (ret != 0 ) {
                perror("cfsetispeed failed\n");
                return -1;
            }

            ret = cfsetospeed(&opt, speed_arr[i]);
            if (ret != 0) {
                perror("cfsetospeed failed\n");
                return -1;
            }

            status = tcsetattr(fd, TCSANOW, &opt);
            if (status != 0) {
                perror("tcsetattr failed\n");
                return -1;
            }

            ret = tcflush(fd, TCIOFLUSH);
            if (ret != 0) {
                perror("tcflush failed\n");
                return -1;
            }
        }
    }

    if (!find) {
        printf("bad speed arg\n");
        return -1;
    }

    return 0;
}

int set_parity(int fd, int databits, int stopbits, int parity) {
    struct termios opt;
    int ret = 0;
    if (fd <= 0) {
        printf("fd invalid \n");
        return -1;
    }

    if (tcgetattr(fd, &opt) != 0) {
        perror("tcgetattr failed\n");
        return -1;
    }

    opt.c_cflag &= ~CSIZE;
    switch(databits) {
        case 7:
            opt.c_cflag |= CS7;
            break;
        case 8:
            opt.c_cflag |= CS8;
            break;

        default :
            printf("unsupported data size\n");
            return -1;
    }

    switch(parity) {
        case 0://n
            opt.c_cflag &= ~PARENB;
            break;
        case 1://o
            opt.c_cflag |= (PARODD | PARENB);
            break;
        case 2://e
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
            break;
        case 3://s
            opt.c_cflag &= ~PARENB;
            opt.c_cflag &= ~CSTOPB;
            break;

        default:
            printf ("unsupported parity\n");
            return -1;
    }

    switch(stopbits) {
        case 1:
            opt.c_cflag &= ~CSTOPB;
            break;
        case 2:
            opt.c_cflag |= CSTOPB;
            break;

        default:
            printf("unsupported stop bits\n");
            return -1;
    }

    if (parity != 0 && parity != 0)
        opt.c_iflag |= INPCK;
    else
        opt.c_iflag &= ~INPCK;

    opt.c_lflag &= ~ECHO;
    opt.c_lflag &= ~ISIG;
    opt.c_lflag &= ~ICANON;
    opt.c_iflag &= ~ICRNL;
    ret = tcflush(fd, TCIFLUSH);
    if (ret != 0) {
        perror("tcflush failed\n");
        return -1;
    }

    opt.c_cc[VTIME] = 0;
    opt.c_cc[VMIN] = 1;

    if (tcsetattr(fd, TCSANOW, &opt) != 0) {
        perror("setup serial opt failed\n");
        return -1;
    }

    return 0;
}

void close_serial(int fd) {
    if (fd <= 0) {
        printf ("fd %d invalid\n", fd);
        return;
    }
    close (fd);
}

struct cdc_dev *cdc_dev_open() {
    int fd_cdc = open_serial(SERIAL_DEV);
    if (fd_cdc == -1) {
        printf("Cannot open device\n");
        return NULL;
    }
    int ret;
    ret = set_speed(fd_cdc, 57600);
    if (ret == -1) {
        printf("Set speed failed : %d\n", ret);
        return NULL;
    }
    ret = set_parity(fd_cdc, 8, 1, 0);
    if (ret == -1) {
        printf("Set parity failed : %d\n", ret);
        return NULL;
    }

    struct cdc_dev *cdc_d = malloc(sizeof(struct cdc_dev));
    memset(cdc_d, 0, sizeof(sizeof(struct cdc_dev)));
    cdc_d->fd = fd_cdc;

    return cdc_d;
}

void cdc_dev_close(struct cdc_dev *cdc_d) {
    close_serial(cdc_d->fd);
    cdc_d->fd = -1;
    free(cdc_d);
    cdc_d = NULL;
}

int cdc_dev_read(struct cdc_dev *cdc_d, unsigned char *data, int data_len, int *actual)
{
    *actual = read(cdc_d->fd, data, data_len);
    int ret;
    ret = (*actual > 0) ? 0 : -1;
    return ret;
}

void cdc_dev_read_sensor(struct cdc_dev *cdc_d) {
    unsigned char data[1024];
    int actual;
    if (cdc_dev_read(cdc_d, data, 1024, &actual) == 0) {
        sensor_data_decode(cdc_d, data, actual);
    }
}

void sensor_content_decode(struct cdc_dev *cdc_d, unsigned char *buf) {
    cJSON *jsonRoot = cJSON_Parse((char *)buf);
    cJSON *ctrlVal;
    if ((ctrlVal = cJSON_GetObjectItem(jsonRoot, "temperature")) != NULL) {
        cdc_d->sensor_temperature = ctrlVal->valuedouble;
    }
    if ((ctrlVal = cJSON_GetObjectItem(jsonRoot, "humidity")) != NULL) {
        cdc_d->sensor_humidity = ctrlVal->valuedouble;
    }
    if ((ctrlVal = cJSON_GetObjectItem(jsonRoot, "pressure")) != NULL) {
        cdc_d->sensor_pressure = ctrlVal->valuedouble;
    }
    cdc_d->updated = 1;
}

void sensor_data_decode(struct cdc_dev *cdc_d, unsigned char *buf, size_t len) {
    int i;
    for (i = 0; i < len; i ++) {
        if (buf[i] == '\r') {
            cdc_d->rev_content[cdc_d->rev_content_index] = '\0';
            cdc_d->rev_content_index ++;
            if (cdc_d->rev_content_index > MAX_REV_CDC_LEN) cdc_d->rev_content_index = 0;
            sensor_content_decode(cdc_d, cdc_d->rev_content);
            cdc_d->rev_content_index = 0;
        } else {
            cdc_d->rev_content[cdc_d->rev_content_index] = buf[i];
            cdc_d->rev_content_index ++;
            if (cdc_d->rev_content_index > MAX_REV_CDC_LEN) cdc_d->rev_content_index = 0;
        }
    }
}
