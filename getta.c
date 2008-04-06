#include <stdio.h>   /* fprintf(), stderr, printf() */
#include <stdlib.h>  /* exit(), EXIT_FAILURE */
#include <tag_c.h>   /* taglib_file_new(), taglib_file_tag(), taglib_tag_year(),
        taglib_tag_comment(), taglib_tag_free_strings(), taglib_tag_track(),
        taglib_tag_genre(), taglib_file_free(), taglib_tag_title(),
        taglib_tag_album(), taglib_tag_artist() */

// cc -I/usr/include/taglib -ltag_c -o getta getta.c -W -Wall -std=c99 -pedantic

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "USO: `%s' [FILES]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    for (int i=1; i<argc; i++) {
        TagLib_File* tlf = taglib_file_new(argv[i]);
        if (!tlf) {
            fprintf(stderr, "WARN: `%s' non è supportato\n", argv[i]);
            continue;
        }
        TagLib_Tag* tlt = taglib_file_tag(tlf);
        printf("---[`%s']---\n" "artista  = `%s'\n" "anno     = %d\n"
                "album    = `%s'\n" "traccia  = %d\n" "titolo   = `%s'\n"
                "genere   = `%s'\n" "commento = `%s'\n" "\n", argv[i],
                taglib_tag_artist(tlt), taglib_tag_year(tlt),
                taglib_tag_album(tlt), taglib_tag_track(tlt),
                taglib_tag_title(tlt), taglib_tag_genre(tlt),
                taglib_tag_comment(tlt));
        taglib_tag_free_strings();
        taglib_file_free(tlf);
    }
}
