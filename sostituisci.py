#!/usr/bin/env python

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
