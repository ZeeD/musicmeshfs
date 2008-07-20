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

#ifndef OPTPARSE_H
#define OPTPARSE_H

#include "../common/utils.h" /* dynamic_obj_t, dynamic_str_t */
#include <argp.h>

/**
    Struttura che ospiterà le varie opzioni
*/
typedef struct {
    dynamic_str_t chiavi;
    dynamic_obj_t valori;
} option_parser_t;

extern const char* argp_program_version;
extern const char* argp_program_bug_address;
extern const char* doc;
extern const char* args_doc;

option_parser_t init_option_parser();
int add_option_int(option_parser_t*, char*, char*, char*, int);
int add_option_str(option_parser_t*, char*, char*, char*, char*);
int add_option(option_parser_t*, char*, char*, char*, int);
int parse_args(option_parser_t*, int*, char**[]);

#endif
