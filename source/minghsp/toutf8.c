/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2006 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <stdlib.h>
#include <windows.h>
#include "toutf8.h"

unsigned char *toutf8(const char *src)
{
	wchar_t *wstr, *pp;
	unsigned char *utf8str, *p;
	unsigned int numchars, utf8size;

	utf8size = lstrlen(src);
	numchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, utf8size, NULL, 0);

	if (!numchars)
		return NULL;

	if (!(wstr = calloc(numchars + 1, 2)))
		return NULL;

	if (numchars != MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, utf8size, wstr, numchars)) {
		free(wstr);
		return NULL;
	}

	pp = wstr;
	while (*pp) {
		if (*pp >= 0x0800) {
			utf8size += 3;
		}
		else if (*pp >= 0x0080) {
			utf8size += 2;
		}
		else {
			utf8size += 1;
		}
		*pp++;
	}

	if (!(utf8str = calloc(utf8size + 1, 1)))
		return NULL;

	pp = wstr;
	p = utf8str;
	while (*pp) {
		if (*pp >= 0x0800) {
			*p++ = 0xe0 | (*pp >> 12);
			*p++ = 0x80 | ((*pp >> 6) & 0x3f);
			*p++ = 0x80 | (*pp & 0x3f);
		}
		else if (*pp >= 0x0080) {
			*p++ = 0xc0 | (*pp >> 6);
			*p++ = 0x80 | (*pp & 0x3f);
		}
		else {
			*p++ = *pp & 0xff;
		}
		*pp++;
	}

	free(wstr);

	return utf8str;
}
