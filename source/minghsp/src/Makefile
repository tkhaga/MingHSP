include ../Makefile.config

OFILES = \
	blocklist.o \
	displaylist.o \
	fill.o \
	font_util.o \
	ming.o \
	movie.o \
	movieclip.o \
	position.o \
	shape_cubic.o \
	shape_util.o \
	text_util.o \
	gc.o \
	pdr.o \
	loadaction.o

COMPILEROFILES = actioncompiler/compile.o \
		 actioncompiler/swf4compiler.tab.o \
		 actioncompiler/lex.swf4.o \
		 actioncompiler/swf5compiler.tab.o \
		 actioncompiler/lex.swf5.o \
		 actioncompiler/compileaction.o \
		 actioncompiler/assembler.o

MHSP = minghsp.o toutf8.o formats.o mhspobj.o mhsputil.o mhsperror.o

TARGET = minghsp.hpi

all: full aedition bedition light

# saveFP and restFP on OS X can be retrieved as such:
# ar -x /usr/lib/libcc_dynamic.a lib/macosx-fpsave.o

CFLAGS += -Os -DPDR_USE_SWFINPUT

full: $(OFILES) $(MHSP)
	rm -f blocks/bitmap.o
	rm -f mhspaction.o
	cd blocks && $(MAKE) CFLAGS="$(CFLAGS) -DUSE_PNG=1 -DUSE_GIF=1 -DUSE_BMP=1 -I.."
	cd actioncompiler && $(MAKE)
	ar r libming.a blocks/*.o $(OFILES) $(COMPILEROFILES)
	$(CC) -c mhspaction.c $(CFLAGS)
	windres minghsp.rc -o minghsp.coff
	dllwrap --def minghsp.def -s -mwindows -o minghsp.hpi mhspaction.o $(MHSP) minghsp.coff libming.a -lungif -lpng -lz

aedition: $(OFILES) $(MHSP)
	rm -f blocks/bitmap.o
	rm -f mhspaction.o
	cd blocks && $(MAKE) CFLAGS="$(CFLAGS) -DUSE_PNG=0 -DUSE_GIF=0 -DUSE_BMP=0 -I.."
	cd actioncompiler && $(MAKE)
	ar r libming.a blocks/*.o $(OFILES) $(COMPILEROFILES)
	$(CC) -c mhspaction.c $(CFLAGS)
	windres minghsp.rc -o minghsp.coff
	dllwrap --def minghsp.def -s -mwindows -o minghspa.hpi mhspaction.o $(MHSP) minghsp.coff libming.a -lz

bedition: $(OFILES) $(MHSP)
	rm -f blocks/bitmap.o
	rm -f mhspaction.o
	cd blocks && $(MAKE) CFLAGS="$(CFLAGS) -DUSE_PNG=1 -DUSE_GIF=1 -DUSE_BMP=1 -I.."
	cd actioncompiler && $(MAKE)
	ar r libming.a blocks/*.o $(OFILES) $(COMPILEROFILES)
	$(CC) -c mhspaction.c $(CFLAGS) -DNOACTION
	windres minghsp.rc -o minghsp.coff
	dllwrap --def minghsp.def -s -mwindows -o minghspb.hpi mhspaction.o $(MHSP) minghsp.coff libming.a -lungif -lpng -lz

light: $(OFILES) $(MHSP)
	rm -f blocks/bitmap.o
	rm -f mhspaction.o
	cd blocks && $(MAKE) CFLAGS="$(CFLAGS) -DUSE_PNG=0 -DUSE_GIF=0 -DUSE_BMP=0 -I.."
	cd actioncompiler && $(MAKE)
	ar r libming.a blocks/*.o $(OFILES) $(COMPILEROFILES)
	$(CC) -c mhspaction.c $(CFLAGS) -DNOACTION
	windres minghsp.rc -o minghsp.coff
	dllwrap --def minghsp.def -s -mwindows -o minghspl.hpi mhspaction.o $(MHSP) minghsp.coff libming.a -lz

dynamic: $(SHAREDLIB)

$(SHAREDLIB): $(OFILES)
	cd blocks && $(MAKE) CFLAGS="$(CFLAGS) -I.."
	cd actioncompiler && $(MAKE) CFLAGS="$(CFLAGS) -I.."
	$(CC) $(OFILES) $(COMPILEROFILES) $(LDFLAGS) $(EXTRA_LIBS) blocks/*.o $(SHLIBLDFLAGS) -o $(SHAREDLIB)
	cp $(SHAREDLIB) ..
	ln -fs $(SHAREDLIB) ../libming$(NOVAR_SHLIBEXT)
	ln -fs $(SHAREDLIB) ../libming$(SHORT_SHLIBEXT)

static: $(OFILES)
	cd blocks && $(MAKE)
	cd actioncompiler && $(MAKE)
	ar r libming.a blocks/*.o ${OFILES} ${COMPILEROFILES}
	ranlib libming.a  # bsd requires this?
	cp libming.a ..

libming.a: $(OFILES)
	cd blocks && $(MAKE)
	cd actioncompiler && $(MAKE)
	ar r libming.a blocks/*.o ${OFILES} ${COMPILEROFILES}
	ranlib libming.a  # bsd requires this?
	cp libming.a ..

clean:
	cd blocks && $(MAKE) clean
	cd actioncompiler && $(MAKE) clean
	rm -f *.o libming.a $(SHAREDLIB) libming$(NOVAR_SHLIBEXT) test test.exe core *.core

# breaks on bsd:
#.c.o: .c .h
#	${CC} ${FLAGS} -c $<

test: static test.o
	${CC} -o test test.o libming.a  ${EXTRA_LIBS} ${LDFLAGS}
