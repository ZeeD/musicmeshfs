#include "db_fuse.h"
#include "db_fuse_utils.h"   /* subdirs(), exist(), get_db_from_context(),
                                get_fissi_from_context(),
                                get_keywords_from_context() */
// #include "utils.h"           /* errprintf(), dynamic_obj_t */
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
    if (!exist(db, keywords, dinamici))
        return -1;
    dynamic_str_t elementi = sub_dir(db, keywords, dinamici);
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
        return -1;
    if (!exist(db, keywords, dinamici))
        return -1;
    if (type == IS_A_DIR) {
        filestat->st_mode = S_IFDIR | 0755;
        filestat->st_nlink = 2;
        return 0;
    }
    else {
        if (is_local_file(db, keywords, dinamici)) {   // file locale
            char* real_path = get_local_path(db, keywords, dinamici);
            errprintf("getattr -> real_path = `%s'\n", real_path);
            if (real_path)
                return stat(real_path, filestat);
            else
                return -1;
        }
        // TODO: file remoto
    }
    return -1;
}

    /*
    dynamic_str_t splitted_path = split(path+1, '/');
    dynamic_str_t sliced_path = slice_str(splitted_path, 0, splitted_path.size-1);
    char* last_element = splitted_path.buf[splitted_path.size-1];

    // devo sapere se l'ultimo elemento di path esiste,
    //      altrimenti ritornare -ENOENT
    // come? controllo se l'"ls" di tutto prima dell'ultimo elemento di path
    //                                        contiene l'ultimo elemento di path

    dynamic_str_t dirs = schema_to_dirs(PATTERN_DIR_NAME, sliced_path);
//     dbgprint_str(dirs, "getattr.dirs");
//     dbgprint_str(splitted_path, "splitted_path");
//     dbgprint_str(sliced_path, "sliced_path");
//     fprintf(stderr, "last_element = `%s'\n", last_element);
    if (!contains_str(dirs, last_element)) { // file non esistente
        free_str(&sliced_path);
        free_str(&splitted_path);
        free_str(&dirs);
        return -ENOENT;
    }
    // file esistente, devo solo decidere se è un file o una directory :)
    memset(filestat, 0, sizeof(struct stat));
    if (is_a_dir(PATTERN_DIR_NAME, splitted_path)) {
        filestat->st_mode = S_IFDIR | 0755;
        filestat->st_nlink = 2;
    }
    else{
        filestat->st_mode = S_IFREG | 0444; // TODO: permessi
        filestat->st_nlink = 1;
        filestat->st_size = get_size(PATTERN_DIR_NAME, splitted_path);
//         fprintf(stderr,
//                 "[DBG]real_path = `%s', virtual_path = `%s'\n",
//                 virtual_path_to_real_path(PATTERN_DIR_NAME, splitted_path),
//                 path);
//         virtual_path_to_real_path(PATTERN_DIR_NAME, splitted_path);
    }
    free_str(&splitted_path);*/

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
    if (!exist(db, keywords, dinamici_from))
        return -1;
    if (parse_path(to, fissi, keywords, &dinamici_to) == -1)
        return -1;
    if (exist(db, keywords, dinamici_to))
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
    \todo stub!
*/
int db_fuse_chmod(const char* path, mode_t mode) {
    errprintf("[CHMOD] path = `%s'\n", path);
    (void) mode;
    sqlite3* db = get_db_from_context();
    dynamic_obj_t fissi = get_fissi_from_context();
    dynamic_obj_t keywords = get_keywords_from_context();
    dynamic_obj_t dinamici;
    init_obj(&dinamici);
    if (parse_path(path, fissi, keywords, &dinamici) == -1)
        return -1;
    if (!exist(db, keywords, dinamici))
        return -1;
    return -1;
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
    if (parse_path(virtual_path, fissi, keywords, &dinamici) == -1)
        return -1;
    if (!exist(db, keywords, dinamici))
        return -1;
    char* real_path = get_local_path(db, keywords, dinamici);
    errprintf("real_path = `%s'\t", real_path);
    int i = open(real_path, O_RDONLY);
    errprintf("i = %d\n", i);
    if (i == -1)
        return -errno;
    ffi->fh = i;
    return 0;
}

/**
    Implementazione di fuse_operations.opendir()
    \param virtual_path percorso nel file system virtuale di cui è stato
            richiesto il contenuto
    \param ffi informazioni aggiuntive di fuse (in particolare, ffi->fh indica
            il file descriptor del file REALE da aprire)
    \return 0 se la directory esiste, -1 altrimenti
    \sa fuse.h, musicmeshfs_open(), close()
*/
// int db_fuse_opendir(const char* virtual_path, struct fuse_file_info* ffi) {
//     errprintf("[OPENDIR] virtual_path = `%s'\t", virtual_path);
//     dynamic_str_t chiavi, valori;
//     if (parse(virtual_path, &chiavi, &valori) == -1)
//         return -1;
//     if (!exist(chiavi, valori))
//         return -1;
//     dynamic_str_t tmp[2];
//     tmp[0] = chiavi;
//     tmp[1] = valori;
//     ffi->fh = (uint64_t) tmp;
//     return 0;
// }

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
    Implementazione di fuse_operations.releasedir()
    \param virtual_path percorso nel file system virtuale di cui è stato
            richiesto il contenuto
    \param ffi informazioni aggiuntive di fuse (in particolare, ffi->fh indica
            la coppia chiavi / valori da liberare
    \return 0;
    \sa fuse.h, musicmeshfs_open(), close()
 */
// int db_fuse_releasedir(const char* virtual_path, struct fuse_file_info* ffi) {
//     errprintf("[RELEASEDIR] path = `%s'\n", virtual_path);
//     dynamic_str_t* tmp = (dynamic_str_t*)ffi->fh;
//     free_str(&tmp[0]);
//     free_str(&tmp[1]);
//     return 0;   // nella doc dice che non è usata
// }

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
