/*
    Copyright © 2008 Vito De Tullio

    This file is part of Fuse_DB.

    Fuse_DB is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fuse_DB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Fuse_DB.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DB_FUSE_UTILS_H
#define DB_FUSE_UTILS_H

#include <sqlite3.h>         /* sqlite3, sqlite3_mprintf(), sqlite3_free() */
#include "../common/utils.h" /* dynamic_obj_t */
#define FUSE_USE_VERSION 26  /* richiesto da <fuse.h> */
#include <fuse.h>            /* fuse_get_context() */
#include <stdlib.h>          /* calloc() */

sqlite3* get_db_from_context();
dynamic_obj_t get_fissi_from_context();
dynamic_obj_t get_keywords_from_context();

int exist(sqlite3*, dynamic_obj_t, dynamic_obj_t, dynamic_obj_t, const char*);

dynamic_str_t sub_dir(sqlite3*, dynamic_obj_t, dynamic_obj_t, dynamic_obj_t);

char* calcola_query_subdirs(dynamic_str_t, dynamic_str_t);
void cercaPercorso(const char*, const char*, dynamic_str_t*, dynamic_str_t*);
int calcola_tabelle(dynamic_str_t, dynamic_str_t*);
void calcola_where(dynamic_str_t, dynamic_str_t*);

int get_one_column(void*, int, char**, char**);
int get_two_columns(void*, int, char**, char**);

#endif
