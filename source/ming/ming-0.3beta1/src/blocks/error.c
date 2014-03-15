/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* $Id: error.c,v 1.8 2003/11/08 18:24:57 whamann Exp $ */

#include <stdio.h>
#include <stdlib.h> // for exit()
#include <stdarg.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "error.h"

char funcname[32];
char *warnbuf = NULL;
int warnlimit;
int warnsize = 0;
char buf[4096];

void
warn_default(const char *msg, ...)
{
	int funclen;
	va_list args;

	va_start(args, msg);
	vprintf(msg, args);
#ifdef WIN32
  vsprintf(buf, msg, args);
  funclen = lstrlen(funcname);
  warnsize += funclen + lstrlen(buf) + 4;
  if (warnsize < warnlimit && warnbuf != NULL)
  {
    lstrcat(warnbuf, funcname);
    lstrcat(warnbuf, ": ");
    warnbuf += funclen + 2;
    warnbuf += vsprintf(warnbuf, msg, args);
    lstrcat(warnbuf, "\r\n");
    warnbuf += 2;
  }
#else
	vprintf(msg, args);
#endif

	va_end(args);
}


void
error_default(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
#ifdef WIN32
  lstrcpy(buf, "命令 \"");
  if (funcname[0] != '\0')
    lstrcat(buf, funcname);
  lstrcat(buf, "\" 内でエラーが発生しました\n\nエラーメッセージ: ");
  vsprintf(&buf[lstrlen(buf)], msg, args);
  MessageBox(NULL, buf, "MingHSP", MB_OK | MB_ICONSTOP);
#else
	vprintf(msg, args);
#endif
	va_end(args);
	exit(0);
}


void (*SWF_warn)(const char *msg, ...) = warn_default;
void (*SWF_error)(const char *msg, ...) = error_default;


void
setSWFWarnFunction(void (*warnfunc)(const char *msg, ...))
{
	SWF_warn = warnfunc;
}


void
setSWFErrorFunction(void (*errorfunc)(const char *msg, ...))
{
	SWF_error = errorfunc;
}


void
SWF_assert(int c)
{
	if ( !c )
		SWF_error("failed assertion");
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
