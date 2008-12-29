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
#include "../common/utils.h"    /* dynamic_str_t, strmalloccat(), init_str(), preprocessing() */
#include "../common/sqlite.h"   /* esegui_query_callback() */
#include "../db_fuse/db_fuse_utils.h"   /* get_one_column() */
#include "../db_fuse/constants.h"   /* column_from_keyword() */

/**
    ritorna la dimensione (in byte) del file originario
    assumo che chiavi.size == valori.size
*/
int get_size(sqlite3* db,  dynamic_obj_t fissi, dynamic_obj_t keywords,
            dynamic_obj_t dinamici) {
    char query[] = "SELECT file.dimensioni FROM file WHERE "
            "(file.host = '127.0.0.1') and (file.path = %Q)";
    dynamic_str_t ret;
    init_str(&ret);
    char* path = get_local_paths(db, fissi, keywords, dinamici).buf[0];
    esegui_query_callback(db, get_one_column, &ret, query, path);
    free(path);
    free(query);
    return atoi(ret.buf[0]);
}

/**
    Calcola il percorso locale del primo file reale che supera i vincoli
    impostati dalle coppie chiave/valore

    Assumo che dinamici.size <= keywords.size, non posso assicurare il
    contrario
    \param db database
    \param keywords "chiavi" del dizionario chiave/valore
    \param dinamici "valori" del dizionario chiave/valore
    \return path locale del file corrispondente,
            NULL in caso non ne esista uno
*/
dynamic_str_t get_local_paths(sqlite3* db, dynamic_obj_t fissi, dynamic_obj_t
        keywords, dynamic_obj_t dinamici) {
    dynamic_str_t ret;
    init_str(&ret);
    dynamic_str_t tabelle, where;
    init_str(&tabelle);
    append_str(&tabelle, "file");
    for (int i=0; i<keywords.size; i++)
        if (calcola_tabelle(*(dynamic_str_t*)keywords.buf[i], &tabelle) == -1)
            return ret;

    char* query = strmalloccat(calloc(1,1),
            "SELECT\n\tDISTINCT file.path\nFROM\n\t");
    for (int i=0; i<tabelle.size; i++) {
        if (i)
            query = strmalloccat(query, ", ");
        query = strmalloccat(query, tabelle.buf[i]);
    }
    query = strmalloccat(query, "\nWHERE (1=1)\n");
    // assumo che fissi.size == keywords.size >= dinamici.size

    for (int i=0; i<dinamici.size; i++) {
        dynamic_str_t fissi2 = *(dynamic_str_t*)(fissi.buf[i]);
        dynamic_str_t chiavi = *(dynamic_str_t*)(keywords.buf[i]);
        dynamic_str_t valori = *(dynamic_str_t*)(dinamici.buf[i]);

        //assumo che nella directory chiavi.size == valori.size
        for (int j=0; j<chiavi.size; j++) {
//             if (i)
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
            sqlite3_free(tmp);
        }
    }

    calcola_where(tabelle, &where);
    for (int i=0; i<where.size; i++)
        query = strmalloccat(strmalloccat(query, " AND\n\t"), where.buf[i]);

    esegui_query_callback(db, get_one_column, &ret, query);
    free(query);
    return ret;
}

/**
    Dati il db, e il vettore di dizionari < keywords, dinamici > valuta se
    corrisponde ad un file locale
    \param db datase su cui fare le query
    \param fissi elementi fissi
    \param keywords chiavi
    \param dinamici valori delle chiavi
    \return 1 se il file è locale,
            0 altrimenti
    \attention al momento è fittizio! (ritorna sempre 1s)
 */
int is_local_file(sqlite3* db, dynamic_obj_t fissi, dynamic_obj_t keywords,
        dynamic_obj_t dinamici) {
    (void) db;
    (void) fissi;
    (void) keywords;
    (void) dinamici;
    return 1;
}

int rename_local_file(void* db, dynamic_obj_t fissi, dynamic_obj_t keywords,
        dynamic_obj_t values_from, dynamic_obj_t values_to) {
    dynamic_str_t paths = get_local_paths((sqlite3*)db, fissi, keywords, values_from);
    for (int ipath=0; ipath<paths.size; ipath++) {
        char* path = paths.buf[ipath];
        TagLib_File* tlf = taglib_file_new(path);
        if (!tlf)
            return -1;
        TagLib_Tag* tlt = taglib_file_tag(tlf);
        for (int i=0; i<values_to.size; i++)
            for (int j=0; j<((dynamic_str_t*)values_to.buf[i])->size; j++) {
                char* keyword = ((dynamic_str_t*)keywords.buf[i])->buf[j];
                char* value = ((dynamic_str_t*)values_to.buf[i])->buf[j];
                if (!strcmp(keyword, "artist"))
                    taglib_tag_set_artist(tlt, value);
                else if (!strcmp(keyword, "year"))
                    taglib_tag_set_year(tlt, atoi(value));
                else if (!strcmp(keyword, "album"))
                    taglib_tag_set_album(tlt, value);
                else if (!strcmp(keyword, "track"))
                    taglib_tag_set_track(tlt, atoi(value));
                else if (!strcmp(keyword, "title"))
                    taglib_tag_set_title(tlt, value);
                else if (!strcmp(keyword, "genre"))
                    taglib_tag_set_genre(tlt, value);
                else
                    warn("keyword non supportata `%s' (value = `%s')\n",
                            keyword, value);
            }
        taglib_file_save(tlf);
        taglib_file_free(tlf);
        taglib_tag_free_strings();
    }
    return 0;
}

