#include <argp.h>   /* argp_program_version argp_program_bug_address,
        struct argp_option, NULL, error_t, struct argp_state, struct argp,
        argp_parse(), printf() */

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
    switch (key) {
        case 'q':
            ((struct arguments*)state->input)->quiet = 1;
            break;
        case ARGP_KEY_ARG:
            switch (state->arg_num) {
                case 0: /* TODO: verificare che DATABASE sia un db di sqlite3 */
                    ((struct arguments*)state->input)->DATABASE = arg;
                    break;
                case 1: /* TODO: verificare che PATH sia una directory */
                    ((struct arguments*)state->input)->PATH = arg;
                    break;
                case 2: /* TODO: verificare che MOUNT_SCHEMA sia scritto bene */
                    ((struct arguments*)state->input)->MOUNT_SCHEMA = arg;
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

static struct argp argp = {
    options,
    parser,
    "DATABASE PATH MOUNT_SCHEMA",
    "MOUNT_SCHEMA è una stringa del tipo `KEYWORD[/KEYWORDS_O_SEPARATORI]'\n"
    "Le keywords accettate sono:\n"
    "%artist, %title, %album, %track, %genre, "
    "%year, %host, %path, %type, %filename\n\n"
    "Es: `%artist/%year - %album/%track - %title.%type'",
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
