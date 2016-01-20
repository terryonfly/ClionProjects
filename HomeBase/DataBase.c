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

void test() {
    char *begin = "\n+--------------BEGIN---------------+\n\n";
    printf(begin);

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "robot.mokfc.com";
    char *user = "root";
    char *password = "513939";
    char *database = "sensors";

    conn = mysql_init(NULL); /* Connect to database */

    /*
    * CLIENT_MULTI_RESULTS
    * 通知服务器，客户端能够处理来自多语句执行或存储程序的多个结果集。
    * 如果设置了CLIENT_MULTI_STATEMENTS，将自动设置它。
   */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, CLIENT_MULTI_RESULTS)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    char *tell = "SQL Table Query...\n";
    printf(tell);
    // SQL 普通表查询
    char *sql = "select * from `sensors`.`weather`";
    if (mysql_query(conn, sql)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }
    res = mysql_use_result(conn);
    printf("SqlCommand:%s", sql);
    printf("\n");
    while ((row = mysql_fetch_row(res)) != NULL) {
        printf("PassWord:%s \n\n", row[1]);
    }
    mysql_free_result(res);

    mysql_close(conn);

    char *end = "+--------------END----------------+\n";
    printf(end);
}
