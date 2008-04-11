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

#include "db_fuse_utils.h"
#include "constants.h"   /* KEYWORDS_SIZE, KEYWORDS, column_from_keyword */
#include "../common/utils.h"       /* dynamic_str_t, init_str(), append_str(),
                            index_of_str(), max() */
#include "../common/sqlite.h"      /* esegui_query_callback() */

/**
    utility per ottenere, da fuse, il database passato come parametro
*/
sqlite3* get_db_from_context() {
    return (sqlite3*)((dynamic_obj_t*)fuse_get_context()->private_data)->buf[0];
}

/**
    utility per ottenere, da fuse, il vettore degli elementi fissi passato come
    parametro
*/
dynamic_obj_t get_fissi_from_context() {
    return *(dynamic_obj_t*)((dynamic_obj_t*)fuse_get_context()->private_data)->
            buf[1];
}

/**
    utility per ottenere, da fuse, il vettore degli elementi keywords passato
    come parametro
*/
dynamic_obj_t get_keywords_from_context() {
    return *(dynamic_obj_t*)((dynamic_obj_t*)fuse_get_context()->private_data)->
            buf[2];
}

/**
    \todo stub
*/
int exist(sqlite3* db, dynamic_obj_t chiavi, dynamic_obj_t valori) {
    errprintf("[EXIST] Warn, stub\n");
    return 1;
}

/**
    ritorna il contenuto di una directory :)
    assunto: 0 < fissi.file == keywords.size > dinamici.size (altrimenti erano
            stati generati da un file)
*/
dynamic_str_t sub_dir(sqlite3* db, dynamic_obj_t fissi, dynamic_obj_t keywords,
        dynamic_obj_t dinamici) {
    dynamic_str_t ret;
    init_str(&ret);
    dynamic_str_t fissi_file = *(dynamic_str_t*)fissi.buf[dinamici.size];
    dynamic_str_t keywords_file = *(dynamic_str_t*)keywords.buf[dinamici.size];

    char* query = strmalloccat(calloc(1, 1), "SELECT DISTINCT (\n");

    for (int i=0; i<keywords_file.size; i++) {
        if (i)
            query = strmalloccat(query, " ||");
        char* tmp = sqlite3_mprintf("\t%Q ||\n", fissi_file.buf[i]);
        query = strmalloccat(query, tmp);
        sqlite3_free(tmp);
        if (fissi_file.size > i+1)          // se è seguito da un elemento fisso
            query = strmalloccat(query, "\tREPLACE(");
        else
            query = strmalloccat(query, "\t");
        query = strmalloccat(strmalloccat(strmalloccat(query, "REPLACE("),
                column_from_keyword(keywords_file.buf[i])), ", '/', '_')");
        if (fissi_file.size > i+1) {        // se è seguito da un elemento fisso
            char* tmp = sqlite3_mprintf(", %Q, '_')", fissi_file.buf[i+1]);
            query = strmalloccat(query, tmp);
            sqlite3_free(tmp);
        }
    }

    dynamic_str_t tabelle, where;
    init_str(&tabelle);
    if (calcola_tabelle(keywords_file, &tabelle) == -1)
        return ret;

    query = strmalloccat(strmalloccat(query, "\n) FROM\n\t"), tabelle.buf[0]);

    calcola_where(tabelle, &where);

    for (int i=1; i<tabelle.size; i++)
        query = strmalloccat(strmalloccat(query, ", "), tabelle.buf[i]);

    if (where.size)
        query = strmalloccat(query, "\nWHERE\n\t");

    for (int i=0; i<where.size; i++) {
        if (i)
            query = strmalloccat(query, " AND\n\t");
        query = strmalloccat(query, where.buf[i]);
    }

    // TODO: ulteriori condizioni di where (delle sottodirectories)

    errprintf("sub_dir->query = `%s'\n", query);
    esegui_query_callback(get_db_from_context(), get_one_column, &ret, query);
    free(query);
    return ret;
}

int calcola_tabelle(dynamic_str_t keywords, dynamic_str_t* tabelle) {
    for (int i=0; i<keywords.size; i++)
        // patch per COUNT(COLONNA)
        if (strncmp(keywords.buf[i], "COUNT(", 6)) {
            const char* colonna = column_from_keyword(keywords.buf[i]);
            if (!colonna)
                return -1;
            char* tabella = split(colonna, '.').buf[0];
            if (index_of(TABLES, TABLES_SIZE, tabella) == -1)
                return -1;
            if (!contains_str(*tabelle, tabella))
                append_str(tabelle, tabella);
        }
    if (!tabelle->size)
        return -1;
    return 0;
}

