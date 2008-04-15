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

#ifndef DB_FUSE_VARIANTS_H
#define DB_FUSE_VARIANTS_H

#include "../common/utils.h"    /* dynamic_str_t */
#include "../common/sqlite.h"   /* sqlite3 */

dynamic_str_t get_local_paths(sqlite3*, dynamic_obj_t, dynamic_obj_t, dynamic_obj_t);
int is_local_file(sqlite3*, dynamic_obj_t, dynamic_obj_t, dynamic_obj_t);
int rename_local_file(void*, dynamic_obj_t, dynamic_obj_t, dynamic_obj_t, dynamic_obj_t);

#endif
