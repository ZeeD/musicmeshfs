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

#ifndef UTILS_H
#define UTILS_H

#include <dirent.h>      /* DIR, dir, opendir, readdir, closedir */
#include <stdio.h>       /* perror */
#include <string.h>      /* NULL, size_t, strlen, strcat, strdup */
#include <sys/stat.h>    /* stat, S_ISREG, S_ISDIR, S_IROTH, S_IWOTH */
#include <stdlib.h>      /* realloc, malloc, free */
#include <stdarg.h>      /* va_list, va_start, va_end */

/**
    vettore di interi a dimensione variabile

    da usare con le altre funzioni presenti in utils.h
    \param size dimensione corrente del vettore
    \param buf vettore vero e proprio
    \sa init_int(), append_int(), extend_int(), free_int()
*/
typedef struct {
    int size;
    int* buf;
} dynamic_int_t;

/**
    vettore di stringhe a dimensione variabile

    da usare con le altre funzioni presenti in utils.h
    \param size dimensione corrente del vettore
    \param buf vettore vero e proprio
    \sa init_str(), append_str(), extend_str(), free_str()
*/
typedef struct {
    int size;
    char** buf;
} dynamic_str_t;

void extend_int(dynamic_int_t*, const dynamic_int_t);
void extend_str(dynamic_str_t*, const dynamic_str_t);
void free_int(dynamic_int_t*);
void free_str(dynamic_str_t*);
void append_int(dynamic_int_t*, const int);
void append_str(dynamic_str_t*, const char*);
void init_int(dynamic_int_t*);
void init_str(dynamic_str_t*);
dynamic_int_t from_int(const int*, const int);
dynamic_str_t from_str(const char**, const int);
void remove_str(dynamic_str_t*, const char*);
void remove_int(dynamic_int_t*, const int);
int pop_str(dynamic_str_t*);

int contains_int(const dynamic_int_t, const int);
int contains_str(const dynamic_str_t, const char*);
int index_of_int(const dynamic_int_t, const int);
int index_of_str(const dynamic_str_t, const char*);

dynamic_int_t slice_int(const dynamic_int_t, const int, const int);
dynamic_str_t slice_str(const dynamic_str_t, const int, const int);

char* preprocessing(const char*);
const char* extract_extension(const char*);
int extract_group_perm(const char*);
int extract_size(const char*);
const char* extract_basename(const char*);
char* extract_dirname(const char*);

dynamic_str_t split(const char*, const char);
char* join(const dynamic_str_t, const char);

void dbgprint_str(dynamic_str_t, char*);
void dbgprint_int(dynamic_int_t, char*);

char* strmalloccat(char*, const char*);

int errprintf(char*, ...);

char* value_from_key(dynamic_str_t, dynamic_str_t, char*);

/**
    vettore di "oggetti non meglio specificati" a dimensione variabile

    da usare con le altre funzioni presenti in utils.h
    \param size dimensione corrente del vettore
    \param buf vettore vero e proprio
    \sa init_str(), append_str(), extend_str(), free_str()
 */
typedef struct {
    int size;
    const void** buf;
} dynamic_obj_t;

void init_obj(dynamic_obj_t*);
void free_obj(dynamic_obj_t*);
void append_obj(dynamic_obj_t*, const void*);

int max(int, int);
int startswith(const char*, const char*);

#endif
