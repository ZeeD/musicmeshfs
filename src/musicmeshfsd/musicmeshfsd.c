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

#include <stdlib.h>          /* exit(), EXIT_FAILURE */
#include <stdio.h>           /* perror() */
#include <err.h>             /* warn() */
#include "commons.h"         /* inotify_t, new_inotify_t(), sqlite3,
        crea_db_vuoto(), init_db_and_inotify(), free_str(), local_loop() */
#include "../common/utils.h" /* dynamic_str_t, init_str(), append_str() */

int main(int argc, char** argv) {
    // basico controllo dei parametri
    if (argc < 2) {
        warn("USO: `%s DATABASE [FILES_DA_MONITORARE]'\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // creazione delle due strutture principali
    inotify_t inotify = new_inotify_t();
    if (inotify.instance == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }
    sqlite3* db = crea_db_vuoto(argv[1]);

    // gestione passaggio parametri
    dynamic_str_t params;
    init_str(&params);
    if (argc == 2)                  /* se non ci sono altri parametri  */
        append_str(&params, ".");   /* controlla la directory corrente */
    else
        for (int i=2; i<argc; i++)
            append_str(&params, argv[i]);

    // inizializzazione delle due strutture principali (coi parametri)
    init_db_and_inotify(params, db, &inotify);
    free_str(&params);

    // main loop (con funzione da chiamare in caso di evento locale)
    local_loop(db, inotify, local_event_callback);
}
