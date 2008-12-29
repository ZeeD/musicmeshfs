CFLAGS = -std=c99 -pedantic -Wall -W -g `pkg-config --cflags taglib fuse` -D_XOPEN_SOURCE=500

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

LDFLAGS = `pkg-config --libs fuse sqlite3` -ltag_c

.PHONY : clean

musicmeshfsc: ../common/utils.o ../db_fuse/parser.o ../db_fuse/db_fuse.o\
 ../db_fuse/constants.o ../db_fuse/db_fuse_utils.o db_fuse_variant.o\
 ../common/sqlite.o configurazione.o
	$(CC) $(CFLAGS) $(@:=.c) $? $(LDFLAGS) -o ../../bin/$@

clean:
	rm -rf *.o *~ ../../bin/musicmeshfsc
	$(MAKE) -C ../common clean
	$(MAKE) -C ../db_fuse clean
