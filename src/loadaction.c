/*
    MingHSP - Ming wrapper for HSP
    THAGA http://www.h5.dion.ne.jp/~markent/

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

#include <windows.h>
#include <stdio.h>

#include "./blocks/output.h"
#include "./blocks/action.h"
#include "./actioncompiler/compile.h"

#undef  EXPORT
#define EXPORT __declspec(dllexport)

extern char funcname[];
extern int SWF_versionNum;

char SWFAction_load_flag = 0;
char *obj;

EXPORT BOOL WINAPI SWFAction_load(SWFAction *p1, int p2, int p3, int p4)
{
	SWFOutput output;

	if (SWFAction_load_flag)
	{
		strcpy(funcname, "SWFAction_load");
		output = newSWFOutput();
		SWFOutput_writeBuffer(output, obj + sizeof(long), *(long *)obj);
		SWFOutput_writeUInt8(output, SWFACTION_END);
		*p1 = newSWFAction_fromOutput(output);
		free(obj);
		SWFAction_load_flag = 0;
	}else{
		obj = malloc(p2);
		memcpy(obj, p1, p2);
		SWFAction_load_flag = 1;
	}
	return 0;
}

EXPORT BOOL WINAPI SWFAction_save(long *p1, char *script, int p3, int p4)
{
  Buffer b;
	long blength;

	strcpy(funcname, "SWFAction_save");

  /* yydebug = 1; */

  if(SWF_versionNum == 4)
  {
    swf4ParseInit(script, 0);

    if(swf4parse((void *)&b) != 0)
      return -1;
  }
  else
  {
    swf5ParseInit(script, 0);

    if(swf5parse((void *)&b) != 0)
      return -1;
  }

    blength = bufferLength(b);
    if (blength > p3)
    	return -2;
    *p1 = blength;
    memcpy(p1 + 1, b->buffer, blength);

	return -(blength + sizeof(long));
}
