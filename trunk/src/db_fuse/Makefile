CFLAGS = -std=c99 -pedantic -Wall -W -g -D_XOPEN_SOURCE=500 `pkg-config --cflags taglib`

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

LDFLAGS = `pkg-config --libs sqlite3` -ltag_c

parser: ../common/utils.o ../musicmeshfsc/configurazione.o
	$(MAKE) $(CFLAGS) $(LDFLAGS) -DDEBUG ../common/utils.o ../musicmeshfsc/configurazione.o parser.c -o parser

argp: ../common/sqlite.o ../common/utils.o parser.o ../musicmeshfsc/configurazione.o DAMN_C_MACROS.h

../common/sqlite.o:
	$(MAKE) -C ../common sqlite.o

../common/utils.o:
	$(MAKE) -C ../common utils.o

../musicmeshfsc/configurazione.o:
	$(MAKE) -C ../musicmeshfsc configurazione.o

DAMN_C_MACROS.h: mk_DAMN_C_MACROS
	./mk_DAMN_C_MACROS DAMN_C_MACROS.h

mk_DAMN_C_MACROS: ../musicmeshfsc/configurazione.o

clean:
	rm -rf *.o *~ argp mk_DAMN_C_MACROS DAMN_C_MACROS.h
