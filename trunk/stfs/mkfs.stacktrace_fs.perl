#!/usr/bin/env perl

my $sb = 'stacktrace_fs';
for (my $i=13; $i<512; $i++) {
        $sb .= "\0" ;
}

my $file1 = "pippo\0\0\0\0\0\0\0\0\0\0\0" ;
for(my $i =16;$i<512;$i++) {
        $file1.= "R" ;
}

my $file2 = "pluto\0\0\0\0\0\0\0\0\0\0\0" ;
for(my $i =16;$i<512;$i++) {
        $file2.= "S" ;
}

my $file3 = "topolino\0\0\0\0\0\0\0\0" ;
for(my $i =16;$i<512;$i++) {
        $file3.= "T" ;
}

print $sb.$file1.$file2.$file3 ;
