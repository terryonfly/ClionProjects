//
// Created by Terry on 16/1/20.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <mysql/mysql.h>

#include "DataBase.h"

void database_insert(char *sql) {
    MYSQL *conn;
    char *server = "robot.mokfc.com";
    char *user = "root";
    char *password = "513939";
    char *database = "sensors";

    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, CLIENT_MULTI_RESULTS)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    printf("will insert\n");
    if (mysql_query(conn, sql)) {
        printf("insert failure\n");
    } else {
        printf("insert %d rows\n", (int)mysql_affected_rows(conn));
    }

    mysql_close(conn);
}
