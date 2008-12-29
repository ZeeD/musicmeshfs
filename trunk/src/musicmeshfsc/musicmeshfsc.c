/*
    Copyright © 2008 Vito De Tullio

    This file is part of MusicMeshFSc.

    MusicMeshFSc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MusicMeshFSc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MusicMeshFSc.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>              /* exit(), EXIT_FAILURE */
#include <signal.h>              /* signal, SIGINT */
#include <sqlite3.h>             /* sqlite3, sqlite3_open(), SQLITE_OK,
                                    sqlite3_errmsg(), sqlite3_close() */
#include <err.h>                 /* err(), warn() */
#include "../common/utils.h"     /* dynamic_obj_t, init_obj(), append_obj() */
#include "../db_fuse/parser.h"   /* parse_schema() */
#include "../db_fuse/db_fuse.h"  /* struct fuse_operations, db_fuse_getattr(),
        db_fuse_rename(), db_fuse_chmod(), db_fuse_read(),  db_fuse_readdir(),
        db_fuse_open(), db_fuse_release(), struct fuse_args, FUSE_ARGS_INIT,
        struct fuse_chan, fuse_parse_cmdline(), fuse_mount(), struct fuse,
        fuse_new(), fuse_loop(), fuse_unmount(), fuse_opt_free_args() */

struct fuse_args args;
struct fuse_chan* ch;
char* mountpoint;
int err_value = -1;

void chiudi_fuse_prima_di_morire(int) __attribute__ ((noreturn));

void chiudi_fuse_prima_di_morire(int unused) {
    unused = unused;
    fuse_unmount(mountpoint, ch);
    fuse_opt_free_args(&args);
    exit(err_value ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    if (argc != 4)
        err(EXIT_FAILURE, "USO: `%s DATABASE PATH MOUNT_SCHEMA'\n", argv[0]);
    struct stat tmp;
    if (stat(argv[1], &tmp) != 0)
        err(EXIT_FAILURE, "ERR: DATABASE `%s' non esistente!\n", argv[1]);
    if (stat(argv[2], &tmp) != 0)
        err(EXIT_FAILURE, "ERR: PATH `%s' non esistente!\n", argv[2]);
    dynamic_obj_t fissi, keywords;
    init_obj(&fissi);
    init_obj(&keywords);
    if (parse_schema(argv[3], &fissi, &keywords) != 0)
        err(EXIT_FAILURE, "ERR: MOUNT_SCHEMA `%s' non valido!\n", argv[3]);
    struct sigaction s = { .sa_handler = chiudi_fuse_prima_di_morire };
    sigaction(SIGINT, &s, NULL);
    sqlite3* db;
    if (sqlite3_open(argv[1], &db) != SQLITE_OK) {
        warn("sqlite3_open: `%s'\n", sqlite3_errmsg(db));
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
        struct fuse_args args2 = FUSE_ARGS_INIT(2, tmp_argv);
        args = args2;
        dynamic_obj_t parametro;
        init_obj(&parametro);
        append_obj(&parametro, db);
        append_obj(&parametro, &fissi);
        append_obj(&parametro, &keywords);
        if (fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1 &&
                (ch = fuse_mount(mountpoint, &args)) != NULL) {
            struct fuse* se;
            if ((se = fuse_new(ch, &args, &musicmeshfs_op,
                    sizeof musicmeshfs_op, &parametro)) != NULL)
                err_value = fuse_loop(se);
        }
    }
}
