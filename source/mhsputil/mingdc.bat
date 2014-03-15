gcc -c mingdc.c mhsputil.c -I../src -Os
dllwrap --def mingdc.def -s -o mingdc.dll mingdc.o mhsputil.o -lming -lz


