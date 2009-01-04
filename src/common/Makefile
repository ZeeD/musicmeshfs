CFLAGS = $(DEBUGFLAGS) -std=c99 -pedantic -Wall -W -g -D_XOPEN_SOURCE=500 \
    `pkg-config --cflags taglib`

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

.PHONY : clean

schema.h: schema.sql
	echo "/* Warning: this file is auto-generated (from \`$?')! */" > schema.h
	xxd -i < schema.sql >> schema.h
	echo ', 0' >> schema.h
	echo "/* Warning: this file is auto-generated (from \`$?')! */" >> schema.h

sqlite.o: schema.h sqlite.c

test_sqlite: utils.o
	$(MAKE) "DEBUGFLAGS=-D__DEBUG_SQLITE_C__" sqlite.o
	$(CC) $(CFLAGS) -lsqlite3 -ltag_c -o $@ $? sqlite.o

clean:
	rm -rf *.o *~ schema.h test_sqlite
