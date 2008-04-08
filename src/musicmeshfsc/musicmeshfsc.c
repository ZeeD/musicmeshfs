#include "../common/utils.h"     /* errprintf(), dynamic_obj_t, init_obj(),
                                    append_obj() */
#include <stdlib.h>              /* exit(), EXIT_FAILURE */
#include "../db_fuse/parser.h"   /* parse_schema() */
#include <sqlite3.h>             /* sqlite3, sqlite3_open(), SQLITE_OK,
                                    sqlite3_errmsg(), sqlite3_close() */
#include "../db_fuse/db_fuse.h"  /* struct fuse_operations, db_fuse_getattr(),
        db_fuse_rename(), db_fuse_chmod(), db_fuse_read(),  db_fuse_readdir(),
        db_fuse_open(), db_fuse_release(), struct fuse_args, FUSE_ARGS_INIT,
        struct fuse_chan, fuse_parse_cmdline(), fuse_mount(), struct fuse,
        fuse_new(), fuse_loop(), fuse_unmount(), fuse_opt_free_args() */

int main(int argc, char** argv) {
    if (argc != 4) {
        errprintf("USO: `%s DATABASE PATH MOUNT_SCHEMA'\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct stat tmp;
    if (stat(argv[1], &tmp) != 0) {
        errprintf("ERR: DATABASE `%s' non esistente!\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (stat(argv[2], &tmp) != 0) {
        errprintf("ERR: PATH `%s' non esistente!\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    dynamic_obj_t fissi, keywords;
    init_obj(&fissi);
    init_obj(&keywords);
    if (parse_schema(argv[3], &fissi, &keywords) != 0) {
        errprintf("ERR: MOUNT_SCHEMA `%s' non valido!\n", argv[3]);
        exit(EXIT_FAILURE);
    }
    // TODO: mettere in loop un oggetto di tipo sqlite3*!!
    sqlite3* db;
    if (sqlite3_open(argv[1], &db) != SQLITE_OK) {
        errprintf("sqlite3_open: `%s'\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    else {
        struct fuse_operations musicmeshfs_op = {
            .getattr = db_fuse_getattr, // ll -l
            .rename  = db_fuse_rename,  // mv
            .chmod   = db_fuse_chmod,   // chmod
            .read    = db_fuse_read,    // cat
            .readdir = db_fuse_readdir, // ls
            .open    = db_fuse_open,    // cat (ottimizza)
            .release = db_fuse_release  // cat (ottimizza)
        };

        char* tmp_argv[] = { argv[0], argv[2] };
        struct fuse_args args = FUSE_ARGS_INIT(2, tmp_argv);
        struct fuse_chan* ch;
        char* mountpoint;
        int err = -1;

        dynamic_obj_t* parametro;
        init_obj(parametro);
        append_obj(parametro, db);
        append_obj(parametro, &fissi);
        append_obj(parametro, &keywords);

        if (fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1 &&
                (ch = fuse_mount(mountpoint, &args)) != NULL) {
            struct fuse* se;
            if ((se = fuse_new(ch, &args, &musicmeshfs_op,
                    sizeof musicmeshfs_op, parametro)) != NULL)
                err = fuse_loop(se);
        }
        fuse_unmount(mountpoint, ch);
        fuse_opt_free_args(&args);

        return err ? 1 : 0;
    }
}
