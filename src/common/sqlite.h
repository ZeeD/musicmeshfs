/*
    Copyright © 2008 Vito De Tullio

    This file is part of MusicMeshFS[cd].

    MusicMeshFS[cd] is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MusicMeshFS[cd] is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MusicMeshFS[cd].  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SQLITE_H
#define SQLITE_H

#include <stdarg.h>  /* va_start(), va_list, va_copy(), va_arg(), va_end() */
#include <sqlite3.h> /* sqlite3, sqlite3_open(), SQLITE_OK, sqlite3_errmsg(),
                        sqlite3_close(), sqlite3_exec(), sqlite3_free(),
                        sqlite3_vmprintf() */
#include <tag_c.h>   /* TagLib_File, taglib_file_new, TagLib_Tag,
                        taglib_file_tag, taglib_tag_title, taglib_tag_artist,
                        taglib_tag_album, taglib_tag_comment, taglib_tag_genre,
                        taglib_tag_year, taglib_tag_track, taglib_tag_set_title,
                        taglib_tag_set_artist, taglib_tag_set_album,
                        taglib_tag_set_comment, taglib_tag_set_genre,
                        taglib_tag_set_year, taglib_tag_set_track,
                        taglib_tag_free_strings, taglib_file_free */
#include <string.h>  /* strcmp() */
#include <err.h>     /* warn() */
#include "utils.h"   /* dynamic_obj_t */

int path_is_a_db(const char*);
sqlite3* crea_db_vuoto(char*);
int callback_print(void*, int, char**, char**);
int esegui_query(sqlite3*, const char*, ...);
int esegui_query_callback(sqlite3*, int(*callback)(void*, int, char**, char**),
        void*, const char*, ...);

int add_local_file_in_db(void*, const char*);
int del_local_file_from_db(void*, char*);

int get_columns(void*, int, char**, char**);

#endif
