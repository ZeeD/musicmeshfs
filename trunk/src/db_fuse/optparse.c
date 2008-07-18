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

#include "optparse.h"

/**
    equivalente c del costruttore della classe python optparse.OptionParser
    \return oggetto inizializzato con l'help generato da constants.h
    \sa constants.h
*/
option_parser_t init_option_parser() {
    option_parser_t option_parser;
    return option_parser;
}

/**
    equivalente c del metodo optparse.OptionParser.addoption (per gli int)
    \param option_parser collezione di opzioni (modificato in-place)
    \param short_param parametro corto: '-' , un carattere, un int (es '-c 1')
    \param long_param parametro lungo: '--', una stringa, un int (es '--ciao 1')
    \param help messaggio di aiuto
    \param default_value valore di default
    \return 0 se non ci sono stati problemi, -1 altrimenti
    \sa constants.h
 */
int add_option_int(option_parser_t* option_parser, char* short_param,
        char* long_param, char* help, int default_value) {
    return 0;
}

/**
    equivalente c del metodo optparse.OptionParser.addoption (per i booleani)
    \param option_parser collezione di opzioni (modificato in-place)
    \param short_param parametro corto: '-' , un carattere (es '-c')
    \param long_param parametro lungo: '--', una stringa (es '--ciao')
    \param help messaggio di aiuto
    \param default_value valore di default
    \return 0 se non ci sono stati problemi, -1 altrimenti
    \sa constants.h
 */
int add_option_bool(option_parser_t* option_parser, char* short_param,
        char* long_param, char* help, int default_value) {
    return 0;
}

/**
    equivalente c del metodo optparse.OptionParser.addoption (per le stringhe)
    \param option_parser collezione di opzioni (modificato in-place)
    \param short_param parametro corto: '-' , un carattere, una stringa (es '-c mondo')
    \param long_param parametro lungo: '--', una stringa, una stringa (es '--ciao mondo')
    \param help messaggio di aiuto
    \param default_value valore di default
    \return 0 se non ci sono stati problemi, -1 altrimenti
    \sa constants.h
 */
int add_option_str(option_parser_t* option_parser, char* short_param,
        char* long_param, char* help, char* default_value) {
    return 0;
}

/**
    avvio dell'analisi della linea di comando
    \param option_parser collezione di opzioni (modificato in-place)
    \param argc argc del main (modificato in-place)
    \param argv argv del main (modificato in-place)
    \return 0 se non ci sono stati problemi, -1 altrimenti
    \note in argc/argv rimangono gli argomenti "non opzioni"
*/
int parse_args(option_parser_t* option_parser, int* argc, char** argv[]) {
    return argc;
}
