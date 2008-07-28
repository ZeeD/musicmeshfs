CFLAGS = -std=c99 -pedantic -Wall -W -g

parser: parser.c
debug: parser.c
	$(CC) $(CFLAGS) -DDEBUG parser.c -o parser
clean:
	rm -f *~ parser *.yy.c
