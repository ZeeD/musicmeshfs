#include "db_fuse_utils.h"
#include "constants.h"   /* KEYWORDS_SIZE, KEYWORDS, column_from_keyword */
#include "../common/utils.h"       /* dynamic_str_t, init_str(), append_str(),
                            index_of_str(), max() */
// #include "../common/sqlite.h"      /* esegui_query_callback() */

/**
    funzioni idiote, ma dannatamente lunghe da scrivere
 */
sqlite3* get_db_from_context() {
    return (sqlite3*)((dynamic_obj_t*)fuse_get_context()->private_data)->buf[0];
}
dynamic_obj_t get_fissi_from_context() {
    return *(dynamic_obj_t*)((dynamic_obj_t*)fuse_get_context()->private_data)->
            buf[1];
}
dynamic_obj_t get_keywords_from_context() {
    return *(dynamic_obj_t*)((dynamic_obj_t*)fuse_get_context()->private_data)->
            buf[2];
}




// /**
//     Controllo *sintattico* (indipendente dal contenuto del database) di una
//     qualunque stringa (in particolare, del path inserito dall'utente).
//
//     Come effetto collaterale, riempie chiavi e valori coi valori ritrovati.
//     Grammatica:
//         PATH   = ^{COPPIA}*"/"({CHIAVE}("/"{VALORE})?)?$
//         COPPIA = "/"{CHIAVE}"/"{VALORE}
//         CHIAVE = qualunque elemento di KEYWORDS non usato in precedenti {COPPIA}
//         VALORE = [^/]+
//     \param path stringa da parserizzare
//     \param chiavi
//     \todo stub
//     \sa constants.h, KEYWORDS
//     \attention KEYWORDS induce una grammatica molto più complessa, ma sempre
//             context-free!
//     \return 0 in caso di successo, -1 in caso d'errori
// */
// int parse(const char* path, dynamic_str_t* chiavi, dynamic_str_t* valori) {
//     init_str(chiavi);
//     init_str(valori);
//     int i = 0, j = 0;
//     if (!path || !path[0])
//         return -1;
//     while (i != -1 && path[i]) {
//         j = i;
//         i = coppia(path, chiavi, valori, i);
//     }
//     if (i != -1 && !path[i]) { // c'e' una coppia di troppo
//         pop_str(chiavi);
//         pop_str(valori);
//         i = j;
//     }
//     if (path[j] != '/')
//         return -1;
//     if (!path[j+1])
//         return 0;   // {COPPIA}*"/"
//     if ((i = chiave(path, chiavi, j+1)) == -1)
//         return -1;
// //     errprintf("path+%d = `%s'\n", i, path+i);
//     if (!path[i])
//         return 0;   // {COPPIA}*"/"{CHIAVE}
// //     errprintf("NON DEVO ARRIVARE QUI\n");
//     if (path[i] != '/')
//         return -1;
//     if ((j = valore(path, valori, i+1)) == -1)
//         return -1;
//     if (!path[j])
//         return 0;   // {COPPIA}*"/"{CHIAVE}"/"{VALORE}
//     return -1;
// }
//
// /**
//     COPPIA = "/"{CHIAVE}"/"{VALORE}
// */
// int coppia(const char* path, dynamic_str_t* chiavi, dynamic_str_t* valori, int
//         offset) {
// //     errprintf("\t\t[COPPIA] path = `%s'\n", path+offset);
//     int ret = 0;
//     if (path[offset] != '/')
//         return -1;
//     offset += 1;
//     if ((ret = chiave(path, chiavi, offset)) == -1)
//         return -1;
//     offset = ret;
//     if (path[offset] != '/') {
//         pop_str(chiavi);    // ultima chiave aggiunta da chiave()
//         return -1;
//     }
//     offset += 1;
//     if ((ret = valore(path, valori, offset)) == -1) {
//         pop_str(chiavi);    // ultima chiave aggiunta da chiave()
//         return -1;
//     }
//     offset = ret;
//     return offset;  // "/"{CHIAVE}"/"{VALORE}
// }
//
// /**
//     CHIAVE = qualunque degli elementi KEYWORDS (non usato, quindi \f$ \not \in
//             chiavi \f$)
// */
// int chiave(const char* path, dynamic_str_t* chiavi, int offset) {
// //     errprintf("\t\t[CHIAVE] path = `%s'\n", path+offset);
//     for (int i=0; i<KEYWORDS_SIZE; i++)
//         if (!contains_str(*chiavi, KEYWORDS[i])) {
//             const char* keyword = KEYWORDS[i];
//             int keyword_size = strlen(keyword);
//             if (!strncmp(path+offset, keyword, keyword_size)) {
//                 append_str(chiavi, keyword);
//                 return offset+keyword_size;
//             }
//         }
//     return -1;
// }
//
// /**
//     VALORE = [^/]+
// */
// int valore(const char* path, dynamic_str_t* valori, int offset) {
// //     errprintf("\t\t[VALORE] path = `%s'\n", path+offset);
//     int i = 0;
//     if (!path[offset] || path[offset] == '/')  // non accetto stringhe vuote
//         return -1;
//     while (path[offset+i] && path[offset+i] != '/') // né '\0' né '/'
//         i += 1;
//     char value[i+1];
//     strncpy(value, path+offset, i);
//     value[i] = '\0';
//     append_str(valori, value);
//     return offset+i;
// }


