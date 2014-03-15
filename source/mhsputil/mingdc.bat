gcc -c mingdc.c mhsputil.c -I../src -Os
dllwrap --def mingdc.def -s -mwindows -o mingdc.dll mingdc.o mhsputil.o -L.. -lming -lz
