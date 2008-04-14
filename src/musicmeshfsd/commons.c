/*
    Copyright © 2008 Vito De Tullio

    This file is part of MusicMeshFSd.

    MusicMeshFSd is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MusicMeshFSd is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MusicMeshFSd.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "commons.h"
#include "../common/sqlite.h"    /* add_local_file_in_db(), del_local_file_from_db() */
#include "inotify.h"             /* add_watch(), rem_watch(), MAX_BUFFER_LEN,
        rm_all_watches(), event_file_name() */

/**
    Inizializza il database con i file presenti nei tree con radici params

    \param params vettore dinamico dei path delle radici da dove analizzare i
            file da includere in db
    \param db database (preferibilmente vuoto)
    \param inotify istanza di inotify_t di riferimento
    \return 0 se tutto è a posto, l'ultimo valore di sqlite3_exec() se sono
            occorsi errori
    \sa crea_db_vuoto(), esegui_query()
*/
int init_db_and_inotify(dynamic_str_t params, sqlite3* db, inotify_t* inotify) {
    // versione delayed, per evitare falsi positivi all'inizio
    dynamic_str_t to_watch, to_add_in_db;
    init_str(&to_watch);
    init_str(&to_add_in_db);
    for (int i=0; i<params.size; i++)
        if (walk(params.buf[i],
                (int(*)(void*, const char* str))append_str, &to_add_in_db,
                (int(*)(void*, const char* str))append_str, &to_watch))
            return -1;
    esegui_query(db, "BEGIN TRANSACTION");
    for (int i=0; i<to_add_in_db.size; i++)
        add_local_file_in_db(db, to_add_in_db.buf[i]);
    esegui_query(db, "COMMIT");
    for (int i=0; i<to_watch.size; i++)
        add_watch(inotify, to_watch.buf[i]);
    return 0;
}

/**
    main loop

    Si presuppone che chiuda inotify e db!
    \param db database condiviso
    \param inotify istanza di inotify da monitorare
    \param callback funzione di callback da chiamare alla notifica di un evento
    \sa MAX_BUFFER_LEN
*/
void local_loop(sqlite3* db, inotify_t inotify,
        int(*callback)(struct inotify_event*, sqlite3*, inotify_t)) {
    char queue[MAX_BUFFER_LEN];
    while (1) {
        ssize_t readed = read(inotify.instance, queue, MAX_BUFFER_LEN);
        if (readed == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        for (int i=0; i<readed;) {
            struct inotify_event* event = (struct inotify_event*) &queue[i];
            if (callback && callback(event, db, inotify) == -1)
                break;
            i += sizeof(struct inotify_event)+event->len;
        }
    }
    rm_all_watches(inotify);
}

/**
    callback usata quando in loop() si notifica un evento

    regole utilizzate:
    - se l'evento riguarda una directory
        - se viene creata o se viene aggiunta, aggiungila a files e watch_fds
        - se viene distrutta o spostata via, rimuovila da files e watch_fds
    \todo finire di decidere queste benedette regole da usare!

    \param event evento notificato
    \param db database sqlite3 (modificato in-place)
    \param inotify instanza di inotify_t di riferimento
    \return 0 in caso di successo, -1 altrimenti
*/
int local_event_callback(struct inotify_event* event, sqlite3* db,
        inotify_t inotify) {
//     info_print(event, inotify); // TODO: rimuovere info_print()!!!
    char* file_name = event_file_name(event, inotify);
    if (event->mask & IN_ISDIR) {    // se è una directory
        if (event->mask & (IN_CREATE | IN_MOVED_TO))    // creata o spostata in
            add_watch(&inotify, file_name);
        else if (event->mask & (IN_DELETE | IN_MOVED_FROM)) // rimossa
            rem_watch(&inotify, file_name);
        else
            errprintf("NON SO CHE FARE CON `%s'\n", file_name);
    }
    else {  // se è un file
        if (event->mask & (IN_DELETE | IN_MOVED_FROM))
            del_local_file_from_db(db, file_name);
        else if (event->mask & (IN_CREATE | IN_MOVED_TO))
            add_local_file_in_db(db, file_name);
        else if (event->mask & IN_CLOSE_WRITE) { // (IN_CLOSE_WRITE | IN_ATTRIB)
            del_local_file_from_db(db, file_name);
            char* dirname = extract_dirname(file_name);
            rem_watch(&inotify, dirname);
            add_local_file_in_db(db, file_name);
            add_watch(&inotify, dirname);
            free(dirname);
        }
        else if (event->mask & IN_IGNORED)
            ;   // erh, non ci faccio niente...
        else
            errprintf("NON SO CHE FARE CON `%s'\n", file_name);
    }
    return 0;
}

/**
    Attraversa root (ricorsivamente, se è una directory), applicando
    reg_callback() per ogni file regolare, e dir_callback() per ogni directory
    visitata

    reg_callback() e dir_callback() hanno, come parametri, prima un parametro
    generico, e poi il path completo con radice in root
    \param root path della radice dell'albero da visitare
    \param reg_callback funzione da applicare ad ogni file regolare visitato
    \param reg_first_param primo parametro da passare a reg_callback
    \param dir_callback funzione da applicare ad ogni directory visitata
    \param dir_first_param primo parametro da passare a dir_callback
    \return 0 se non ci sono stati problemi, -1 altrimenti
*/
int walk(char* root, int(*reg_callback)(void*, const char*), void*
        reg_first_param, int(*dir_callback)(void*, const char*), void*
        dir_first_param) {
    struct stat data;
    if (stat(root, &data) == -1) {
        perror("stat");
        return -1;
    }
    if (S_ISREG(data.st_mode)) {
        if (*reg_callback && reg_callback(reg_first_param, root) == -1)
            return -1;
    }
    if (S_ISDIR(data.st_mode)) {
        if (*dir_callback && dir_callback(dir_first_param, root) == -1)
            return -1;

        DIR* dir = opendir(root);
        if (dir == NULL) {
            perror("opendir");
            return -1;
        }
        struct dirent* direntry;
        while ((direntry = readdir(dir))) {
            if (direntry->d_name[0] != '.') {   // ignora file nascosti
                char subroot[strlen(root) + 1 + strlen(direntry->d_name)];
                subroot[0] = '\0';
                strcat(subroot, root);
                strcat(subroot, "/");
                strcat(subroot, direntry->d_name);
                if (walk(subroot, reg_callback, reg_first_param, dir_callback,
                        dir_first_param) == -1)
                    return -1;
            }
        }
        closedir(dir);
    }
    return 0;
}
