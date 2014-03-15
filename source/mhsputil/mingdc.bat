gcc -c mingdc.c mhsputil.c readbmp.c readgif.c readpng.c -I../src -Os
dllwrap --def mingdc.def -s -mwindows -o mingdc.dll mingdc.o mhsputil.o readbmp.o readgif.o readpng.o -L.. -lming -lungif -lpng -lz
