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

#include "db_fuse_variant.h"
#include "../common/utils.h"             /* dynamic_str_t, strmalloccat(),
                                            init_str(), preprocessing() */
#include "../common/sqlite.h"            /* esegui_query_callback() */
#include "../db_fuse/db_fuse_utils.h"    /* get_one_column() */

/**
    ritorna la dimensione (in byte) del file originario
    assumo che chiavi.size == valori.size
*/
int get_size(sqlite3* db, dynamic_obj_t keywords, dynamic_obj_t dinamici) {
    char* path = get_local_path(db, keywords, dinamici);
    char query[] = "SELECT dimensioni FROM file WHERE (host='127.0.0.1') and "
            "(path=%Q)";
    dynamic_str_t ret;
    init_str(&ret);
    esegui_query_callback(db, get_one_column, &ret, query, path);
    return atoi(ret.buf[0]);
}

// /**
//     Funzione booleana che decide se il path richiesto è una directory
//
//     Il motto è "When there's only one candidate, there's only one choice!" [MI1]
//     \param db database su cui effettuare eventuali query
//     \param chiavi vettore dinamico delle chiavi presenti
//     \param valori vettore dinamico dei valori corrispondenti
//     \return 1 se è una dir, 0 altrimenti
//     \attention host e path corrispondono alla chiave primaria di file, che
//             indica dove andare a prendere la musica!
//  */
// int is_a_dir(sqlite3* db, dynamic_str_t chiavi, dynamic_str_t valori) {
//     if (chiavi.size == valori.size + 1)
//         return 1;
//     dynamic_str_t copiaChiavi = slice_str(chiavi, 0, chiavi.size);
//     dynamic_str_t copiaValori = slice_str(valori, 0, valori.size);
//     if (!contains_str(copiaChiavi, "host")) {
//         append_str(&copiaChiavi, "host");
//         append_str(&copiaValori, "*");
//     }
//     if (!contains_str(copiaChiavi, "path")) {
//         append_str(&copiaChiavi, "path");
//         append_str(&copiaValori, "*");
//     }
//     append_str(&copiaChiavi, "COUNT()");
//     char * query = calcola_query_subdirs(copiaChiavi, copiaValori);
//     dynamic_str_t ret;
//     init_str(&ret);
//     esegui_query_callback(db, get_one_column, &ret, query);
//     free(query);
//     free_str(&copiaChiavi);
//     free_str(&copiaValori);
//     int r = atoi(ret.buf[0]);
//     free_str(&ret);
//     return r > 1;
// }

/**
    Calcola il percorso locale del primo file reale che supera i vincoli
    impostati dalle coppie chiave/valore,
    \param chiavi schema del filesystem con cui vengono mostrati i dati
    \param valori path richiesto dall'utente nel filesystem virtuale
    \return path locale del file corrispondente, NULL in caso non ne esista uno
    \sa virtual_path_to_host()
    \attention utilizza COLUMNS_MISC definita in constants.c
*/
char* get_local_path(sqlite3* db, dynamic_obj_t keywords, dynamic_obj_t
        dinamici) {
/*    // non modifico direttamente chiavi
    dynamic_str_t copiaChiavi = slice_str(chiavi, 0, chiavi.size);
    append_str(&copiaChiavi, "path");
    dynamic_str_t risultati;
    init_str(&risultati);
    char* query = calcola_query_subdirs(copiaChiavi, valori);
//     errprintf("get_real_path->query = `%s'\n", query);
    esegui_query_callback(db, get_one_column, &risultati, query);
    free(query);
    free_str(&copiaChiavi);
//     dbgprint_str(risultati, "get_real_path->risultati");
    if (risultati.size) {
        char* ret = strdup(risultati.buf[0]);
        free_str(&risultati);
//         errprintf("get_real_path->ret = `%s'\n", ret);
        return ret;
    }*/
    return NULL;
}
/*    char* query = malloc(1); query[0] = '\0';
    query = strmalloccat(query, "SELECT path FROM ");  // TODO: cacciare fuori il literal "path" dalla query!
    query = strmalloccat(query, TABLE);
    query = strmalloccat(query, " WHERE ");
    dynamic_str_t where;
    calcola_where(schema, virtual_path, &where);
    int indice_where = 0;
    dynamic_str_t tmp_fissi, tmp_dinamici;
    if (virtual_path.size) {
    calcola_elementi(schema.buf[0], &tmp_fissi, &tmp_dinamici);
    free_str(&tmp_fissi);
    if (tmp_dinamici.size) {
    query = strmalloccat(query, "(");
    query = strmalloccat(query, tmp_dinamici.buf[0]);
    char* quoted = sqlite3_mprintf(" = %Q)", where.buf[indice_where++]);
    query = strmalloccat(query, quoted);
    sqlite3_free(quoted);
}
    for (int j=1; j<tmp_dinamici.size; j++) {
    query = strmalloccat(query, "and (");
    query = strmalloccat(query, tmp_dinamici.buf[j]);
    char* quoted = sqlite3_mprintf(" = %Q)", where.buf[indice_where++]);
    query = strmalloccat(query, quoted);
    sqlite3_free(quoted);
}
    free_str(&tmp_dinamici);
}
    for (int i=1; i<virtual_path.size; i++) {
    calcola_elementi(schema.buf[i], &tmp_fissi, &tmp_dinamici);
    free_str(&tmp_fissi);
    for (int j=0; j<tmp_dinamici.size; j++) {
    query = strmalloccat(query, "and (");
    query = strmalloccat(query, tmp_dinamici.buf[j]);
    char* quoted = sqlite3_mprintf(" = %Q)", where.buf[indice_where++]);
    query = strmalloccat(query, quoted);
    sqlite3_free(quoted);
}
    free_str(&tmp_dinamici);
}
    dynamic_str_t ret;
    init_str(&ret);
    esegui_query_callback(DB, piglia_stringa, &ret, query);
    sqlite3_free(query);
    if (ret.size)
    return ret.buf[0];*/

/**
    Dati il db, e il vettore di dizionari < keywords, dinamici > valuta se
    corrisponde ad un file locale
    \param db datase su cui fare le query
    \param keywords chiavi
    \param dinamici valori delle chiavi
    \return 1 se il file è locale,
            0 altrimenti
    \attention al momento è fittizio! (ritorna sempre 1s)
 */
int is_local_file(sqlite3* db, dynamic_obj_t keywords, dynamic_obj_t dinamici) {
    (void) db;
    (void) keywords;
    (void) dinamici;
    return 1;
}
