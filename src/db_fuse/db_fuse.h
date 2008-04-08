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
