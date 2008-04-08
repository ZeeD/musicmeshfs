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

#include "inotify.h"
#include "../common/utils.h"
#include "commons.h"

/**
    MAX_BUFFER_LEN rappresenta la massima dimensione usabile per la coda degli
    eventi. Come effetto collaterale, non è possibile monitorare la creazione /
    modifica di files con nomi di lunghezza superiore a
    MAX_BUFFER_LEN - sizeof(struct inotify_event)
*/
const unsigned int MAX_BUFFER_LEN = 1024;

/**
    Genera e inizializza una nuova struttura di tipo inotify_t

    \return da notare che, se ci sono stati errori, lo si potrà sapere
            analizzando il valore di new_inotify_t().instance == -1
*/
inotify_t new_inotify_t() {
    inotify_t r;
    init_int(&r.watch_fds);
    init_str(&r.files);
    r.instance = inotify_init();
    return r;
}

/**
    stampa le informazioni su un evento accorso

    \param event evento accorso
    \param inotify istanza di inotify_t di riferimento
*/
void info_print(struct inotify_event* event, inotify_t inotify) {
    printf("nome = `%s'\t", event_file_name(event, inotify));
    if (event->mask & IN_ACCESS)        printf("ACCESS ");
    if (event->mask & IN_ATTRIB)        printf("ATTRIB ");
    if (event->mask & IN_CLOSE_WRITE)   printf("CLOSE_WRITE ");
    if (event->mask & IN_CLOSE_NOWRITE) printf("CLOSE_NOWRITE ");
    if (event->mask & IN_CREATE)        printf("CREATE ");
    if (event->mask & IN_DELETE)        printf("DELETE ");
    if (event->mask & IN_DELETE_SELF)   printf("DELETE_SELF ");
    if (event->mask & IN_MODIFY)        printf("MODIFY ");
    if (event->mask & IN_MOVE_SELF)     printf("MOVE_SELF ");
    if (event->mask & IN_MOVED_FROM)    printf("MOVED_FROM ");
    if (event->mask & IN_MOVED_TO)      printf("MOVED_TO ");
    if (event->mask & IN_OPEN)          printf("OPEN ");
    if (event->mask & IN_IGNORED)       printf("IGNORED ");
    if (event->mask & IN_ISDIR)         printf("ISDIR ");
    if (event->mask & IN_Q_OVERFLOW)    printf("Q_OVERFLOW ");
    if (event->mask & IN_UNMOUNT)       printf("UNMOUNT ");
    puts("");
}

/**
    recupera il nome del file associato all'evento richiesto

    \param event evento accorso
    \param inotify istanza di inotify_t di riferimento
    \return il nome del file associato all'evento (memoria da liberare
            esplicitamente), NULL altrimenti
*/
char* event_file_name(struct inotify_event* event, inotify_t inotify) {
//     fprintf(stderr, "wd = %d\t", event->wd);
//     dbgprint_int(inotify.watch_fds, "event_file_name->inotify.watch_fds");
    for (int i=0; i<inotify.watch_fds.size; i++)
        if (event->wd == inotify.watch_fds.buf[i]) {
            char* buf = inotify.files.buf[i];
            char* r = calloc(strlen(buf) + 1 + event->len, 1);
            r = strcat(r, buf);
            if (event->len) {
                r = strcat(r, "/");
                r = strcat(r, event->name);
            }
//             fprintf(stderr, "(event_file_name) r = `%s'\n", r);
            return r;
        }
    return NULL;
}

/**
    aggiunge un percorso da monitorare (callback di utils -> walk)

    \param path percorso per il file da monitorare
    \param inotify istanza di inotify_t di riferimento
    \sa rm_all_watches(), utils.h
    \todo decidere se IN_ALL_EVENTS va bene!!!
    \return 0 se non ci sono stati problemi, -1 altrimenti
*/
int add_watch(void* inotify, char* path) {
    int watch_fd = inotify_add_watch(((inotify_t*)inotify)->instance, path,
            IN_ALL_EVENTS);
    if (watch_fd == -1) {
        perror("inotify_add_watch");
        return -1;
    }
    append_int(&((inotify_t*)inotify)->watch_fds, watch_fd);
    append_str(&((inotify_t*)inotify)->files, path);
    return 0;
}

/**
    rimuove ogni file descriptor watched da un'instanza di inotify

    Da notare come files e watch_fds devono essere della stessa lunghezza!
    \param inotify istanza di inotify_t a cui rimuovere tutti i file descriptors
*/
void rm_all_watches(inotify_t inotify) {
    for (int i=0; i<inotify.watch_fds.size; i++)
        if (inotify_rm_watch(inotify.instance, inotify.watch_fds.buf[i])==-1) {
            perror("inotify_rm_watch");
            exit(EXIT_FAILURE);
        }
    free_int(&inotify.watch_fds);
    free_str(&inotify.files);
}
