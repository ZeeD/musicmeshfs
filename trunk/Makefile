CFLAGS = -std=c99 -pedantic -Wall -W -I/usr/include/taglib -g
LDFLAGS = -ltag_c

all: getta setta

getta: getta.c
setta: setta.c

.PHONY : clean
clean:
	rm -f getta setta *~