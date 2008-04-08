#ifndef DB_FUSE_VARIANTS_H
#define DB_FUSE_VARIANTS_H

#include "../common/utils.h"    /* dynamic_str_t */
#include "../common/sqlite.h"   /* sqlite3 */

char* get_local_path(sqlite3*, dynamic_obj_t, dynamic_obj_t);
int is_local_file(sqlite3*, dynamic_obj_t, dynamic_obj_t);

// int get_size(sqlite3*, dynamic_str_t, dynamic_str_t);
// int is_a_dir(sqlite3*, dynamic_str_t, dynamic_str_t);

#endif
