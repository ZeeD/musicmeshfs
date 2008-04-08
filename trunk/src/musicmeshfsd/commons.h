#ifndef COMMONS_H
#define COMMONS_H

#include <sys/inotify.h>         /* struct inotify_event */
#include <unistd.h>              /* read */
#include <stdio.h>               /* perror, puts */
#include "../common/sqlite.h"    /* sqlite3 */
#include "inotify.h"             /* inotify_t */

int init_db_and_inotify(dynamic_str_t, sqlite3*, inotify_t*);
void local_loop(sqlite3*, inotify_t,
        int(*)(struct inotify_event*, sqlite3*, inotify_t));
int local_event_callback(struct inotify_event*, sqlite3*, inotify_t);
int walk(char*, int(*)(void*, const char*), void*, int(*)(void*, const char*),
        void*);

#endif
