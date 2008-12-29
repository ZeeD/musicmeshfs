/*
    Copyright © 2008 Vito De Tullio

    This file is part of MusicMeshFSd.

    MusicMeshFSd is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MusicMeshFSd is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MusicMeshFSd.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COMMONS_H
#define COMMONS_H

#include <sys/inotify.h>         /* struct inotify_event */
#include <unistd.h>              /* read */
#include <stdio.h>               /* perror, puts */
#include <err.h>                 /* warn() */
#include "../common/sqlite.h"    /* sqlite3 */
#include "inotify.h"             /* inotify_t */

int init_db_and_inotify(dynamic_str_t, sqlite3*, inotify_t*);
void local_loop(sqlite3*, inotify_t, int(*)(struct inotify_event*, sqlite3*,
        inotify_t))  __attribute__ ((noreturn));
int local_event_callback(struct inotify_event*, sqlite3*, inotify_t);
int walk(char*, int(*)(void*, const char*), void*, int(*)(void*, const char*),
        void*);

#endif