/**
    \todo stub
*/
int exist(sqlite3* db, dynamic_obj_t chiavi, dynamic_obj_t valori) {
//     errprintf("\t\t[EXIST] Warn, stub\n");
    (void) db;
    (void) chiavi;
    (void) valori;
    return 1;
}

/**
    ritorna il contenuto di una directory :)
*/
dynamic_str_t sub_dir(sqlite3* db, dynamic_obj_t keywords, dynamic_obj_t
        dinamici) {
    dynamic_str_t ret;
    init_str(&ret);
/*    if (chiavi.size == valori.size) {   // elenca chiavi rimanenti
        for (int i=0; i<KEYWORDS_SIZE; i++)
            if (!contains_str(chiavi, KEYWORDS[i])) // KEYWORDS[i] non è usata
                append_str(&ret, KEYWORDS[i]);
    }
    else {  // chiavi.size == (valori.size + 1) -> elenca elementi dal db
        char* query = calcola_query_subdirs(chiavi, valori);
        esegui_query_callback(db, get_one_column, &ret, query);
        free(query);
        for (int i=0; i<ret.size; i++) {
            char* tmp = ret.buf[i];
            ret.buf[i] = preprocessing(tmp);
            free(tmp);
        }
    }*/
    return ret;
}

/*     dynamic_str_t fissi, dinamici;
    char* query = malloc(1);
    query[0] = '\0';
    query = strmalloccat(query, "SELECT DISTINCT ");
    int is_root = !realpath.size || (realpath.size == 1 && !realpath.buf[0][0]);
    // devo prima capire di quale directory elencare i file
    if (is_root)   // == ls ROOT
        calcola_elementi(schema.buf[0], &fissi, &dinamici);
    else
        calcola_elementi(schema.buf[realpath.size], &fissi, &dinamici);
    dbgprint_str(fissi, "schema_to_dirs->fissi");
//     dbgprint_str(dinamici, "schema_to_dirs->dinamici");
    if (dinamici.size)
        query = strmalloccat(query, dinamici.buf[0]);
    for (int i=1; i<dinamici.size; i++) {
        query = strmalloccat(query, ", ");
        query = strmalloccat(query, dinamici.buf[i]);
    }
    query = strmalloccat(query, " FROM ");
    query = strmalloccat(query, TABLE);
    dynamic_str_t all_dinamici;
    init_str(&all_dinamici);
    if (!is_root) { // aggiungi clausola where
        dynamic_str_t valori_where;
        calcola_where(schema, realpath, &valori_where);
        // dbgprint_str(valori_where, "schema_to_dirs->valori_where");
        int indice_where = 0;
        query = strmalloccat(query, " WHERE ");
        dynamic_str_t tmp_fissi, tmp_dinamici;
        if (realpath.size) {
            calcola_elementi(schema.buf[0], &tmp_fissi, &tmp_dinamici);
            extend_str(&all_dinamici, tmp_dinamici);
            if (tmp_dinamici.size) {
                query = strmalloccat(query, "(");
                query = strmalloccat(query, tmp_dinamici.buf[0]);
                char* quoted = sqlite3_mprintf(" = %Q)", valori_where.buf[indice_where++]);
                query = strmalloccat(query, quoted);
                sqlite3_free(quoted);
            }
            for (int j=1; j<tmp_dinamici.size; j++) {
                query = strmalloccat(query, " and (");
                query = strmalloccat(query, tmp_dinamici.buf[j]);
                char* quoted = sqlite3_mprintf(" = %Q)", valori_where.buf[indice_where++]);
                query = strmalloccat(query, quoted);
                sqlite3_free(quoted);
            }
        }
        for (int i=1; i<realpath.size; i++) {
            calcola_elementi(schema.buf[i], &tmp_fissi, &tmp_dinamici);
            extend_str(&all_dinamici, tmp_dinamici);
            for (int j=0; j<tmp_dinamici.size; j++) {
                query = strmalloccat(query, " and (");
                query = strmalloccat(query, tmp_dinamici.buf[j]);
                char* quoted = sqlite3_mprintf(" = %Q)", valori_where.buf[indice_where++]);
                query = strmalloccat(query, quoted);
                sqlite3_free(quoted);
            }
        }
    }
    query = strmalloccat(query, ";");
    dynamic_str_t* buf[2];
    buf[0] = &fissi;
    buf[1] = &ret;
    esegui_query_callback(DB, piglia_da_db, buf, query);
    free(query);
    free_str(&fissi);
    free_str(&dinamici);*/

