#!/usr/bin/env python

from sys import stdout

def fill(string, size, filler='\0'):
    return string[:size] + ''.join(filler for _ in xrange(len(string), size))

stdout.write(fill('HiXoS_FS', 512))

stdout.write(fill(fill('pippo', 16), 512, 'R'))
stdout.write(fill(fill('pluto', 16), 512, 'S'))
stdout.write(fill(fill('topolino', 16), 512, 'T'))
