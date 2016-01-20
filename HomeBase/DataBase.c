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

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char *server = "robot.mokfc.com";
char *user = "root";
char *password = "513939";
char *database = "sensors";

void database_init() {
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, CLIENT_MULTI_RESULTS)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }
    printf("database_init finished\n");
}

void database_release() {
    mysql_close(conn);
}

void database_query() {
    char *sql = "select * from `sensors`.`weather`";
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }
    res = mysql_use_result(conn);
    printf("SQL Command:%s\n", sql);
    while ((row = mysql_fetch_row(res)) != NULL) {
        printf("Row[1] : %s\n", row[1]);
    }
    mysql_free_result(res);
}

void database_insert(char *sql) {
    if (mysql_query(conn, sql)) {
        printf("执行插入失败");
    } else {
        printf("插入成功,受影响行数:%d\n", (int)mysql_affected_rows(conn));
    }
}
