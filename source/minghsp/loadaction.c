/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2006 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <windows.h>

#include "blocks/output.h"
#include "blocks/action.h"
#include "actioncompiler/compile.h"
#include "actioncompiler/action.h"
#include "hspdll.h"

#undef  EXPORT
#define EXPORT __declspec(dllexport)

extern char funcname[];
extern int idx, bufsize;

void mhsp_method(byte b, byte *data);

EXPORT BOOL WINAPI SWFAction_load(HSPEXINFO *hei, int p2, int p3, int p4)
{
	unsigned char *obj;
	unsigned long length;
	SWFOutput output;
	SWFAction *p1;
	lstrcpy(funcname, "SWFAction_load");
	p1 = (SWFAction *)hei->HspFunc_prm_getv();
	obj = (unsigned char *)hei->HspFunc_prm_getv();
	length = ((unsigned long)obj[0]      ) + ((unsigned long)obj[1] <<  8) +
	         ((unsigned long)obj[2] << 16) + ((unsigned long)obj[3] << 24);
	output = newSWFOutput();
	SWFOutput_writeBuffer(output, (char *)obj + sizeof(unsigned long), length);
	SWFOutput_writeUInt8(output, SWFACTION_END);
	*p1 = newSWFAction_fromOutput(output);
	return 0;
}

EXPORT BOOL WINAPI SWFAction_save(unsigned char *p1, int p2, SWFAction action, int p4)
{
	unsigned long length;
	SWFOutput output;
	lstrcpy(funcname, "SWFAction_save");
#ifdef JAMING
	output = action->output;
	length = SWFOutput_length(output);
#else
	output = SWFOutputBlock_getOutput(action);
	length = SWFOutputBlock_getLength(action);
#endif
	if (length + sizeof(unsigned long) > p2)
		return -(length + sizeof(unsigned long));
	p1[0] =  length        & 0xff;
	p1[1] = (length >> 8)  & 0xff;
	p1[2] = (length >> 16) & 0xff;
	p1[3] = (length >> 24) & 0xff;
	idx = sizeof(unsigned long);
	bufsize = p2;
	SWFOutput_writeToMethod(output, (SWFByteOutputMethod)mhsp_method, p1);
	return -(length + sizeof(unsigned long));
}