/**
    \todo: trovare altro nome!!!
*/
/* int piglia_da_db(void* buf, int n_colonne, char** value, char** header) {
    (void) header;
    dynamic_str_t* fissi = ((dynamic_str_t**) buf)[0];
    dynamic_str_t* ret = ((dynamic_str_t**) buf)[1];
    char* tmp = malloc(1);
    tmp[0] = '\0';
    for (int i=0; i<n_colonne; i++)
        tmp = strmalloccat(strmalloccat(tmp, fissi->buf[i]), value[i]);
    tmp = strmalloccat(tmp, fissi->buf[n_colonne]);
    append_str(ret, tmp);
//     dbgprint_str(*ret, "(DELME) ret");
    free(tmp);
    return 0;
}
*/

/**
    schema è un frammento (solo una dir) dello schema originario
    assunto: len(fissi) == len(dinamici) + 1
    ("grammatica" ==  {fisso, dinamico}*, fisso  )
*/
/* void calcola_elementi(char* schema, dynamic_str_t* fissi,
        dynamic_str_t* dinamici) {
    init_str(fissi);
    init_str(dinamici);
    int i=0, j=0, turno_fissi=1;
    for (; schema[i] != '\0'; i++) {
        if (turno_fissi && schema[i] == '%') {
            if (schema[i+1] == '%')
                continue;   // ignora
            if (i > j) {
                char* tmp = calloc(i-j+1, 1); // sizeof(char) == 1
                strncpy(tmp, schema+j, i-j);
                tmp[i-j] = '\0';
                append_str(fissi, tmp);
                free(tmp);
            }
            else
                append_str(fissi, "");
            j = i;
            i--;    // non devo perdermi il '%'
            turno_fissi = !turno_fissi;
        }
        else if (!turno_fissi && schema[i] == '%') {
            for (int k=0; k<SIZE; k++) {
                int l = strlen(KEYWORDS[k]);
                if (!strncmp(schema+i, KEYWORDS[k], l)) {
                    append_str(dinamici, COLUMNS[k]);
                    i += l - 1; // non voglio perdermi il '%'
                    j = i + 1;
                    turno_fissi = !turno_fissi;
                    break;
                }
            }
        }
    }
    if (turno_fissi) {
        char* tmp = calloc(i-j+1, 1); // sizeof(char) == 1
        strncpy(tmp, schema+j, i-j);
        tmp[i-j] = '\0';
        append_str(fissi, tmp);
        free(tmp);
    }
}
*/

