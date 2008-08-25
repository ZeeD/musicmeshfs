CFLAGS = -std=c99 -pedantic -Wall -W -g -D_XOPEN_SOURCE=500 `pkg-config --cflags taglib`

.PHONY : clean

sqlite.c: sqlite.pre_c schema.sql
	../../sostituisci.py sqlite.pre_c sqlite.c schema.sql

sqlite.o: sqlite.c utils.o

clean:
	rm -rf *.o *~ sqlite.c
