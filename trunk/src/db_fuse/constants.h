/*
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "string.h"  /* strcmp(), NULL */

/** Nome della tabelle sulle quali si vuole basare il fs */
extern const char* TABLES[];

/** Numero di elementi in TABLES */
extern const int TABLES_SIZE;

/** Parole chiave che il programma deve capire (user unterface) */
extern const char* KEYWORDS[];

/** Array di Tabella.Colonna, corrispondenti a ogni elemento di KEYWORDS */
extern const char* COLUMNS[];

/** Numero di elementi di KEYWORDS (implicitamente anche di COLUMNS) */
extern const int KEYWORDS_SIZE;

/** Tabella.Colonna indicante l'estensione del singolo file */
extern const char* COLUMN_EXT;

/** Array di Tabella.Tabella di join supportati */
extern const char* JOINS[];

/** Condizioni WHERE corrispondenti ad ogni elemento di JOINS */
extern const char* WHERE[];

/** Numero di elementi di JOINS (implicitamente anche di WHERE) */
extern const int JOINS_SIZE;

/** \todo decidere che fare di MISC_COLUMNS */
extern const char* MISC_COLUMNS[];

/** Numero di elementi di MISC_COLUMNS */
extern const int MISC_COLUMNS_SIZE;

// *********************** UTILITÀ ***********************

const char* column_from_keyword(const char*);
int index_of(const char*[], const int, char*);
const char* where_from_join(const char*);

#endif
