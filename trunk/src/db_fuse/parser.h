#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>  /* malloc(), realloc(), calloc() */
#include <string.h>  /* strncmp(), strncpy() */
#include "../common/utils.h"   /* dynamic_obj_t */

extern const int IS_A_DIR;
extern const int IS_A_FILE;

// parsing dello schema

int parse_schema(const char*, dynamic_obj_t*, dynamic_obj_t*);
int nome(const char*, int, dynamic_str_t*, dynamic_str_t*);
int nome1(const char*, int, dynamic_str_t*, dynamic_str_t*);
int nome2(const char*, int, dynamic_str_t*, dynamic_str_t*);
int fisso(const char*, int, dynamic_str_t*);
int keyword(const char*, int, dynamic_str_t*);

// parsing dei path, usando i valori di output di parse_schema()

int parse_path(const char*, dynamic_obj_t, dynamic_obj_t, dynamic_obj_t*);

#endif
