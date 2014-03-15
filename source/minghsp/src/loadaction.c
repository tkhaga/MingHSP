/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2006 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include "blocks/output.h"
#include "blocks/action.h"
#include "actioncompiler/compile.h"
#include "actioncompiler/action.h"

extern int method_index, method_size;

extern void mhsp_method(byte b, byte *data);

SWFAction SWFAction_load(unsigned char *obj)
{
	unsigned long length;
	SWFOutput output;

	length = ((unsigned long)obj[0]      ) + ((unsigned long)obj[1] <<  8) +
	         ((unsigned long)obj[2] << 16) + ((unsigned long)obj[3] << 24);
	output = newSWFOutput();
	SWFOutput_writeBuffer(output, obj + sizeof(unsigned long), length);
	SWFOutput_writeUInt8(output, SWFACTION_END);
	return newSWFAction_fromOutput(output);
}

int SWFAction_save(unsigned char *obj, int objsize, SWFAction action)
{
	unsigned long length;
	SWFOutput output;

#ifdef JAMING
	output = action->output;
	length = SWFOutput_length(output);
#else
	output = SWFOutputBlock_getOutput(action);
	length = SWFOutputBlock_getLength(action);
#endif
	if (length + sizeof(unsigned long) > objsize) {
		return -(length + sizeof(unsigned long));
	}
	obj[0] =  length        & 0xff;
	obj[1] = (length >> 8)  & 0xff;
	obj[2] = (length >> 16) & 0xff;
	obj[3] = (length >> 24) & 0xff;
	method_index = sizeof(unsigned long);
	method_size = objsize;
	SWFOutput_writeToMethod(output, (SWFByteOutputMethod)mhsp_method, obj);
	return length + sizeof(unsigned long);
}
