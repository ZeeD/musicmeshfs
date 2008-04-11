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
#include "db_fuse_utils.h"   /* subdirs(), exist(), get_db_from_context(),
        get_fissi_from_context(), get_keywords_from_context() */
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
    ritorna -ENOENT
    \todo: stub, farla davvero!!!
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
    else {
        if (is_local_file(db, fissi, keywords, dinamici))   // file locale
            return -stat(get_local_path(db, fissi, keywords, dinamici),
                    filestat);
        // TODO: file remoto
    }
    return -ENOENT;
}

/**
    Implementazione di fuse_operations.rename()
    \todo stub!
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
        return -1;
    if (!exist(db, fissi, keywords, dinamici_from, from))
        return -1;
    if (parse_path(to, fissi, keywords, &dinamici_to) == -1)
        return -1;
    if (exist(db, fissi, keywords, dinamici_to, to))
        return -1;
/*    dynamic_str_t fissi, dinamici, splitted_to = split(to+1, '/');
    for (int i=0; i<splitted_to.size; i++) {
        calcola_elementi(splitted_to.buf[i], &fissi, &dinamici);
        dbgprint_str(fissi, "RENAME->fissi");
        dbgprint_str(dinamici, "RENAME->dinamici");
    }*/
    return -1;
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
        if (is_local_file(db, fissi, keywords, dinamici))   // file locale
//             int chmod(const char *path, mode_t mode);
            return -chmod(get_local_path(db, fissi, keywords, dinamici), mode);
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
        char* real_path = get_local_path(db, fissi, keywords, dinamici);
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
