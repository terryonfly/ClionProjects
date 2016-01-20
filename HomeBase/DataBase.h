//
// Created by Terry on 16/1/20.
//

#ifndef HOMEBASE_DATABASE_H
#define HOMEBASE_DATABASE_H

void database_init();

void database_release();

void database_query();

void database_insert(char *sql);

#endif //HOMEBASE_DATABASE_H
