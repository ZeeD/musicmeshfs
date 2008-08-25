CFLAGS = -std=c99 -pedantic -Wall -W -g -D_XOPEN_SOURCE=500 `pkg-config --cflags taglib`
LDFLAGS = `pkg-config --libs sqlite3` -ltag_c

parser: ../common/utils.o ../musicmeshfsc/configurazione.o
	$(MAKE) $(CFLAGS) $(LDFLAGS) -DDEBUG ../common/utils.o ../musicmeshfsc/configurazione.o parser.c -o parser

argp: ../common/sqlite.o ../common/utils.o parser.o ../musicmeshfsc/configurazione.o

../common/sqlite.o:
	$(MAKE) -C ../common sqlite.o

../common/utils.o:
	$(MAKE) -C ../common utils.o

../musicmeshfsc/configurazione.o:
	$(MAKE) -C ../musicmeshfsc configurazione.o

clean:
	rm -rf *.o *~ argp
