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

#include "parser.h"
#include "constants.h"   /* KEYWORDS, TABLE, COLUMNS */
#include "../common/utils.h"       /* dynamic_str_t, init_str, */

const int IS_A_DIR = 1;
const int IS_A_FILE = 0;

/**
    Analizza la stringa pattern per la verifica che sia uno schema valido per
    musicmeshfsc.
    Come effetto collaterale, riempie due vettori di dynamic_str_t (non ha
    effetti collaterali se si passa NULL ad entrambi i valori)

    SCHEMA = NOME ("/" NOME)*
    NOME = NOME1 | NOME2
    NOME1 = FISSO NOME2?
    NOME2 = KEYWORD NOME1?
    FISSO = ([^/%] | "%%")+
    KEYWORD = %artist | %year | %album | ...

    Nota che KEYWORD e dipende dal contenuto di KEYWORDS, definito in
            constants.c

    Al momento gli unici vincoli sono che
# non deve né iniziare né terminare per '/'
# il carattere '%' non è usato liberamente: esso infatti è usato per
        delimitare i segnaposto da usare; gli unici segnaposto supportati sono:
        - %artist -> Artista
        - %year -> Anno
        - %album -> Album
        - %track -> Traccia
        - %title -> Titolo
        - %genre -> Genere
        - %host -> Host
        - %path -> Path originale
        - %type -> Tipo del file originale
# gli elementi di tipo FISSO non possono essere stringhe vuote, né essere il
        solo carattere '_' (poiché è usato per altri scopi)


    es:
        %artist/%year - %album/%track + %title.%type
    diventa:
        NOME   "/"       NOME          "/"       NOME
        NOME2  "/"       NOME2         "/"       NOME2
        KEYWORD"/"KEYWORD     NOME1    "/"KEYWORD    NOME1
        KEYWORD"/"KEYWORD FISSO  NOME2 "/"KEYWORD FISSO     NOME2
        KEYWORD"/"KEYWORD FISSO KEYWORD"/"KEYWORD FISSO KEYWORD NOME1
        KEYWORD"/"KEYWORD FISSO KEYWORD"/"KEYWORD FISSO KEYWORD FISSO NOME2
        KEYWORD"/"KEYWORD FISSO KEYWORD"/"KEYWORD FISSO KEYWORD FISSO KEYWORD

        %artist"/" %year  " - " %album "/" %track " + " %title   "."   %type

    \param schema schema da verificare
    \param fissi vettore contenente gli elementi fissi riconosciuti
    \param keywords vettore contenente le keywords riconosciute
    \return 0 in caso di successo, -1 altrimenti
    \note fissi e keywords non sono riempiti se viene passato NULL
*/
int parse_schema(const char* schema, dynamic_obj_t* fissi, dynamic_obj_t*
        keywords) {
    dynamic_str_t *el_fisso = NULL, *el_keyword = NULL;
    if (fissi) {
        el_fisso = malloc(sizeof(dynamic_str_t));
        init_str(el_fisso);
    }
    if (keywords) {
        el_keyword = malloc(sizeof(dynamic_str_t));
        init_str(el_keyword);
    }
    int ret = nome(schema, 0, el_fisso, el_keyword);
    if (ret == -1)
        return -1;  // non c'è neanche un frammento
    while (ret != -1) {
        if (fissi)
            append_obj(fissi, el_fisso);
        if (keywords)
            append_obj(keywords, el_keyword);
        if (!schema[ret])
            return 0;   // fine stringa
        if (schema[ret++] != '/')
            return -1;
        if (fissi) {
            el_fisso = malloc(sizeof(dynamic_str_t));
            init_str(el_fisso);
        }
        if (keywords) {
            el_keyword = malloc(sizeof(dynamic_str_t));
            init_str(el_keyword);
        }
        ret = nome(schema, ret, el_fisso, el_keyword);
    }
    return -1;
}

/**
    NOME = NOME1 | NOME2
*/
int nome(const char* schema, int offset, dynamic_str_t* fissi, dynamic_str_t*
            keywords) {
    int ret = nome1(schema, offset, fissi, keywords);
    if (ret == -1) {
        // TODO: rimuovere da fissi e da keywords gli ultimi elementi
        return nome2(schema, offset, fissi, keywords);
    }
    return ret;
}

