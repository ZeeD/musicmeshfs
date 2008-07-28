#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef DEBUG
void dbg_printf(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "DBG: ");
    vfprintf(stderr, fmt, ap);
}
#endif

typedef struct {
    const char* name;
    const char* value;
} config_t;

typedef struct {
    unsigned size;
    config_t* configs;
} configs_t;

char* append(char* s, char c) {
    int size = strlen(s);
    char* s1 = realloc(s, size+1);
    s1[size] = c;
    return s1;
}

unsigned parse(FILE* stream, configs_t* configs) {
    char c = '\0', *key = NULL, *value = NULL;
    int left = 1, singoli_apici = 0, doppi_apici = 0;
    while (!feof(stream)) {
        c = fgetc(stream);
        switch (c) {
            case '\n':
                if (doppi_apici || singoli_apici || !left)
                    return -1;
            case '#':       /* ignora i # commenti fino all'a-capo o a EOF  */
#ifdef DEBUG
                dbg_printf("Sono in un commento\n");
#endif
                while (c != '\n' && c != EOF)
                    c = fgetc(stream);
#ifdef DEBUG
                dbg_printf("Fine del commento\n");
#endif
                left = 1;
                doppi_apici = 0;
                singoli_apici = 0;
                break;
            case ' ':                   /* ignora gli spazi non protetti da */
            case '\t':                  /* apici singoli o doppi            */
                if (!singoli_apici || !doppi_apici)
                    c = fgetc(stream);
                else {
                    if (left)
                        key = append(key, c);
                    else
                        value = append(value, c);
                }
                break;
            case '=':
                left = 0;
                break;
            case '"':
                if (!singoli_apici)     /* inizia una stringa protetta da   */
                    doppi_apici = 1;    /* doppi apici                      */
                else                    /* ho finito una stringa protetta da*/
                    if (left) {         /* doppi apici: la assegno a key    */
//                         key = append(key, '"');
                        left = 0;
                    }
                    else {              /* o a value?                       */
//                         value = append(value, '"'); /* ho left = 1 con '\n' */
                    }
                break;
            case '\'':
                if (!doppi_apici)       /* inizia una stringa protetta da   */
                    singoli_apici = 1;  /* singoli apici                    */
                else                    /* ho finito una stringa protetta da*/
                    if (left) {         /* singoli apici: la assegnoa a key */
//                         key = append(key, '\'');
                        left = 0;
                    }
                    else {              /* o a value?                       */
//                         value = append(value, '\'');/* ho left = 1 con '\n' */
                    }
                break;
        }
    }
    return 0;
}

void print(configs_t configs) {
    printf("size = %d, dict = {\n", configs.size);
    config_t config;
    for (unsigned i=0; i<configs.size; i++) {
        config = configs.configs[i];
        printf("\t`%s': \t `%s'\n", config.name, config.value);
    }
    printf("}\n");
}

void set(const char* name, const char* value, configs_t* configs) {
    config_t* config = malloc(sizeof(config_t));
    config->name = name;
    config->value = value;
    configs->configs = realloc(configs->configs, ++(configs->size));
    configs->configs[configs->size-1] = *config;
}

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, ap);
    exit(EXIT_FAILURE);
}

#ifdef DEBUG
int main(int argc, char* argv[]) {
    FILE* stream;
    if (argc > 1)
        stream = fopen(argv[1], "r");
    else
        stream = stdin;
    if (stream) {
        configs_t configs = { 0, NULL };
        if (parse(stream, &configs))
            print(configs);
        else
            error("parse() == 0\n");
    }
    else
        error("stream == 0\n");
}
#endif
