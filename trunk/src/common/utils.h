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
