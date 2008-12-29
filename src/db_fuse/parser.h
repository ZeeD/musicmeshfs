/*
    Copyright © 2008 Vito De Tullio

    This file is part of Fuse_DB.

    Fuse_DB is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fuse_DB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Fuse_DB.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>  /* malloc(), realloc(), calloc() */
#include <string.h>  /* strncmp(), strncpy() */
#include <err.h>     /* warn() */
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