void calcola_where(dynamic_str_t tabelle, dynamic_str_t* where) {
    init_str(where);
    // calcolo condizioni where dei join
    for (int i=0; i<tabelle.size; i++)
        for (int j=i+1; j<tabelle.size; j++) {
            char* titj = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    tabelle.buf[i]), ","), tabelle.buf[j]);
            char* tjti = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    tabelle.buf[j]), ","), tabelle.buf[i]);
            // se ci sono entrambi in un elemento di JOINS, metticelo e basta
            if (index_of(JOINS, JOINS_SIZE, titj) != -1)
                append_str(where, where_from_join(titj));
            else if (index_of(JOINS, JOINS_SIZE, tjti) != -1)
                append_str(where, where_from_join(tjti));
            else
                cercaPercorso(tabelle.buf[i], tabelle.buf[j], &tabelle, where);
            free(titj);
            free(tjti);
        }
}

/**
    Assume che buf sia un dynamic_str_t, e che la query abbia n_colonne == 1
    Riempie buf con le tuple trovate
*/
int get_one_column(void* buf, int n_colonne, char** value, char** header) {
    (void) header; // non usato
    if (n_colonne != 1)
        return -1;
    append_str(((dynamic_str_t*)buf), value[0]);
    return 0;
}

/**
    Assume che buf sia un dynamic_obj_t, composto da 2 dynamic_str_t, e che la
    query abbia n_colonne == 2
    Riempie le coppie di buf con le tuple trovate
 */
int get_two_columns(void* buf, int n_colonne, char** value, char** header) {
    (void) header; // non usato
    if (n_colonne != 1)
        return -1;
    append_str(((dynamic_str_t*)((dynamic_obj_t*)buf)->buf[0]), value[0]);
    append_str(((dynamic_str_t*)((dynamic_obj_t*)buf)->buf[1]), value[1]);
    return 0;
}

// /**
//     Assume che chiavi.size == valori.size + 1, e che chiavi indichi le KEYWORDS
//     dell'utente! (\f$ \forall chiave \in chiavi \to chiave \in KEYWORDS \f$)
//     \param chiavi keyword \f$ \in \f$ KEYWORDS
//     \param valori valori presenti nel database nelle colonne nelle colonne
//             corrispondenti a chiavi
//     \return query del tipo "SELECT chiavi[-1] ... WHERE (chiavi[:-1] = valori)",
//             NULL in caso d'errore
// */
// char* calcola_query_subdirs(dynamic_str_t chiavi, dynamic_str_t valori) {
//     if (chiavi.size != valori.size + 1)
//         return NULL;
//     dynamic_str_t colonne, tabelle, where;
//     init_str(&colonne);
//     init_str(&tabelle);
//     init_str(&where);
//     for (int i=0; i<chiavi.size; i++) {
//         // patch per COUNT(COLONNA)
//         if (!strncmp(chiavi.buf[i], "COUNT(", 6))
//             append_str(&colonne, chiavi.buf[i]);
//         else {
//             const char* colonna = column_from_keyword(chiavi.buf[i]);
//             if (!colonna)
//                 return NULL;
//             append_str(&colonne, colonna);
//             char* tabella = split(colonna, '.').buf[0];
//             if (index_of(TABLES, TABLES_SIZE, tabella) == -1)
//                 return NULL;
//             if (!contains_str(tabelle, tabella))
//                 append_str(&tabelle, tabella);
//         }
//     }
//     if (!tabelle.size)
//         return NULL;
//
//     /* ora in tabelle ho tutte le tabelle necessarie, mi tocca calcolare quali
//     sono le corrispettive condizioni di where + le ulteriori condizioni di
//     where! */
//
//     char* query = strmalloccat(strmalloccat(strmalloccat(strmalloccat(calloc(1,
//             1), "SELECT DISTINCT "), colonne.buf[colonne.size-1]), " FROM "),
//             tabelle.buf[0]);
//
// //     errprintf("calcola_query_subdirs->query = `%s'\n", query);
//
//     // calcolo condizioni where dei join
//     for (int i=0; i<tabelle.size; i++)
//         for (int j=i+1; j<tabelle.size; j++) {
//             char* titj = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
//                     tabelle.buf[i]), ","), tabelle.buf[j]);
//             char* tjti = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
//                     tabelle.buf[j]), ","), tabelle.buf[i]);
//             // se ci sono entrambi in un elemento di JOINS, metticelo e basta
//             if (index_of(JOINS, JOINS_SIZE, titj) != -1)
//                 append_str(&where, where_from_join(titj));
//             else if (index_of(JOINS, JOINS_SIZE, tjti) != -1)
//                 append_str(&where, where_from_join(tjti));
//             else
//                 cercaPercorso(tabelle.buf[i], tabelle.buf[j], &tabelle, &where);
//             free(titj);
//             free(tjti);
//         }
//
//     for (int i=1; i<tabelle.size; i++)
//         query = strmalloccat(strmalloccat(query, ", "), tabelle.buf[i]);
//
//     // innesto delle condizioni where dei join nella query
//
//     if (where.size || valori.size)
//         query = strmalloccat(query, " WHERE ");
//
//     if (where.size)
//         query = strmalloccat(query, where.buf[0]);
//     for (int i=1; i<where.size; i++)
//         query = strmalloccat(strmalloccat(query, " AND "), where.buf[i]);
//
//     // ulteriori condizioni di where (quelle di chiavi[:-1] = valori)
//     if (!where.size && valori.size) {
//         char* quoted = sqlite3_mprintf(") GLOB %Q)", valori.buf[0]);
//         query = strmalloccat(strmalloccat(strmalloccat(query, "(prep("),
//                 column_from_keyword(chiavi.buf[0])), quoted);
//         sqlite3_free(quoted);
//     }
//     for (int i=(where.size?0:1); i<valori.size; i++) {
//         char* quoted = sqlite3_mprintf(") GLOB %Q)", valori.buf[i]);
//         query = strmalloccat(strmalloccat(strmalloccat(query, " AND (prep("),
//                 column_from_keyword(chiavi.buf[i])), quoted);
//         sqlite3_free(quoted);
//     }
//
// //     query = strmalloccat(strmalloccat(query, " GROUP BY "),
// //             colonne.buf[colonne.size-1]);
// //     errprintf("calcola_query_subdirs) query = `%s'\n", query);
//     free_str(&colonne);
//     free_str(&tabelle);
//     free_str(&where);
//     return query;
// }

