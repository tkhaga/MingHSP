gcc -c ifdbl.c mhsputil.c -Os
dllwrap --def ifdbl.def -s -mwindows -o ifdbl.spi ifdbl.o mhsputil.o -lz -Wl,--enable-stdcall-fixup
