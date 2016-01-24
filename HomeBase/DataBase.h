//
// Created by Terry on 16/1/20.
//

#ifndef HOMEBASE_DATABASE_H
#define HOMEBASE_DATABASE_H

#include "Platform.h"

#ifdef USE_MYSQL
void database_init();
void database_release();
void database_query();
void database_insert(char *sql);
int database_get_history(float *temperature, float *humidity, float *pressure, int history_len, char *time_min, char *time_max, int page, int page_size);
#endif

#endif //HOMEBASE_DATABASE_H
