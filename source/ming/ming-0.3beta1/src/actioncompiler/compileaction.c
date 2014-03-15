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

#ifndef WIN32
	#include <unistd.h>
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "libming.h"
#include "blocks/output.h"
#include "blocks/action.h"
#include "compile.h"
#include "action.h"

extern char* ErrorString;
extern char* WarnString;

extern void (*SWF_error)(const char *msg, ...);
extern void (*SWF_warn)(const char *msg, ...);

void mhsp_error(const char*msg,...);
void mhsp_warn(const char *msg,...);

char* strmirror(char** form,const char* str)
{
    *form = (char*)realloc(*form,strlen(str) + 1);
    if(*form == NULL) return NULL;

    strcpy(*form,str);
    return *form;
}

char* strplus(char** form,const char* latt)
{
    *form = (char*)realloc(*form,strlen(*form) + strlen(latt) + 1);
    if(*form == NULL) return NULL;
    strcat(*form,latt);

    return *form;
}


void print_error(const char*format,...)
{
    char buf[1024];
    int len;
    va_list arglist;
    va_start(arglist, format);
    vsprintf(buf, format, arglist);
    va_end(arglist);

    len = strlen(buf);
    while(len>0 && buf[len-1]=='\n') len--;
    buf[len] = '\n';
    buf[len+1] = 0;
	ErrorString = strplus(&ErrorString , buf);
    fprintf(stderr, "error: %s", buf);
}

void print_warn(const char*format,...)
{
    char buf[1024];
    int len;
    va_list arglist;
    va_start(arglist, format);
    vsprintf(buf, format, arglist);
    va_end(arglist);

    len = strlen(buf);
    while(len>0 && buf[len-1]=='\n') len--;
    buf[len] = '\n';
    buf[len+1] = 0;
	WarnString = strplus(&WarnString , buf);
    printf("%s", buf);
    fprintf(stderr, "warning: %s", buf);
}

SWFAction compileSWFActionCode(const char *script)
{
	SWFOutput output;
	Buffer b;

	SWF_error = print_error;
	SWF_warn = print_warn;

	ErrorString = strmirror(&ErrorString,"");
	WarnString = strmirror(&WarnString,"");

	/* yydebug = 1; */

	if(SWF_versionNum == 4)
	{
		swf4ParseInit(script, 0);

		if(swf4parse((void *)&b) != 0)
		{
			SWF_error = mhsp_error;
			SWF_warn = mhsp_warn;
			return NULL;
		}
	}
	else
	{
		swf5ParseInit(script, 0);

		if(swf5parse((void *)&b) != 0)
		{
			SWF_error = mhsp_error;
			SWF_warn = mhsp_warn;
			return NULL;
		}
	}

	SWF_error = mhsp_error;
	SWF_warn = mhsp_warn;

	output = newSWFOutput();

	if(b)
	{
		SWFOutput_writeBuffer(output, (char*) b->buffer, bufferLength(b));
		destroyBuffer(b);
	}

	SWFOutput_writeUInt8(output, SWFACTION_END);

	return newSWFAction_fromOutput(output);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
