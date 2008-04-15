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

#include "db_fuse.h"
#include "db_fuse_utils.h"   /* subdirs(), exist(), get_db_from_context(), get_fissi_from_context(), get_keywords_from_context() */
#include "parser.h"          /* parse_path(), IS_A_DIR, IS_A_FILE */

/**
    Implementazione di fuse_operations.readdir()
*/
int db_fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi) {
    errprintf("[READDIR] path = `%s'\n", path);
    (void) fi;      // uso path
    (void) offset;  // lettura tutta d'una volta
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    sqlite3* db = get_db_from_context();
    dynamic_obj_t fissi = get_fissi_from_context();
    dynamic_obj_t keywords = get_keywords_from_context();
    dynamic_obj_t dinamici;
    init_obj(&dinamici);
    if (parse_path(path, fissi, keywords, &dinamici) == -1)
        return -1;
    if (!exist(db, fissi, keywords, dinamici, path))
        return -1;
    dynamic_str_t elementi = sub_dir(db, fissi, keywords, dinamici);
    for (int i=0; i<elementi.size; i++)
        filler(buf, elementi.buf[i], NULL, 0);
    free_obj(&dinamici);
    return 0;
}

/**
    Implementazione di fuse_operations.getattr()
    regola: se path != "/" e non è elencato dalla directory superiore di path,
    ritorna l'errore appropriato
*/
int db_fuse_getattr(const char* path, struct stat* filestat) {
    errprintf("[GETATTR] path = `%s'\n", path);
    if (!strcmp(path, "/")) {   // root esiste :)
        memset(filestat, 0, sizeof(struct stat));
        filestat->st_mode = S_IFDIR | 0755;
        filestat->st_nlink = 2;
        return 0;
    }
    sqlite3* db = get_db_from_context();
    dynamic_obj_t fissi = get_fissi_from_context();
    dynamic_obj_t keywords = get_keywords_from_context();
    dynamic_obj_t dinamici;
    init_obj(&dinamici);
    int type = parse_path(path, fissi, keywords, &dinamici);
    if (type == -1)
        return -ENOENT;
    if (!exist(db, fissi, keywords, dinamici, path))
        return -ENOENT;
    if (type == IS_A_DIR) {
        filestat->st_mode = S_IFDIR | 0755;
        filestat->st_nlink = 2;
        return 0;
    }
    else
        if (is_local_file(db, fissi, keywords, dinamici)) {  // file locale
            if (stat(get_local_paths(db, fissi, keywords, dinamici).buf[0],
                    filestat))
                return -errno;
            return 0;
        }
        else // TODO: file remoto
            return -EACCES;
    return -ENOENT;
}

