CFLAGS = -std=c99 -pedantic -Wall -W -g

CFLAGS += -fno-common -Wall -Wextra -Wformat=2  \
    -Winit-self -Winline -Wpacked -Wp,-D_FORTIFY_SOURCE=2 -Wpointer-arith     \
    -Wlarger-than-65500 -Wmissing-declarations -Wmissing-format-attribute     \
    -Wmissing-noreturn -Wmissing-prototypes  -Wnested-externs                 \
    -Wold-style-definition -Wredundant-decls -Wsign-compare                   \
    -Wstrict-aliasing=2 -Wstrict-prototypes -Wswitch-enum -Wundef             \
    -Wunreachable-code -Wunsafe-loop-optimizations -Wwrite-strings

CFLAGS += -Wbad-function-cast -Werror-implicit-function-declaration           \
    -Wfloat-equal -Wmissing-field-initializers -Wmissing-include-dirs         \
    -Wswitch-default

parser: parser.c
debug: parser.c
	$(CC) $(CFLAGS) -DDEBUG parser.c -o parser
clean:
	rm -f *~ parser *.yy.c
