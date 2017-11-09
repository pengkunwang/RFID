#ifndef __DATA_H_
#define __DATA_H_
#include "sqlite3.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//创建表
char *creat_table();

int show_cabk(void *noused, int argc, char **argv, char **colname);
void show_all(sqlite3 *db);
int find(sqlite3 *db, char *cardid);
void insert(sqlite3 *db, char *carID, char *licence, char *time_in, char *photo);
void delete(sqlite3 *db, char *cardid);
void update(sqlite3 *db, char *cardid, char *time);
char *gettime();
#endif