#include "constants.h"           /* column_from_keyword() */
#include "../common/sqlite.h"    /* esegui_query() */
/**
    Implementazione di fuse_operations.rename()
    UPDATE
        musica
    SET
        lavoro_anno = '1971', nome_album = 'Fætus'
    WHERE
        lavoro_anno = '1971' AND
        nome_album = 'Fetus' AND
        artista_nome_artista in (
            select artista_nome_artista
            from musica,artista
            where
                nome_artista = 'Franco Battiato' and
                artista_nome_artista = nome_artista
        )
    ;
*/
int db_fuse_rename(const char* from, const char* to) {
    errprintf("[RENAME] from = `%s', to = `%s'\n", from, to);
    sqlite3* db = get_db_from_context();
    dynamic_obj_t fissi = get_fissi_from_context();
    dynamic_obj_t keywords = get_keywords_from_context();
    dynamic_obj_t dinamici_from, dinamici_to;
    init_obj(&dinamici_from);
    init_obj(&dinamici_to);
    if (parse_path(from, fissi, keywords, &dinamici_from) == -1)
        return -ENOENT;
    if (!exist(db, fissi, keywords, dinamici_from, from))
        return -ENOENT;
    if (parse_path(to, fissi, keywords, &dinamici_to) == -1)
        return -EROFS;
    if (exist(db, fissi, keywords, dinamici_to, to))
        return -EEXIST;

    /* ATTENZIONE io permetto lo spostamento solo "allo stesso livello": non
            voglio poter rinominare un cantante in un anno! */
    if (dinamici_from.size != dinamici_to.size)
        return -EROFS;

    if (is_local_file(db, fissi, keywords, dinamici_from))
        rename_local_file(db, fissi, keywords, dinamici_from, dinamici_to);
//     else
        // TODO: file remoti
/*
    dynamic_str_t tabelle, where;
    init_str(&tabelle);
    if (calcola_tabelle(*(dynamic_str_t*)keywords.buf[dinamici_to.size-1],
            &tabelle) == -1)
        return -EIO;
    if (!tabelle.size)
        return -EIO;
    calcola_where(tabelle, &where);

    char* query = strmalloccat(calloc(1, 1), "BEGIN TRANSACTION\n");
    for (int ii=0; ii<tabelle.size; ii++) {
        query = strmalloccat(strmalloccat(strmalloccat(query, "UPDATE "),
                tabelle.buf[ii]), " SET\n\t");
        for (int i=0; i<dinamici_to.size; i++) {
            dynamic_str_t tabella_colonna = split(column_from_keyword(
                    ((dynamic_str_t*)keywords.buf[dinamici_to.size-1])->buf[i]),
                    '.');
            if (contains_str(tabelle, tabella_colonna.buf[0])) {
                if (i)
                    query = strmalloccat(query, ",\n\t");
                char* tmp = sqlite3_mprintf(" = %Q", ((dynamic_str_t*)
                        dinamici_to.buf[dinamici_to.size-1])->buf[i]);
                query = strmalloccat(strmalloccat(query,
                        tabella_colonna.buf[1]), tmp);
                free(tmp);
            }
        }
        query = strmalloccat(query, "\nWHERE\n\t");
        for (int i=0; i<dinamici_from.size; i++)
            for (int j=0; j<((dynamic_str_t*)dinamici_from.buf[i])->size; j++) {
                if (i || j)
                    query = strmalloccat(query, " AND\n\t");
                dynamic_str_t tabella_colonna = split(column_from_keyword(
                        ((dynamic_str_t*)keywords.buf[i])->buf[j]), '.');
//                 dbgprint_str(tabella_colonna, "keywords");
                if (contains_str(tabelle, tabella_colonna.buf[0])) {
                    char* tmp = sqlite3_mprintf(" = %Q", ((dynamic_str_t*)
                            dinamici_from.buf[i])->buf[j]);
                    query = strmalloccat(strmalloccat(query,
                                        tabella_colonna.buf[1]), tmp);
                    free(tmp);
                }
                else {
                    // trova percorso da tabella_colonna a tabelle.buf[ii]
                    // es: da artista.nome_artista a musica.artista_nome_artista
                    // artista_nome_artista in (
                    //  SELECT artista_nome_artista
                    //  FROM artista,musica
                    //  WHERE
                    //    nome_artista = '...'
                    //    artista_nome_artista = artista.nome_artista
                    // )
                }
            }
        query = strmalloccat(query, "\n;");

    }
    query = strmalloccat(query, "\nEND");

*/

/*    char* query = strmalloccat(calloc(1, 1), "UPDATE\n\t");

    for (int i=0; i<tabelle.size; i++) {
        if (i)
            query = strmalloccat(query, ", ");
        query = strmalloccat(query, tabelle.buf[i]);
    }
    query = strmalloccat(query, "\nSET (\n\t");

    // lavoro sull'ultimo vettore di elementi di dinamici_to
    dynamic_str_t keywords_ = *(dynamic_str_t*)keywords.buf[dinamici_to.size-1];
    dynamic_str_t to_ = *(dynamic_str_t*)dinamici_to.buf[dinamici_to.size-1];
    // assumo che keywords_.size == to_.size == > 0

    // sto cercando di rinominare una cartella fatta solo da elementi fissi O_o
    if (!keywords_.size)
        return -EROFS;

    for (int i=0; i<keywords_.size; i++) {
        if (i)
            query = strmalloccat(query, ",\n\t");
        query = strmalloccat(query, column_from_keyword(keywords_.buf[i]));
        char* tmp = sqlite3_mprintf(" = %Q", to_.buf[i]);
        query = strmalloccat(query, tmp);
        free(tmp);
    }
    // il WHERE c'è di sicuro
    query = strmalloccat(query, "\n) WHERE\n");

    for (int i=0; i<dinamici_from.size; i++) {
        keywords_ = *(dynamic_str_t*)keywords.buf[i];
        dynamic_str_t from_ = *(dynamic_str_t*)dinamici_from.buf[i];
        // assumo che keywords.size == from_.size
//         for (int j=0; j<keywords_.size; j++) {
//             if (j)
//                 query = strmalloccat(query, " AND\n\t");
//             query = strmalloccat(strmalloccat(strmalloccat(query,
//                     column_from_keyword(keywords_.buf[j])), " GLOB "),
//                     from_.buf[j]);

        for (int j=0; j<keywords_.size; j++) {
            if (i)
                query = strmalloccat(query, " AND\n");
//             if (fissi2.size > j+1)          // se è seguito da un elemento fisso
//                 query = strmalloccat(query, "\t(REPLACE(");
//             else
                query = strmalloccat(query, "\t(");
            query = strmalloccat(strmalloccat(strmalloccat(query, "REPLACE("),
                    column_from_keyword(keywords_.buf[j])), ", '/', '_')");
//             if (fissi2.size > j+1) {        // se è seguito da un elemento fisso
//                 char* tmp = sqlite3_mprintf(", %Q, '_')", fissi2.buf[j+1]);
//                 query = strmalloccat(query, tmp);
//                 sqlite3_free(tmp);
//             }
            char* tmp = sqlite3_mprintf(" GLOB %Q)", from_.buf[j]);
            query = strmalloccat(query, tmp);
            free(tmp);
        }

    }
*/

//     calcola_where(tabelle, &where);
//     for (int i=0; i<where.size; i++)
//         query = strmalloccat(strmalloccat(query, " AND\n\t"), where.buf[i]);
//     errprintf("rename->query = `%s'\n", query);

//     int ret = esegui_query(db, query);
//     free(query);
//     return ret;
    return 0;
}

