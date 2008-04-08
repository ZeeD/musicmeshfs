#ifndef SQLITE_H
#define SQLITE_H

#include <stdarg.h>  /* va_start, va_list, va_copy, va_arg, va_end */
#include <sqlite3.h> /* sqlite3, sqlite3_open, SQLITE_OK, sqlite3_errmsg,
                        sqlite3_close, sqlite3_exec, sqlite3_free,
                        sqlite3_vmprintf */
#include <tag_c.h>   /* TagLib_File, taglib_file_new, TagLib_Tag,
                        taglib_file_tag, taglib_tag_title, taglib_tag_artist,
                        taglib_tag_album, taglib_tag_comment, taglib_tag_genre,
                        taglib_tag_year, taglib_tag_track, taglib_tag_set_title,
                        taglib_tag_set_artist, taglib_tag_set_album,
                        taglib_tag_set_comment, taglib_tag_set_genre,
                        taglib_tag_set_year, taglib_tag_set_track,
                        taglib_tag_free_strings, taglib_file_free */

sqlite3* crea_db_vuoto(char*);
int callback_print(void*, int, char**, char**);
int esegui_query(sqlite3*, char*, ...);
int esegui_query_callback(sqlite3*, int(*callback)(void*, int, char**, char**),
        void*, char*, ...);
int add_local_file_in_db(void*, const char*);
int del_local_file_from_db(void*, char*);

#endif