/**
    NOME1 = FISSO NOME2?
*/
int nome1(const char* schema, int offset, dynamic_str_t* fissi, dynamic_str_t*
            keywords) {
    int ret = fisso(schema, offset, fissi);
    if (ret == -1)
        return -1;
    int ret2 = nome2(schema, ret, fissi, keywords);
    if (ret2 == -1) {
        // TODO: rimuovere da fissi e da keywords gli ultimi elementi
        return ret;
    }
    return ret2;
}

/**
    NOME2 = KEYWORD NOME1?
*/
int nome2(const char* schema, int offset, dynamic_str_t* fissi, dynamic_str_t*
        keywords) {
    int ret = keyword(schema, offset, keywords);
    if (ret == -1)
        return -1;
    if (fissi)
        if (!fissi->size)
            append_str(fissi, "");  // patch per "fake" fisso, per fare iniziare sempre con fissi
    int ret2 = nome1(schema, ret, fissi, keywords);
    if (ret2 == -1) {
        // TODO: rimuovere da fissi e da keywords gli ultimi elementi
        return ret;
    }
    return ret2;
}

/**
    FISSO = ([^/%] | "%%")+
*/
int fisso(const char* schema, int offset, dynamic_str_t* fissi) {
    dynamic_str_t componenti_fisso;
    if (fissi)
        init_str(&componenti_fisso);
    if (!schema[offset] || schema[offset] == '/' ||
            (schema[offset] == '%' && schema[offset+1] != '%'))
        return -1;  // deve avere lunghezza > 0!
    int bkp = offset;
    for (; schema[offset] && schema[offset] != '/'; offset++) {
        if (schema[offset] == '%') {
            if (schema[offset+1] == '%') {
                if (fissi) {
                    char tmp[offset-bkp+1];
                    tmp[0] = tmp[offset-bkp] = '\0';
                    strncpy(tmp, schema+bkp, offset-bkp);
                    append_str(&componenti_fisso, tmp);
                }
                bkp = ++offset + 1;
            }
            else    // dev'essere una keyword -> mi fermo
                break;
        }
    }
    if (fissi) {
        char tmp[offset-bkp+1];
        tmp[0] = tmp[offset-bkp] = '\0';
        strncpy(tmp, schema+bkp, offset-bkp);
        append_str(&componenti_fisso, tmp);
        append_str(fissi, join(componenti_fisso, '%'));
        free_str(&componenti_fisso);
    }
    return offset;
}

/**
    KEYWORD = %artist | %year | %album | ...
*/
int keyword(const char* schema, int offset, dynamic_str_t* keywords) {
    if (schema[offset++] != '%')    // le keywords iniziano per '%'
        return -1;
    for (unsigned i=0; i<KEYWORDS_SIZE; i++) {
        int len = strlen(KEYWORDS[i]);
        if (!strncmp(schema+offset, KEYWORDS[i], len)) {
            if (keywords)
                append_str(keywords, KEYWORDS[i]);
            return offset+len;
        }
    }
    return -1;
}

// path

/**
    analizza il generico percorso, e avvalora dinamici

    es:
        schema = %artist/%year - %album/%track + %title.%type
    tramite parse_schema() ottengo
        fissi = [ [""], ["", " - "], ["", " + , "."] ]
        keywords = [ ["artist"], ["year", "album"], ["track", "title", "type"] ]
    se ora ho un percorso del tipo
        path =
            "/Max Gazzè/2008 - Tra l'aratro e la radio/02 + Il solito sesso.mp3"
    allora tramite parse_path() == IS_A_FILE ottengo
        dinamici = [ ["Max Gazzè"], ["2008", "Tra l'aratro e la radio"], ["02",
            "Il solito sesso", "mp3"] ]
    se invece ho un percorso tipo
        path = "/Max Gazzè/2008 - Tra l'aratro e la radio" # già normalizzato!
    allora tramite parse_path() == IS_A_DIR ottengo
        dinamici = [ ["Max Gazzè"], ["2008", "Tra l'aratro e la radio"] ]

    \param path percorso da analizzare
    \param fissi elementi fissi *già analizzati* tramite parse_schema()
    \param keywords keywords *già analizzate* tramite parse_schema()
    \param dinamici elementi dinamici ottenuti sostituendo le keywords
            (modificato in-place)
    \return
            IS_A_FILE se path è un percorso valido, per un file,
            IS_A_DIR se path è un percorso valido, per una directory,
            -1 altrimenti
 */
