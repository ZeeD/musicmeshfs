#include "../common/utils.h"     /* errprintf(), dynamic_str_t, init_str(),
        append_str() */
#include <stdlib.h>  /* exit(), EXIT_FAILURE */
#include "commons.h" /* inotify_t, new_inotify_t(), sqlite3, crea_db_vuoto(),
        init_db_and_inotify(), free_str(), local_loop() */
#include <stdio.h>   /* perror() */

int main(int argc, char** argv) {
    // basico controllo dei parametri
    if (argc < 2) {
        errprintf("USO: `%s DATABASE [FILES_DA_MONITORARE]'\n", argv[0]);
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
