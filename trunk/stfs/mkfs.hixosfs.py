#!/usr/bin/env python
# -*- encoding: utf-8 -*-

BLOCK_SIZE = 512
MAX_FILENAME = 12

def fill(string, size=BLOCK_SIZE, filler='\0'):
    return string[:size] + ''.join(filler for _ in xrange(len(string), size))

if __name__ == '__main__':
    from sys import argv
    l = len(argv)
    if l < 3 or not l % 2:
        # FIXME: trovare un modo per capire da solo la dimensione, in caso di device
        raise SystemExit("Uso: %s device size [nomefile contenuto]*\n" % argv[0])
    size = int(argv[2])
    if size*2 < l-3:
        raise SystemExit("Non puoi creare un fs di %d blocchi e ficcarci dentro %d blocchi (+ 1 per il superblock)\n" % (size, l-1))
    out = file(argv[1], 'wb')
    out.write(fill('HiXoS_FS'))     # il primo blocco è già stato scritto
    for i, _ in enumerate(xrange(BLOCK_SIZE, size*BLOCK_SIZE, BLOCK_SIZE)):
        if (i+2)*2 < l:
            out.write(fill(argv[(i+2)*2-1], MAX_FILENAME))
            out.write(fill(argv[(i+2)*2], BLOCK_SIZE-MAX_FILENAME))
        else:
            out.write('\0'*BLOCK_SIZE)
    out.close()
