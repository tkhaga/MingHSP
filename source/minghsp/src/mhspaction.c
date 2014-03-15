/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2006 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <string.h>
#include <windows.h>

#ifdef JAMING
#include "../ming.h"
#else
#include "ming.h"
#endif
#include "toutf8.h"
#include "mhsputil.h"
#include "mhspobj.h"
#include "mhsperror.h"
#include "hspdll.h"

#undef  EXPORT
#define EXPORT __declspec(dllexport)

SWFAction SWFAction_load(unsigned char *obj);
int SWFAction_save(unsigned char *obj, int objsize, SWFAction action);

#ifndef NOACTION
char* ErrorString;
char* WarnString;
#endif

extern int SWF_versionNum;

/* SWFAction */

EXPORT BOOL WINAPI mhsp_SWFAction(PMhspObject *p1, char *script, int p3, int p4)
{
#ifndef NOACTION
	SWFAction action;
	char *out;

	setFuncName("SWFAction");
	if (SWF_versionNum >= 6) {
		out = toutf8(script);
	}
	else {
		out = script;
	}
	action = compileSWFActionCode(out);
	free(out);
	if ( !(*p1 = addMhspObject(action, T_SWFAction)) ) {
		return -1;
	}
#endif
	return 0;
}

EXPORT BOOL WINAPI mhsp_SWFAction_load(HSPEXINFO *hei, int p2, int p3, int p4)
{
	unsigned char *obj;
	PMhspObject *p1;
	SWFAction action;

	setFuncName("SWFAction_load");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	obj = (unsigned char *)hei->HspFunc_prm_getv();
	action = SWFAction_load(obj);
	if ( !(*p1 = addMhspObject(action, T_SWFAction)) ) {
		return -1;
	}

	return 0;
}

EXPORT BOOL WINAPI mhsp_SWFAction_save(unsigned char *p1, int p2, PMhspObject action, int p4)
{
#ifndef NOACTION
	setFuncName("SWFAction_save");
	return -SWFAction_save(p1, p2, MINGOBJ(action, SWFAction));
#else
	return 0;
#endif
}

#ifndef JAMING
EXPORT BOOL WINAPI mhsp_getaswarn(void *p1, int p2, int p3, int p4)
{
#ifndef NOACTION
	int len = strlen(WarnString);
	if (len < p2) {
		strcpy(p1, WarnString);
	}
	return -len - 1;
#else
	return 0;
#endif
}

EXPORT BOOL WINAPI mhsp_getaserror(void *p1, int p2, int p3, int p4)
{
#ifndef NOACTION
	int len = strlen(ErrorString);
	if (len < p2) {
		strcpy(p1, ErrorString);
	}
	return -len - 1;
#else
	return 0;
#endif
}
#endif
