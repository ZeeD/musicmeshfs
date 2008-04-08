#include "constants.h"

const char* TABLES[] = { "artista", "musica", "file" };
const int TABLES_SIZE = 3;

const char* KEYWORDS[] = { "artist", "title", "album", "track", "genre", "year",
        "host", "path", "type" };
const char* COLUMNS[] = { "artista.nome_artista", "musica.titolo",
        "musica.nome_album", "musica.traccia", "musica.genere",
        "musica.lavoro_anno", "file.host", "file.path", "file.formato" };
const int KEYWORDS_SIZE = 9;

const char* COLUMN_EXT = "file.formato";

const char* JOINS[] = { "artista,musica", "musica,file" }; // senza spazi!!!
const char* WHERE[] = {
    "(artista.nome_artista = musica.artista_nome_artista)",
    "(musica.titolo = file.musica_titolo) AND (musica.nome_album =\
            file.musica_nome_album) AND (musica.traccia = file.musica_traccia)"
};

const int JOINS_SIZE = 2;

// *********************** UTILITÀ ***********************

/**
    Funzione di utilità che vede KEYWORDS e COLUMNS come un dizionario
    \param keyword elemento di KEYWORDS
    \return valore di COLUMNS corrispondente a keyword, NULL in caso d'errore
    \sa value_from_key(), where_from_join()
*/
const char* column_from_keyword(const char* keyword) {
    for (int i=0; i<KEYWORDS_SIZE; i++)
        if (!strcmp(KEYWORDS[i], keyword))
            return COLUMNS[i];
    return NULL;
}

/**
    Funzione di utilità che verifica se, e dove, un certo elemento è presente
    in un char*[], di dimensione nota
    \return pos(element) se element \f$ \in \f$ array,
            -1 altrimenti
    \sa index_of_str(), contains_str()
*/
int index_of(const char* array[], const int size, char* element) {
    for (int i=0; i<size; i++)
        if (!strcmp(element, array[i]))
            return i;
    return -1;
}

/**
    Funzione di utilità che vede JOINS e WHERE come un dizionario
    \param join elemento di JOINS
    \return valore di WHERE corrispondente a join, NULL in caso d'errore
    \sa value_from_key(), column_from_keyword()
*/
const char* where_from_join(const char* join) {
    for (int i=0; i<JOINS_SIZE; i++)
        if (!strcmp(JOINS[i], join))
            return WHERE[i];
    return NULL;
}
