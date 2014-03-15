/*
 * Copyright (C) 2001 Peter Harris <peter.harris@hummingbird.com>
 * Copyright (C) 2001 Edmund Grimley Evans <edmundo@rano.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Convert a string between UTF-8 and the locale's charset.
 */

#include <stdlib.h>
#include <string.h>

#include "utf8.h"


#ifdef _WIN32

	/* Thanks to Peter Harris <peter.harris@hummingbird.com> for this win32
	 * code.
	 */

#include <stdio.h>
#include <windows.h>

static unsigned char *make_utf8_string(const wchar_t *unicode)
{
    int size = 0, index = 0, out_index = 0;
    unsigned char *out;
    unsigned short c;

    /* first calculate the size of the target string */
    c = unicode[index++];
    while(c) {
        if(c < 0x0080) {
            size += 1;
        } else if(c < 0x0800) {
            size += 2;
        } else {
            size += 3;
        }
        c = unicode[index++];
    }	

    out = malloc(size + 1);
    if (out == NULL)
        return NULL;
    index = 0;

    c = unicode[index++];
    while(c)
    {
        if(c < 0x080) {
            out[out_index++] = (unsigned char)c;
        } else if(c < 0x800) {
            out[out_index++] = 0xc0 | (c >> 6);
            out[out_index++] = 0x80 | (c & 0x3f);
        } else {
            out[out_index++] = 0xe0 | (c >> 12);
            out[out_index++] = 0x80 | ((c >> 6) & 0x3f);
            out[out_index++] = 0x80 | (c & 0x3f);
        }
        c = unicode[index++];
    }
    out[out_index] = 0x00;

    return out;
}

int utf8_encode(const char *from, char **to)
{
	wchar_t *unicode;
	int wchars, err;

	wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from,
			strlen(from), NULL, 0);

	if(wchars == 0)
	{
//		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}

	unicode = calloc(wchars + 1, sizeof(unsigned short));
	if(unicode == NULL) 
	{
//		fprintf(stderr, "Out of memory processing string to UTF8\n");
		return -1;
	}

	err = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from, 
			strlen(from), unicode, wchars);
	if(err != wchars)
	{
		free(unicode);
//		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}

	/* On NT-based windows systems, we could use WideCharToMultiByte(), but 
	 * MS doesn't actually have a consistent API across win32.
	 */
	*to = make_utf8_string(unicode);

	free(unicode);
	return 0;
}
#endif