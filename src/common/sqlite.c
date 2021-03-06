/*
    Copyright © 2008 Vito De Tullio

    This file is part of MusicMeshFS[cd].

    MusicMeshFS[cd] is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MusicMeshFS[cd] is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MusicMeshFS[cd].  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sqlite.h"
#include "utils.h"


/**
    Assume che buf sia un dynamic_obj_t
    Riempie le coppie di buf con le tuple trovate
 */
int get_columns(void* buf, int n_colonne, char** value, char** header) {
    (void) header; // non usato
    dynamic_str_t* row = malloc(sizeof(dynamic_str_t));
    init_str(row);
    for (int i=0; i<n_colonne; i++)
        append_str(row, value[i]);
    append_obj((dynamic_obj_t*)buf, row);
    return 0;
}


/**
    ha in input un path, e (se non esiste) crea un "database per sqlite3" vuoto.

    \param db_file path dove creare / inizializzare il database
    \return database sqlite3
*/
sqlite3* crea_db_vuoto(char* db_file) {
    char schema[] = {
        #include "schema.h"
    };
    sqlite3* db;
    if (sqlite3_open(db_file, &db) != SQLITE_OK) {
        warn("sqlite3_open: `%s'\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    else
        if (esegui_query(db, schema) != SQLITE_OK) {
            warn("esegui_query: `%s'\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }
    return db;
}

/**
    Verifica che un certo file corrisponda ad un database effettivo
    \param path percorso del file da verificare
    \return 1 se il percorso corrisponde ad un database,
            0 altrimenti
    \note *al* *momento* NON verifica che il database uno di quelli creati
            dall'applicazione stessa!
*/
int path_is_a_db(const char* path) {
    struct stat t;
    if (stat(path, &t) == -1)
        return 0;   // path non valido
    if (!S_ISREG(t.st_mode))
        return 0;   // non è un file regolare
    sqlite3* db;
    int ret = sqlite3_open(path, &db);
    if (ret != SQLITE_OK)
        return 0;   // non è un database
    return sqlite3_close(db) != SQLITE_BUSY;
}

/**
    funzione di callback applicata da esegui_query()

    \param non_usato parametro non usato (presente per compatibilità con
            sqlite3_exec())
    \param argc numero di colonne di output
    \param argv risultati della query
    \param nome_colonne headers dei risultati
    \return 0   (non è previsto che abbia problemi)
    \sa esegui_query()
*/
int callback_print(void* non_usato, int argc, char** argv, char**
        nome_colonne) {
    non_usato = non_usato;  // messo per evitare il warning del compilatore
    for (int i=0; i<argc; i++)
        printf("%30s = %30s\n", nome_colonne[i], argv[i]);
    return 0;
}

/**
    wrapper di comodo attorno sqlite3_exec()

    esegue, in modo verboso (stampa a video i risultati della query), una query
    data su un database
    \param db database su cui effetturare la query
    \param query comandi SQL da effetturare (sintassi printf-like + "%q" e "%Q")
    \return il valore di sqlite3_exec()
    \sa sqlite3_exec(), callback_print()
*/
int esegui_query(sqlite3* db, const char* query, ...) {
    va_list ap;
    va_start(ap, query);
    char* quoted_query = sqlite3_vmprintf(query, ap);
    va_end(ap);
    char* errmsg = NULL;
    int r = sqlite3_exec(db, quoted_query, callback_print, NULL, &errmsg);
    sqlite3_free(quoted_query);
    if (errmsg) {
        warn("esegui_query: `%s'\n", errmsg);
        sqlite3_free(errmsg);
    }
    return r;
}

int esegui_query_callback(sqlite3* db, int(*callback)(void*, int, char**,
            char**), void* callback_par, const char* query, ...) {
    va_list ap;
    va_start(ap, query);     /* inizializzo ap col primo valore successivo a
                                query, se ce ne sono */
    char* quoted_query = sqlite3_vmprintf(query, ap);
    va_end(ap); // chiudo ap (...)
    char* errmsg = NULL;
    int r = sqlite3_exec(db, quoted_query, callback, callback_par, &errmsg);
    sqlite3_free(quoted_query);
    if (errmsg) {
        warn("esegui_query_callback: `%s'\n", errmsg);
        sqlite3_free(errmsg);
    }
    return r;
}


/**
    funzione di callback applicata da init_db() (usabile anche esternamente,
    ovvio, ma aggiunge al db solo file locali!)

    prende un percorso (si assume che punti ad un file valido). Se questo è un
    file supportato da taglib, ne analizza i tag e inserisce i dati nel database
    \attention le query *non* sono generiche, nè usano il file constants.h!
            inoltre, poiché questi dati saranno usati per generare i percorsi
            virtuali, non sono ammessi metadati col carattere '/': in tal caso
            saranno sostituiti col carattere '_'

    \param db database (di tipo sqlite*, ma castato a void* per questioni di
            compatibilità)
    \param path percorso del file da analizzare e, se supportato, aggiungere al
            database
    \return 0 se non ci sono errori, -1 altrimenti
    \sa init_db(), esegui_query(), preprocessing()
*/
int add_local_file_in_db(void* db, const char* path) {
    warn("[add_local_file_in_db] path = `%s'\n", path);
    TagLib_File* tlf = taglib_file_new(path);
    if (!tlf)
        return -1;
    TagLib_Tag* tlt = taglib_file_tag(tlf); // automaticamente free`d
    char* titolo = preprocessing(taglib_tag_title(tlt));
    char* artista = preprocessing(taglib_tag_artist(tlt));
    char* album = preprocessing(taglib_tag_album(tlt));
    char* genere = preprocessing(taglib_tag_genre(tlt));
    unsigned int anno = taglib_tag_year(tlt);
    unsigned int traccia = taglib_tag_track(tlt);

    char* anno_s = calloc(5, 1);    // andrà bene fino all'anno 9999
    char* traccia_s = calloc(3, 1); // andrà bene fino alla traccia 99

    snprintf(anno_s, 5, "%04d", anno);
    snprintf(traccia_s, 3, "%02d", traccia);

    if (esegui_query(db, "INSERT OR REPLACE INTO artista(nome_artista) values "
            "(%Q)", artista) == -1)
        return -1;
    if (esegui_query(db, "INSERT OR REPLACE INTO musica(titolo, nome_album,"
            " traccia, genere, artista_nome_artista, lavoro_anno) values "
            "(%Q, %Q, %Q, %Q, %Q, %Q)", titolo, album, traccia_s, genere,
            artista, anno_s) == -1)
        return -1;
    if (esegui_query(db, "INSERT OR REPLACE INTO file(host, path, permessi,"
            " formato, dimensioni, data_ultimo_aggiornamento, musica_titolo"
            ", musica_nome_album, musica_traccia, basename) values ("
            "'127.0.0.1', %Q, %d, %Q, %d, NULL, %Q, %Q, %Q, %Q)", path, 6,
            extract_extension(path), extract_size(path), titolo, album,
            traccia_s, extract_basename(path)) == -1)
        return -1;
    taglib_tag_free_strings();
    taglib_file_free(tlf);
    return 0;
}

/**
    rimuove dal database le informazioni sul file
    \param db database da modificare (sqlite3*, ma castato a void* per
            compatibilità)
    \param path percorso del file da rimuovere
    \return 0 se non ci sono errori, -1 altrimenti
    \sa add_local_file_in_db()
*/
int del_local_file_from_db(void* db, char* path) {
    warn("[del_local_file_from_db] path = `%s'\n", path);
    dynamic_obj_t musica;
    init_obj(&musica);
    if (esegui_query_callback(db, get_columns, &musica, "SELECT titolo,"
            "nome_album,traccia FROM musica,file WHERE musica_titolo=titolo AND"
            " musica_nome_album=nome_album AND musica_traccia=traccia AND host="
            "'127.0.0.1' AND path = %Q", path) == -1)
        return -1;
    for (int i=0; i<musica.size; i++) {
        char* titolo = ((dynamic_str_t*)musica.buf[i])->buf[0];
        char* nome_album = ((dynamic_str_t*)musica.buf[i])->buf[1];
        char* traccia = ((dynamic_str_t*)musica.buf[i])->buf[2];

        dynamic_obj_t artista;
        init_obj(&artista);

        if (esegui_query_callback(db, get_columns, &artista, "SELECT "
                "nome_artista FROM artista,musica WHERE titolo=%Q AND "
                "nome_album=%Q AND traccia=%Q AND musica.artista_nome_artista="
                "nome_artista", titolo, nome_album, traccia) == -1)
            return -1;
        for (int j=0; j<artista.size; j++) {
            char* nome_artista = ((dynamic_str_t*)artista.buf[i])->buf[0];
            if (esegui_query(db, "DELETE FROM artista WHERE nome_artista = %Q",
                    nome_artista) == -1)
                return -1;
        }
        if (esegui_query(db, "DELETE FROM musica WHERE titolo = %Q AND nome_alb"
                "um = %Q AND traccia = %Q", titolo, nome_album, traccia) == -1)
            return -1;
    }
    if (esegui_query(db, "DELETE FROM file WHERE host='127.0.0.1' AND path=%Q",
            path) == -1)
        return -1;
    return 0;
}

#ifdef __DEBUG_SQLITE_C__
int main(int argc, char* argv[]) {
#else
void __DEBUG_SQLITE_C__main(int argc, char* argv[]);
void __DEBUG_SQLITE_C__main(int argc, char* argv[]) {
#endif
    if (argc>1)
        crea_db_vuoto(argv[1]);
}
