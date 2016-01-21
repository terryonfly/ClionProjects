//
// Created by Terry on 16/1/20.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <time.h>

#include "Platform.h"

#ifdef USE_MYSQL
#include <mysql/mysql.h>

#include "DataBase.h"

void database_insert(char *sql) {
    MYSQL *conn;
    char *server = "localhost";
    char *user = "root";
    char *password = "513939";
    char *database = "sensors";

    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, CLIENT_MULTI_RESULTS)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    if (mysql_query(conn, sql)) {
        printf("insert failure\n");
    } else {
//        printf("insert %d rows\n", (int)mysql_affected_rows(conn));
    }

    mysql_close(conn);
}

int database_get_history(float *temperature, float *humidity, float *pressure, int history_len, char *time_min, char *time_max, int page, int page_size) {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "localhost";
    char *user = "root";
    char *password = "513939";
    char *database = "sensors";

    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, CLIENT_MULTI_RESULTS)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return -1;
    }

//    char *sql_format = "select * from weather where time > '%s' and time < '%s' limit %d, %d";
    char *sql_format = "select * from weather limit %d, %d";
    char *sql = malloc(512);
    sprintf(sql, sql_format, time_min, time_max, page * page_size, page_size);
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return -1;
    }
    res = mysql_use_result(conn);
    printf("SqlCommand:%s",sql);
    printf("\n");
    int actual = 0;
    while ((row = mysql_fetch_row(res)) != NULL) {
        temperature[actual] = atof(row[1]);
        humidity[actual] = atof(row[2]);
        pressure[actual] = atof(row[3]);
        actual ++;
    }
    mysql_free_result(res);

    mysql_close(conn);
    return actual;
}
#endif
