#-D_XOPEN_SOURCE server per utils.h

CFLAGS = -std=c99 -pedantic -Wall -W -g -I/usr/include/taglib -D_XOPEN_SOURCE=500
LDFLAGS = -lsqlite3 -ltag_c

.PHONY : clean

musicmeshfsd: commons.o inotify.o ../common/sqlite.o ../common/utils.o
	$(CC) $(CFLAGS) $(@:=.c) $? $(LDFLAGS) -o ../../bin/$@

../common/sqlite.c: ../common/sqlite.pre_c ../common/schema.sql
	../../sostituisci.py ../common/sqlite.pre_c ../common/sqlite.c ../common/schema.sql

clean:
	$(MAKE) -C ../common clean
	rm -rf *.o *~ ../../bin/musicmeshfsd
