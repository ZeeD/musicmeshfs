#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <err.h>    /* warn() */
#include <argp.h>   /* argp_program_version argp_program_bug_address,
        struct argp_option, NULL, error_t, struct argp_state, struct argp,
        argp_parse(), printf() */
#include "../common/sqlite.h"   /* path_is_a_db() */
#include "parser.h" /* parse_schema() */

/* TODO: integrare questo file in Fuse_DB / MusicMeshFS */

const char* argp_program_version = "prova_argp - 1.0";
const char* argp_program_bug_address = "<Vito.DeTullio@gmail.com>";

static struct argp_option options[] = {
    { "quiet", 'q', NULL, 0, "Suppress most of messages (NOT SUPPORTED)", 0 },
    { NULL, 0, NULL, 0, NULL, 0 }   /* terminatore senza warning */
};

struct arguments {
    int quiet;
    char* DATABASE;
    char* PATH;
    char* MOUNT_SCHEMA;
};

static error_t parser(int key, char* arg, struct argp_state* state) {
    struct stat t;
    switch (key) {
        case 'q':
            ((struct arguments*)state->input)->quiet = 1;
            break;
        case ARGP_KEY_ARG:
            switch (state->arg_num) {
                case 0:
                    if (path_is_a_db(arg))
                        ((struct arguments*)state->input)->DATABASE = arg;
                    else {
                        warn("ERROR: `%s' is not a valid db!\n", arg);
                        exit(EXIT_FAILURE);
                    }
                    break;
                case 1:
                    if (stat(arg, &t) == 0 && S_ISDIR(t.st_mode))
                        ((struct arguments*)state->input)->PATH = arg;
                    else {
                        warn("ERROR: `%s' is not a valid path!\n", arg);
                        exit(EXIT_FAILURE);
                    }
                    break;
                case 2:
                    if (parse_schema(arg, NULL, NULL) == 0)
                        ((struct arguments*)state->input)->MOUNT_SCHEMA = arg;
                    else {
                        warn("ERROR: `%s' is not a valid schema!\n", arg);
                        exit(EXIT_FAILURE);
                    }
                    break;
                default:                /* troppi argomenti */
                    argp_usage(state);
            }
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 3)
                argp_usage(state);      /* non abbastanza argomenti */
            break;
        default:
            return ARGP_ERR_UNKNOWN;    /* opzione non riconosciuta */
    }
    return 0;
}

#include "DAMN_C_MACROS.h"

static struct argp argp = {
    options,
    parser,
    "DATABASE PATH MOUNT_SCHEMA",
    DAMN_C_MACROS,
    NULL,
    NULL,
    NULL
};

int main(int argc, char* argv[]) {
    struct arguments arguments = { 0, NULL, NULL, NULL };
    argp_parse(&argp, argc, argv, 0, NULL, &arguments);
    printf("quiet = %s, DATABASE = `%s', PATH = `%s', MOUNT_SCHEMA = `%s'\n",
            arguments.quiet ? "si" : "no", arguments.DATABASE, arguments.PATH,
            arguments.MOUNT_SCHEMA);
    return 0;
}
