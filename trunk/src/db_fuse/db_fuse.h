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

#ifndef DB_FUSE_H
#define DB_FUSE_H

#define FUSE_USE_VERSION 26  /* richiesto da <fuse.h> */
#include <fuse.h>            /* fuse_fill_dir_t, struct fuse_file_info */
#include <sys/stat.h>        /* struct stat */
#include <errno.h>           /* errno */
#include <unistd.h>          /* pread() */
#include <sqlite3.h>         /* sqlite3 */
#include "../common/utils.h" /* dynamic_obj_t */

int db_fuse_readdir(const char*, void*, fuse_fill_dir_t, off_t, struct
        fuse_file_info*);
int db_fuse_getattr(const char*, struct stat*);
int db_fuse_rename(const char*, const char*);
int db_fuse_chmod(const char*, mode_t);
int db_fuse_read(const char*, char*, size_t, off_t, struct fuse_file_info*);
int db_fuse_open(const char*, struct fuse_file_info*);
int db_fuse_release(const char*, struct fuse_file_info*);

// dovranno essere reimplementate -> metodi virtuali -.-
extern char* get_local_path(sqlite3*, dynamic_obj_t, dynamic_obj_t);
extern int is_local_file(sqlite3*, dynamic_obj_t, dynamic_obj_t);

#endif
