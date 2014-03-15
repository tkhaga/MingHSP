/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2006 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <stdlib.h>
#include <windows.h>
#include "toutf8.h"

char *toutf8(const char *src)
{
	wchar_t *wstr, *pp;
	static char *nullstr = "";
	unsigned char *utf8str, *p;
	unsigned int numchars, utf8size;

	utf8size = lstrlen(src);
	numchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, utf8size, NULL, 0);

	if (numchars == 0) {
		return nullstr;
	}

	wstr = (wchar_t *)calloc(numchars + 1, sizeof(wchar_t));
	if (!wstr) {
		return nullstr;
	}

	if (numchars != MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, utf8size, wstr, numchars)) {
		free(wstr);
		return nullstr;
	}

	utf8size = 0;
	pp = wstr;
	while (*pp != '\0') {
		if (*pp < 0x0080) {
			utf8size += 1;
		}
		else if (*pp < 0x0800){
			utf8size += 2;
		}
		else {
			utf8size += 3;
		}
		pp++;
	}

	utf8str = (unsigned char *)malloc(utf8size + 1);
	if (!utf8str) {
		free(wstr);
		return nullstr;
	}

	pp = wstr;
	p = utf8str;
	while (*pp != '\0') {
		if (*pp < 0x0080) {
			*p++ = *pp & 0xff;
		}
		else if (*pp < 0x0800){
			*p++ = 0xc0 | (*pp >> 6);
			*p++ = 0x80 | (*pp & 0x3f);
		}
		else {
			*p++ = 0xe0 | (*pp >> 12);
			*p++ = 0x80 | ((*pp >> 6) & 0x3f);
			*p++ = 0x80 | (*pp & 0x3f);
		}
		pp++;
	}
	*p = '\0';

	free(wstr);

	return (char *)utf8str;
}
