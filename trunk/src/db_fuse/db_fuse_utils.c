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
    verifica che sia presente nel database almeno un elemento per cui siano
    rispettati i vincoli di ogni dizionario rappresentato dagli elementi (a
    coppie) di chiavi e valori

    \param db database su cui effettuare la ricerca
    \param fissi vettore di elementi fissi (uno per directory)
    \param chiavi vettore di chiavi (una per directory)
    \param valori vettore di valori (una per directory)
    \return 1 se l'elemento esiste
            0 altrimenti

    \attention do per scontato valori.size <= chiavi.size e che chiavi[i].size
            == valori[i].size (valori è stato generato da parser())
    \sa parser()
*/
int exist(sqlite3* db, dynamic_obj_t fissi, dynamic_obj_t keywords,
        dynamic_obj_t dinamici, const char* path) {
    dynamic_str_t splitted_path = split(path+1, '/');
    char* filename = splitted_path.buf[splitted_path.size-1];
//     errprintf("[EXIST] path = `%s'\n", filename);
    if (dinamici.size == 0) // mi sto riferendo a "/"
        return 1;
    dynamic_obj_t dinamici_superiore;
    init_obj(&dinamici_superiore);
    for (int i=0; i<dinamici.size-1; i++)
        append_obj(&dinamici_superiore, dinamici.buf[i]);
    dynamic_str_t contenuto_superiore = sub_dir(db, fissi, keywords,
            dinamici_superiore);

    int ret = contains_str(contenuto_superiore, filename);
    free_str(&splitted_path);
    return ret;
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
    char* query = strmalloccat(calloc(1, 1), "SELECT DISTINCT (\n");

    dynamic_str_t fissi_file = *(dynamic_str_t*)fissi.buf[dinamici.size];
    dynamic_str_t keywords_file = *(dynamic_str_t*)keywords.buf[dinamici.size];
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
    for (int i=0; i<keywords.size; i++)
        if (calcola_tabelle(*(dynamic_str_t*)keywords.buf[i], &tabelle) == -1)
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

    int usa_where = 0;
    for (int i=0; i<dinamici.size; i++)
        if (((dynamic_str_t*)dinamici.buf[i])->size) {
            usa_where = 1;
            break;
        }
    if (!where.size && usa_where)
        query = strmalloccat(query, "\nWHERE\n\t");

    for (int i=0; i<dinamici.size; i++) {
        dynamic_str_t fissi2 = *(dynamic_str_t*)(fissi.buf[i]);
        dynamic_str_t chiavi = *(dynamic_str_t*)(keywords.buf[i]);
        dynamic_str_t valori = *(dynamic_str_t*)(dinamici.buf[i]);

        //assumo che nella directory chiavi.size == valori.size
        for (int j=0; j<chiavi.size; j++) {
            if (i || where.size)
                query = strmalloccat(query, " AND\n");
            if (fissi2.size > j+1)          // se è seguito da un elemento fisso
                query = strmalloccat(query, "\t(REPLACE(");
            else
                query = strmalloccat(query, "\t(");
            query = strmalloccat(strmalloccat(strmalloccat(query, "REPLACE("),
                                 column_from_keyword(chiavi.buf[j])), ", '/', '_')");
            if (fissi2.size > j+1) {        // se è seguito da un elemento fisso
                char* tmp = sqlite3_mprintf(", %Q, '_')", fissi2.buf[j+1]);
                query = strmalloccat(query, tmp);
                sqlite3_free(tmp);
            }
            char* tmp = sqlite3_mprintf(" GLOB %Q)", valori.buf[j]);
            query = strmalloccat(query, tmp);
            free(tmp);
        }

    }

//     errprintf("sub_dir) query = `%s'\n", query);
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
