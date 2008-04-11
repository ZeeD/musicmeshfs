#-D_XOPEN_SOURCE server per utils.h

CFLAGS = -std=c99 -pedantic -Wall -W -g `pkg-config --cflags taglib fuse` -D_XOPEN_SOURCE=500
LDFLAGS = `pkg-config --libs fuse sqlite3` -ltag_c

.PHONY : clean

musicmeshfsc: ../common/utils.o ../db_fuse/parser.o ../db_fuse/db_fuse.o\
 ../db_fuse/constants.o ../db_fuse/db_fuse_utils.o db_fuse_variant.o\
 ../common/sqlite.o configurazione.o
	$(CC) $(CFLAGS) $(@:=.c) $? $(LDFLAGS) -o ../../bin/$@

../common/sqlite.c: ../common/sqlite.pre_c ../common/schema.sql
	../../sostituisci.py ../common/sqlite.pre_c ../common/sqlite.c\
 ../common/schema.sql

clean:
	rm -rf *.o *~ ../../bin/musicmeshfsc
	$(MAKE) -C ../common clean
	$(MAKE) -C ../db_fuse clean
