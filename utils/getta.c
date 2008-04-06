/*
    Copyright © 2008 Vito De Tullio

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>   /* fprintf(), stderr, printf() */
#include <stdlib.h>  /* exit(), EXIT_FAILURE */
#include <tag_c.h>   /* taglib_file_new(), taglib_file_tag(), taglib_tag_year(),
        taglib_tag_comment(), taglib_tag_free_strings(), taglib_tag_track(),
        taglib_tag_genre(), taglib_file_free(), taglib_tag_title(),
        taglib_tag_album(), taglib_tag_artist() */

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
