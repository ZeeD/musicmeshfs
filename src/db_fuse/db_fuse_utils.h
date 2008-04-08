#ifndef DB_FUSE_UTIS_H
#define DB_FUSE_UTIS_H

#include <sqlite3.h>         /* sqlite3, sqlite3_mprintf(), sqlite3_free() */
#include "../common/utils.h" /* dynamic_obj_t */
#define FUSE_USE_VERSION 26  /* richiesto da <fuse.h> */
#include <fuse.h>            /* fuse_get_context() */
#include <stdlib.h>          /* calloc() */

sqlite3* get_db_from_context();
dynamic_obj_t get_fissi_from_context();
dynamic_obj_t get_keywords_from_context();

int exist(sqlite3*, dynamic_obj_t, dynamic_obj_t);
dynamic_str_t sub_dir(sqlite3*, dynamic_obj_t, dynamic_obj_t);

char* calcola_query_subdirs(dynamic_str_t, dynamic_str_t);
void cercaPercorso(const char*, const char*, dynamic_str_t*, dynamic_str_t*);
int get_one_column(void*, int, char**, char**);
int get_two_columns(void*, int, char**, char**);

#endif
