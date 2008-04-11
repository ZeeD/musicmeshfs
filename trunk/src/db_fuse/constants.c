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

#include "constants.h"

/**
    Funzione di utilità che vede KEYWORDS e COLUMNS come un dizionario
    \param keyword elemento di KEYWORDS
    \return valore di COLUMNS corrispondente a keyword, NULL in caso d'errore
    \sa value_from_key(), where_from_join()
*/
const char* column_from_keyword(const char* keyword) {
    for (int i=0; i<KEYWORDS_SIZE; i++)
        if (!strcmp(KEYWORDS[i], keyword))
            return COLUMNS[i];
    return NULL;
}

/**
    Funzione di utilità che verifica se, e dove, un certo elemento è presente
    in un char*[], di dimensione nota
    \return pos(element) se element \f$ \in \f$ array,
            -1 altrimenti
    \sa index_of_str(), contains_str()
*/
int index_of(const char* array[], const int size, char* element) {
    for (int i=0; i<size; i++)
        if (!strcmp(element, array[i]))
            return i;
    return -1;
}

/**
    Funzione di utilità che vede JOINS e WHERE come un dizionario
    \param join elemento di JOINS
    \return valore di WHERE corrispondente a join, NULL in caso d'errore
    \sa value_from_key(), column_from_keyword()
*/
const char* where_from_join(const char* join) {
    for (int i=0; i<JOINS_SIZE; i++)
        if (!strcmp(JOINS[i], join))
            return WHERE[i];
    return NULL;
}