/**
    Funzione di utilità che, date due tabelle non presenti in JOINS, mi cerca il
    join più adatto.
    \param t1 tabella presente in TABLES
    \param t2 tabella associata a t1, non presente in TABLES
    \param tabelle tabelle da mettere in join (modificato in-place)
    \param where condizioni di where associate ai join della query (modificato
            in-place)
    \attention al momento funziona solo se \f$ \exists \mathrm{t_{3}} \in
            \mathrm{TABLES} \mbox{ tale che }
            \left(\mathrm{t_{1}},\mathrm{t_{3}}\right) \in \mathrm{JOINS} \land
            \left(\mathrm{t_{3}},\mathrm{t_{2}}\right) \in \mathrm{JOINS} \f$
*/
void cercaPercorso(const char* t1, const char* t2, dynamic_str_t* tabelle,
        dynamic_str_t* where) {
    int done = 0;
    for (int i=0; i<TABLES_SIZE && !done; i++) {
        if (!strcmp(t1, TABLES[i]) || !strcmp(t2, TABLES[i]))
            continue;   // devo scoprire t3 != t1 e t2
        char* t1t3 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1), t1),
                ","), TABLES[i]);
        char* t3t1 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                TABLES[i]), ","), t1);
        int m = 0, n = 0;
        if ((m = max(index_of(JOINS, JOINS_SIZE, t1t3), index_of(JOINS,
                JOINS_SIZE, t3t1))) != -1) {
            char* t3t2 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    TABLES[i]), ","), t2);
            char* t2t3 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    t2), ","), TABLES[i]);
            if ((n = max(index_of(JOINS, JOINS_SIZE, t3t2), index_of(JOINS,
                    JOINS_SIZE, t2t3))) != -1) {
                // ho m e n che sono 2 indici di JOINS, t3 = TABLES[i]
                if (!contains_str(*tabelle, TABLES[i]))
                    append_str(tabelle, TABLES[i]);
                append_str(where, WHERE[m]);
                append_str(where, WHERE[n]);
                break;
                done = !done;
            }
            free(t2t3);
            free(t3t2);
        }
        free(t1t3);
        free(t3t1);
    }
}