/**
    Implementazione di fuse_operations.chmod()
*/
int db_fuse_chmod(const char* path, mode_t mode) {
    errprintf("[CHMOD] path = `%s'\n", path);
    sqlite3* db = get_db_from_context();
    dynamic_obj_t fissi = get_fissi_from_context();
    dynamic_obj_t keywords = get_keywords_from_context();
    dynamic_obj_t dinamici;
    init_obj(&dinamici);
    int type = parse_path(path, fissi, keywords, &dinamici);
    if (type != IS_A_FILE)  // NON posso modificare i diritti su una directory
        return -EROFS;  // ho un "quasi-ReadOnly FS"
    if (!exist(db, fissi, keywords, dinamici, path))
        return -ENOENT; // 'sto file non esiste
    else
        if (is_local_file(db, fissi, keywords, dinamici)) {
            if (chmod(get_local_paths(db, fissi, keywords, dinamici).buf[0], mode))
                return -errno;
            return 0;
        }
        else // TODO: file remoto
            return -EROFS;  // per il momento non posso farlo
    return -ENOENT;
}

/**
    Implementazione di fuse_operations.open()
    \param virtual_path percorso nel file system virtuale di cui è stato
            richiesto il contenuto
    \param ffi informazioni aggiuntive di fuse (in particolare, ffi->fh indica
            il file descriptor del file REALE da aprire)
    \return il valore di ritorno di open(path_REALE, O_RDONLY);
    \sa fuse.h, musicmeshfs_open(), close()
*/
int db_fuse_open(const char* virtual_path, struct fuse_file_info* ffi) {
    errprintf("[OPEN] virtual_path = `%s'\t", virtual_path);
    sqlite3* db = get_db_from_context();
    dynamic_obj_t fissi = get_fissi_from_context();
    dynamic_obj_t keywords = get_keywords_from_context();
    dynamic_obj_t dinamici;
    init_obj(&dinamici);
    if (parse_path(virtual_path, fissi, keywords, &dinamici) != IS_A_FILE)
        return -1;
    if (!exist(db, fissi, keywords, dinamici, virtual_path))
        return -1;
    if (is_local_file(db, fissi, keywords, dinamici)) {   // file locale
        char* real_path = get_local_paths(db, fissi, keywords, dinamici).buf[0];
        if (!real_path)
            return -1;
        int i = open(real_path, O_RDONLY);
        if (i == -1)
            return -errno;
        ffi->fh = i;
        return 0;
    }
    return -1;
}

/**
    Implementazione di fuse_operations.release()
    \param virtual_path percorso nel file system virtuale di cui è stato
            richiesto il contenuto
    \param ffi informazioni aggiuntive di fuse (in particolare, ffi->fh indica
            il file descriptor del file REALE da chiudere)
    \return il valore di ritorno di close(ffi->fh);
    \sa fuse.h, musicmeshfs_open(), close()
*/
int db_fuse_release(const char* virtual_path, struct fuse_file_info* ffi) {
    errprintf("[RELEASE] path = `%s'\n", virtual_path);
    return close(ffi->fh);   // nella doc dice che non è usata
}

/**
    Implementazione di fuse_operations.read()
    \param virtual_path percorso nel file system virtuale di cui è stato
            richiesto il contenuto
    \param buffer vettore su cui scrivere le informazioni richieste
    \param size dimensione del buffer
    \param offset valore di "spiazzamento" da usare per la scrittura su buffer
    \param ffi informazioni aggiuntive di fuse (in particolare, ffi->fh indica
            il file descriptor del file REALE da leggere)
    \return il valore di ritorno di pread(ffi->fh, buffer, size, offset);
    \sa fuse.h, musicmeshfs_open(), pread()
*/
int db_fuse_read(const char* virtual_path, char* buffer, size_t size,
            off_t offset, struct fuse_file_info* ffi) {
    errprintf("[READ] path = `%s'\ti = %d\n", virtual_path, ffi->fh);
    int res = pread(ffi->fh, buffer, size, offset);
    if (res == -1)
        res = -errno;
    return res;
}
