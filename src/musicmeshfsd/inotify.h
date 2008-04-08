#ifndef INOTIFY_H
#define INOTIFY_H

#include <sys/inotify.h>     /* inotify_init, inotify_add_watch, IN_ALL_EVENTS
                                struct inotify_event */
#include <stdio.h>           /* printf, perror, puts */
#include <string.h>          /* strcat, strlen */
#include "../common/utils.h" /* dynamic_str_t, dynamic_int_t */

/**
    Struttura che incapsula le informazioni necessarie per inotify

    \param inotify_istance istanza di inotify corrente
    \param watch_fds file descriptor "osservati" da inotify_istance
    \param files nomi dei files corrispondenti ai file descriptor osservati
    \sa dynamic_int_t, dynamic_str_t, inotify.h, inotify_tools
 */
typedef struct {
    int instance;
    dynamic_int_t watch_fds;
    dynamic_str_t files;
} inotify_t;

const unsigned int MAX_BUFFER_LEN;

inotify_t new_inotify_t();

char* watch_fd_to_file(int, inotify_t);
int file_to_watch_fd(char*, inotify_t);

int add_watch(void*, char*);
void rm_all_watches(inotify_t);

void info_print(struct inotify_event*, inotify_t);
char* event_file_name(struct inotify_event*, inotify_t);

#endif
