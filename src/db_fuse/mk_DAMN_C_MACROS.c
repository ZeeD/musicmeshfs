#include "constants.h"
#include <stdio.h>

const char* headers[] = {
    "DATABASE è il nome del database dei metadati usare",
    "PATH è il nome della directory ove verrà montato il file system",
    "MOUNT_SCHEMA è una stringa del tipo `KEYWORD[/KEYWORDS|SEPARATORS]'",
    "Le keywords accettate sono:",
    NULL
};

int main(int argc, char* argv[]) {
    FILE* stream;
    if (argc > 1)
        stream = fopen(argv[1], "w");
    else
        stream = stdout;
    fprintf(stream, "/* this file is auto-generated, don't modify it! */\n");
    fprintf(stream, "/* instead, use %s! */\n\n", argv[0]);
    fprintf(stream, "#define DAMN_C_MACROS ");
    for (unsigned i=0; headers[i]; i++)
        fprintf(stream, "\"\t%s\\n\"\\\n", headers[i]);
    for (unsigned i=0; i<KEYWORDS_SIZE; i++)
        fprintf(stream, "\"\t* %%%s\\n\"\\\n", KEYWORDS[i]);
    fprintf(stream, "\nstatic const char ISO_C_forbids_an_empty_source_file = '\\0';\n");
}
