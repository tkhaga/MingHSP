gcc -c ifdbl.c mhsputil.c -Os
windres ifdbl.rc -o ifdbl.coff
dllwrap --def ifdbl.def -s -mwindows -o ifdbl.spi ifdbl.o mhsputil.o ifdbl.coff -lz -Wl,--enable-stdcall-fixup