/**
    Calcola i valori (ordinati) della clausola WHERE di schema_to_dirs()
    \param schema schema da usare
    \param realpath path reale
    \param valori_where valori da usare nella clausola WHERE (modificato in-place)
*/
/* void calcola_where(dynamic_str_t schema, dynamic_str_t realpath, dynamic_str_t*
        valori_where) {
    init_str(valori_where);
    for (int i=0; i<realpath.size; i++) {
        dynamic_str_t fissi, variabili;
        calcola_elementi(schema.buf[i], &fissi, &variabili);
        // so che devo avere (%FISSO%VARIABILE)*%FISSO, ne approfitto spudoratamente!
        for (int j=0, k=0, lunghezza_variabile=0; j<fissi.size-1; lunghezza_variabile=0, j++) {
            k += strlen(fissi.buf[j]); // ignoro i caratteri dell'elemento fisso precedente
            int lunghezza_fisso = strlen(fissi.buf[j+1]);

//             dynamic_str_t possibili_valori_di_variabile;
//             init_str(&possibili_valori_di_variabile);
//             char* a = malloc(1); a[0] = '\0';
//             // TODO: mettere where?
//             a = strmalloccat(strmalloccat(strmalloccat(strmalloccat(strmalloccat
//                     (a, "SELECT DISTINCT "), variabili.buf[j]), " FROM "),
//                     TABLE), ";");
//             esegui_query_callback(DB, piglia_stringa,
//                     &possibili_valori_di_variabile, a);
//             free(a);

            // finchè non trovo l'elemento variabile tra i possibi... passa avanti
            for (; 1; lunghezza_variabile++) {
                if (!lunghezza_fisso && j == fissi.size - 2) {
                    if (!realpath.buf[i][k+lunghezza_variabile])
                        break;
                }
                else {
                    if (!strncmp(realpath.buf[i]+k+lunghezza_variabile, fissi.buf[j+1], lunghezza_fisso)) {
                        char foo[lunghezza_variabile+1];
                        foo[0] = '\0';
                        strncpy(foo, realpath.buf[i]+k, lunghezza_variabile);
                        foo[lunghezza_variabile] = '\0';
//                         if (contains_str(possibili_valori_di_variabile, foo))
                            break;
                    }
                }
            }
            char tmp[lunghezza_variabile+1];
            tmp[0] = '\0';
            strncpy(tmp, realpath.buf[i]+k, lunghezza_variabile);
            tmp[lunghezza_variabile] = '\0';
            append_str(valori_where, tmp);
            k += lunghezza_variabile;
        }
        free_str(&fissi);
        free_str(&variabili);
    }
}
*/

/**
    funzione booleana che riconosce se il path richiesto è una directory
    al momento è solo un !is_a_dir()
    \param virtual_path percorso del fs virtuale
    \param schema array di stringhe dello schema di presentazione
    \sa is_a_dir()
    \return 1 se è un file, 0 altrimenti
 */
// int is_a_file(dynamic_str_t schema, dynamic_str_t virtual_path) {
//     return ! is_a_dir(schema, virtual_path);
// }

/**
    Assume che buf sia un dynamic_str_t, e che la query abbia n_colonne == 1
    Riempie buf con le tuple trovate
*/
int get_one_column(void* buf, int n_colonne, char** value, char** header) {
    (void) header; // non usato
    if (n_colonne != 1)
        return -1;
    append_str(((dynamic_str_t*)buf), value[0]);
    return 0;
}

