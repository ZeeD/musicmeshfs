%%
([:alpha:][:alnum:]*)[:space:]*=[:space:]*([:alpha:][:alnum:]*)
%%
    #include <stdlib.h> /* realloc(), NULL */
    #include <stdio.h>  /* printf(), stdin, fopen() */

    typedef struct {
        const char* name;
        const char* value;
    } couple;

    void set(const char* name, const char* value, couple* configs) {
        couple* config = realloc(NULL, sizeof(couple));
        config->name = name;
        config->value = value;
        configs->dict = realloc(configs.dict, ++configs->size);
        configs->dict[configs->size-1] = couple;
    }

    void print(couple configs) {
        printf("size = %d, dict = {\n", configs.size);
        for (unsigned i=0; i<configs.size; i++)
            printf("\t`%s': \t `%s'\n", configs.dict[i].name, configs.dict[i].value);
        printf("}\n");
    }

    int main(unsigned argc, char* argv[]) {
        argv += 1;
        argc -= 1;
        if (argc)
            yyin = fopen(argv[0], "r");
        else
            yyin = stdin;
        struct {
            unsigned size;
            couple dict[]
        } configs;
        configs.size = 0;
        configs.dict = Null;
        yylex();
        print(configs);
    }
