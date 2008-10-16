#!/usr/bin/env python
# -*- encoding: utf-8 -*-

BLOCK_SIZE = 512

def fill(string, size=BLOCK_SIZE, filler='\0'):
    return string[:size] + ''.join(filler for _ in xrange(len(string), size))

if __name__ == '__main__':
    from sys import argv
    if len(argv) != 3:
        # FIXME: trovare un modo per capire da solo la dimensione, in caso di device
        raise SystemExit("Uso: %s device size\n" % argv[0])
    out = file(argv[1], 'wb')
    out.write(fill('HiXoS_FS'))     # il primo blocco è già stato scritto
    for _ in xrange(BLOCK_SIZE, int(argv[2]), BLOCK_SIZE):
        out.write('\0'*BLOCK_SIZE)
    out.close()