/**
    Assume che buf sia un dynamic_obj_t, composto da 2 dynamic_str_t, e che la
    query abbia n_colonne == 2
    Riempie le coppie di buf con le tuple trovate
 */
int get_two_columns(void* buf, int n_colonne, char** value, char** header) {
    (void) header; // non usato
    if (n_colonne != 1)
        return -1;
    append_str(((dynamic_str_t*)((dynamic_obj_t*)buf)->buf[0]), value[0]);
    append_str(((dynamic_str_t*)((dynamic_obj_t*)buf)->buf[1]), value[1]);
    return 0;
}

/**
    Assume che chiavi.size == valori.size + 1, e che chiavi indichi le KEYWORDS
    dell'utente! (\f$ \forall chiave \in chiavi \to chiave \in KEYWORDS \f$)
    \param chiavi keyword \f$ \in \f$ KEYWORDS
    \param valori valori presenti nel database nelle colonne nelle colonne
            corrispondenti a chiavi
    \return query del tipo "SELECT chiavi[-1] ... WHERE (chiavi[:-1] = valori)",
            NULL in caso d'errore
*/
char* calcola_query_subdirs(dynamic_str_t chiavi, dynamic_str_t valori) {
    if (chiavi.size != valori.size + 1)
        return NULL;
    dynamic_str_t colonne, tabelle, where;
    init_str(&colonne);
    init_str(&tabelle);
    init_str(&where);
    for (int i=0; i<chiavi.size; i++) {
        // patch per COUNT(COLONNA)
        if (!strncmp(chiavi.buf[i], "COUNT(", 6))
            append_str(&colonne, chiavi.buf[i]);
        else {
            const char* colonna = column_from_keyword(chiavi.buf[i]);
            if (!colonna)
                return NULL;
            append_str(&colonne, colonna);
            char* tabella = split(colonna, '.').buf[0];
            if (index_of(TABLES, TABLES_SIZE, tabella) == -1)
                return NULL;
            if (!contains_str(tabelle, tabella))
                append_str(&tabelle, tabella);
        }
    }
    if (!tabelle.size)
        return NULL;

    /* ora in tabelle ho tutte le tabelle necessarie, mi tocca calcolare quali
    sono le corrispettive condizioni di where + le ulteriori condizioni di
    where! */

    char* query = strmalloccat(strmalloccat(strmalloccat(strmalloccat(calloc(1,
            1), "SELECT DISTINCT "), colonne.buf[colonne.size-1]), " FROM "),
            tabelle.buf[0]);

//     errprintf("calcola_query_subdirs->query = `%s'\n", query);

    // calcolo condizioni where dei join
    for (int i=0; i<tabelle.size; i++)
        for (int j=i+1; j<tabelle.size; j++) {
            char* titj = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    tabelle.buf[i]), ","), tabelle.buf[j]);
            char* tjti = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    tabelle.buf[j]), ","), tabelle.buf[i]);
            // se ci sono entrambi in un elemento di JOINS, metticelo e basta
            if (index_of(JOINS, JOINS_SIZE, titj) != -1)
                append_str(&where, where_from_join(titj));
            else if (index_of(JOINS, JOINS_SIZE, tjti) != -1)
                append_str(&where, where_from_join(tjti));
            else
                cercaPercorso(tabelle.buf[i], tabelle.buf[j], &tabelle, &where);
            free(titj);
            free(tjti);
        }

    for (int i=1; i<tabelle.size; i++)
        query = strmalloccat(strmalloccat(query, ", "), tabelle.buf[i]);

    // innesto delle condizioni where dei join nella query

    if (where.size || valori.size)
        query = strmalloccat(query, " WHERE ");

    if (where.size)
        query = strmalloccat(query, where.buf[0]);
    for (int i=1; i<where.size; i++)
        query = strmalloccat(strmalloccat(query, " AND "), where.buf[i]);

    // ulteriori condizioni di where (quelle di chiavi[:-1] = valori)
    if (!where.size && valori.size) {
        char* quoted = sqlite3_mprintf(") GLOB %Q)", valori.buf[0]);
        query = strmalloccat(strmalloccat(strmalloccat(query, "(prep("),
                column_from_keyword(chiavi.buf[0])), quoted);
        sqlite3_free(quoted);
    }
    for (int i=(where.size?0:1); i<valori.size; i++) {
        char* quoted = sqlite3_mprintf(") GLOB %Q)", valori.buf[i]);
        query = strmalloccat(strmalloccat(strmalloccat(query, " AND (prep("),
                column_from_keyword(chiavi.buf[i])), quoted);
        sqlite3_free(quoted);
    }

