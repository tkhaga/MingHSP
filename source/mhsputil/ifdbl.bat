gcc -c ifdbl.c mhsputil.c -Os
dllwrap --def ifdbl.def -s -o ifdbl.spi ifdbl.o mhsputil.o -lz
