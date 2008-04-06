#include <stdio.h>   /* fprintf(), stderr */
#include <stdlib.h>  /* exit(), EXIT_FAILURE, atoi() */
#include <string.h>  /* strcmp() */
#include <tag_c.h>   /* taglib_file_new(), taglib_file_tag(),
        taglib_tag_set_year(), taglib_file_save(), taglib_tag_set_comment(),
        taglib_tag_free_strings(), taglib_tag_set_track(),
        taglib_tag_set_genre(), taglib_file_free(), taglib_tag_set_title(),
        taglib_tag_set_album(), taglib_tag_set_artist() */

// cc -I/usr/include/taglib -ltag_c -o setta setta.c -W -Wall -std=c99 -pedantic

int main(int argc, char* argv[]) {
    if (argc % 2) { // NB: '%', non '<'!
        fprintf(stderr, "USO: `%s' FILE [KEYWORD VALUE]*\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    TagLib_File* tlf = taglib_file_new(argv[1]);
    if (!tlf) {
        fprintf(stderr, "ERROR: `%s' non è supportato\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    TagLib_Tag* tlt = taglib_file_tag(tlf);
    for (int i=2; i<argc; i+=2) {
        if (!strcmp(argv[i], "artista"))
            taglib_tag_set_artist(tlt, argv[i+1]);
        else if (!strcmp(argv[i], "anno"))
            taglib_tag_set_year(tlt, atoi(argv[i+1]));
        else if (!strcmp(argv[i], "album"))
            taglib_tag_set_album(tlt, argv[i+1]);
        else if (!strcmp(argv[i], "traccia"))
            taglib_tag_set_track(tlt, atoi(argv[i+1]));
        else if (!strcmp(argv[i], "titolo"))
            taglib_tag_set_title(tlt, argv[i+1]);
        else if (!strcmp(argv[i], "genere"))
            taglib_tag_set_comment(tlt, argv[i+1]);
        else if (!strcmp(argv[i], "commento"))
            taglib_tag_set_genre(tlt, argv[i+1]);
        else {
            fprintf(stderr, "WARN: `%s' non è una keyword supportata\n", argv[i]);
            continue;
        }
    }
    taglib_file_save(tlf);
    taglib_file_free(tlf);
    taglib_tag_free_strings();
}