//     query = strmalloccat(strmalloccat(query, " GROUP BY "),
//             colonne.buf[colonne.size-1]);
//     errprintf("calcola_query_subdirs) query = `%s'\n", query);
    free_str(&colonne);
    free_str(&tabelle);
    free_str(&where);
    return query;
}

/**
    Funzione di utilità che, date due tabelle non presenti in JOINS, mi cerca il
    join più adatto.
    \param t1 tabella presente in TABLES
    \param t2 tabella associata a t1, non presente in TABLES
    \param tabelle tabelle da mettere in join (modificato in-place)
    \param where condizioni di where associate ai join della query (modificato
            in-place)
    \attention al momento funziona solo se \f$ \exists \mathrm{t_{3}} \in
            \mathrm{TABLES} \mbox{ tale che }
            \left(\mathrm{t_{1}},\mathrm{t_{3}}\right) \in \mathrm{JOINS} \land
            \left(\mathrm{t_{3}},\mathrm{t_{2}}\right) \in \mathrm{JOINS} \f$
*/
void cercaPercorso(const char* t1, const char* t2, dynamic_str_t* tabelle,
        dynamic_str_t* where) {
    int done = 0;
    for (int i=0; i<TABLES_SIZE && !done; i++) {
//         errprintf("t1 = `%s', t2 = `%s', t3 = `%s'\n", t1, t2, TABLES[i]);
        if (!strcmp(t1, TABLES[i]) || !strcmp(t2, TABLES[i]))
            continue;   // devo scoprire t3 != t1 e t2
        char* t1t3 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1), t1),
                ","), TABLES[i]);
        char* t3t1 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                TABLES[i]), ","), t1);
//         errprintf("t1t3 = `%s', t3t1 = `%s'\n", t1t3, t3t1);
        int m = 0, n = 0;
        if ((m = max(index_of(JOINS, JOINS_SIZE, t1t3), index_of(JOINS,
                JOINS_SIZE, t3t1))) != -1) {
            char* t3t2 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    TABLES[i]), ","), t2);
            char* t2t3 = strmalloccat(strmalloccat(strmalloccat(calloc(1, 1),
                    t2), ","), TABLES[i]);
            if ((n = max(index_of(JOINS, JOINS_SIZE, t3t2), index_of(JOINS,
                    JOINS_SIZE, t2t3))) != -1) {
                // ho m e n che sono 2 indici di JOINS, t3 = TABLES[i]
//                 errprintf("aggiungo `%s' a tabelle?\n", TABLES[i]);
//                 dbgprint_str(*tabelle, "tabelle");
//                 errprintf("%s\n", !contains_str(*tabelle, TABLES[i])?"si":"no");
                if (!contains_str(*tabelle, TABLES[i]))
                    append_str(tabelle, TABLES[i]);
//                 dbgprint_str(*tabelle, "(dopo)tabelle");
                append_str(where, WHERE[m]);
                append_str(where, WHERE[n]);
                break;
                done = !done;
            }
            free(t2t3);
            free(t3t2);
        }
        free(t1t3);
        free(t3t1);
    }
}
