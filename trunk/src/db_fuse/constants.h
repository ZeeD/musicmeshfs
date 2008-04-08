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
