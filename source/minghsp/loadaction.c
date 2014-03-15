/*
    MingHSP - Ming wrapper for HSP
    THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <windows.h>
#include <stdio.h>

#include "blocks/output.h"
#include "blocks/action.h"
#include "actioncompiler/compile.h"
#include "actioncompiler/action.h"
#include "hspdll.h"

#undef  EXPORT
#define EXPORT __declspec(dllexport)

extern char funcname[];
extern int SWF_versionNum;

#ifndef JAMING
extern char* ErrorString;
extern char* WarnString;

extern void (*SWF_warn)(const char *msg, ...);
extern void (*SWF_error)(const char *msg, ...);

extern void warn_default(const char *msg, ...);
extern void error_default(const char *msg, ...);

char* strmirror(char** form,const char* str);
char* strplus(char** form,const char* latt);

void print_error(const char*format,...);
void print_warn(const char*format,...);
#endif

EXPORT BOOL WINAPI SWFAction_load(HSPEXINFO *hei, int p2, int p3, int p4)
{
	char *obj;
	SWFOutput output;
	SWFAction *p1;
	lstrcpy(funcname, "SWFAction_load");
	p1 = (SWFAction *)hei->HspFunc_prm_getv();
	obj = (char *)hei->HspFunc_prm_getv();
	output = newSWFOutput();
	SWFOutput_writeBuffer(output, obj + sizeof(long), *(long *)obj);
	SWFOutput_writeUInt8(output, SWFACTION_END);
	*p1 = newSWFAction_fromOutput(output);
	return 0;
}

EXPORT BOOL WINAPI SWFAction_save(long *p1, char *script, int p3, int p4)
{
	Buffer b;
	long blength;

	strcpy(funcname, "SWFAction_save");

#ifndef JAMING
	SWF_error = print_error;
	SWF_warn = print_warn;

	ErrorString = strmirror(&ErrorString,"");
	WarnString = strmirror(&WarnString,"");
#endif

  /* yydebug = 1; */

	if(SWF_versionNum == 4)
	{
		swf4ParseInit(script, 0);

		if(swf4parse((void *)&b) != 0)
		{
#ifndef JAMING
			SWF_error = error_default;
			SWF_warn = warn_default;
#endif
			return -1;
		}
	}
	else
	{
		swf5ParseInit(script, 0);

		if(swf5parse((void *)&b) != 0)
		{
#ifndef JAMING
			SWF_error = error_default;
			SWF_warn = warn_default;
#endif
			return -1;
		}
	}

#ifndef JAMING
	SWF_error = error_default;
	SWF_warn = warn_default;
#endif

    blength = bufferLength(b);
    if (blength > p3)
    {
		destroyBuffer(b);
    	return -2;
    }
    *p1 = blength;
    memcpy(p1 + 1, b->buffer, blength);
	destroyBuffer(b);
	return -(blength + sizeof(long));
}
