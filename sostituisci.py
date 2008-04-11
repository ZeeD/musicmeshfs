#!/usr/bin/env python
# -*- encoding: utf-8 -*-
#   Copyright © 2008 Vito De Tullio
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.


# incredibile! funziona!

mark = '"TRANSLATE_ME_PLEASE"'
varname = "schema"

def chunks(string, size=4096):
    if not size:
        raise ValueError("size cannot be zero")
    l = len(string)
    i = 0
    ret = []
    while i < l:
        ret.append(string[i:i+size])
        i += size
    return ret

def flat(couples):
    r = []
    for a,b in couples:
        r.append(a)
        r.append(b)
    return tuple(r)

if __name__ == '__main__':
    from sys import argv

    in_, out = open(argv[1], "r"), open(argv[2], "w")
    query_str = open(argv[3], "r").read()

    for line in in_:
        out.write(line.replace(mark, 'calloc(%d, 1); \nstrcat(%s, "' %
                        (len(query_str)+1, varname)
                + '");\nstrcat(%s+%d, "'.join(s.replace("\n",
                                "\\n\\\n").replace("\"", "\\\"")
                        for s in chunks(query_str, 4000)) % flat((varname, i)
                                for i in xrange(4000, len(query_str), 4000))
                + '")'))
    out.close()