int parse_path(const char* path, dynamic_obj_t fissi, dynamic_obj_t keywords,
        dynamic_obj_t* dinamici) {
    if (!path || !path[0])  // dev'essere almeno una stringa non vuota!
        return -1;
    init_obj(dinamici);
    if ((path[0] == '/') && (!path[1])) // caso particolare: è "/"
        return IS_A_DIR;
    dynamic_str_t nomi = split(path+1, '/');
    for (int i=0; i<nomi.size; i++) {
        /*
            directory = ^ el_fisso (el_dinamico el_fisso)* el_dinamico? $
        quindi avrò
            0 < el_fissi.size == el_dinamici.size + ( 0 || 1 )
        */
        dynamic_str_t el_fissi = *(dynamic_str_t*)(fissi.buf[i]);
        if (!el_fissi.size)
            return -1;
        dynamic_str_t el_keywords = *(dynamic_str_t*)(keywords.buf[i]);
        dynamic_str_t* el_dinamici = malloc(sizeof(dynamic_str_t));
        init_str(el_dinamici);
        int offset = startswith(nomi.buf[i], el_fissi.buf[0]);
        if (offset == -1)
            return -1;
        for (int j=1; j<el_fissi.size; j++) {
            int new_offset = -1, size_dinamico = 0;
            char* b = el_fissi.buf[j];
            for (; new_offset == -1; size_dinamico += 1) {
                char* a = nomi.buf[i]+offset+size_dinamico;
                if (!a[0])
                    return -1;
                new_offset = startswith(a, b);
//                 printf("startswith(`%s', `%s') = %d\n", a, b, new_offset);
            }
            char tmp[size_dinamico];
            tmp[0] = tmp[size_dinamico-1] = '\0';
            strncpy(tmp, nomi.buf[i]+offset, size_dinamico-1);
            append_str(el_dinamici, tmp);
            offset += new_offset + size_dinamico - 1;
        }
        if (el_fissi.size == el_keywords.size) {
            if (!(nomi.buf[i]+offset))
                return -1;
            int size = strlen(nomi.buf[i]+offset);
            char tmp[size+1];
            tmp[0] = '\0';
            strcpy(tmp, nomi.buf[i]+offset);
            append_str(el_dinamici, tmp);
        }
        append_obj(dinamici, el_dinamici);
    }
    return keywords.size == dinamici->size ? IS_A_FILE : IS_A_DIR;
}

#ifndef DEBUG
int _main() {
#else
int main() {
#endif
    const char* schema = "%%eyar - %title.%type";
//     const char* path[] = { "/", "/Born to be Abramo (ciao).mp3", "/.directory", NULL };
    dynamic_obj_t fissi, keywords;
    init_obj(&fissi);
    init_obj(&keywords);
    printf("%d --> %d\n", parse_schema(schema, &fissi, &keywords), parse_schema(schema, NULL, NULL));
//     dynamic_obj_t dinamici;
//     int ret;
//     for (int jj=0; path[jj]; jj++) {
//         errprintf("parse_path(`%s') = ", path[jj]);
//         ret = parse_path(path[jj], fissi, keywords, &dinamici);
//         errprintf("`%s'\tdinamici.size = %d\n",
//                 ret == -1 ? "-1" : ret == IS_A_FILE ? "IS_A_FILE" : "IS_A_DIR",
//                 dinamici.size);
//         for (int i=0; i<dinamici.size; i++) {
//             dbgprint_str(*(dynamic_str_t*)fissi.buf[i], "fissi[i]");
//             dbgprint_str(*(dynamic_str_t*)keywords.buf[i], "keywords[i]");
//             dbgprint_str(*(dynamic_str_t*)dinamici.buf[i], "dinamici[i]");
//             errprintf("\n");
//         }
//     }
    return EXIT_SUCCESS;
}
