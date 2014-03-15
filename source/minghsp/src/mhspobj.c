/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2007 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef JAMING
#include "../ming.h"
#else
#include "ming.h"
#endif
#include "mhsputil.h"
#include "mhspobj.h"

#ifndef JAMING
void destroySWFVideoStream(SWFVideoStream stream);
void destroySWFPrebuiltClip(SWFPrebuiltClip clip);
#endif
void destroySWFCharacter(SWFCharacter character);

mhspList mhspObjects;

MhspObject* allocMhspObject(void *realObject, enum mhspObjectType type)
{
	MhspObject *object;

	if (!realObject) {
		return NULL;
	}
	if ( !(object = (MhspObject *)malloc(sizeof(MhspObject))) ) {
		return NULL;
	}
	object->type = type;
	object->realObject = realObject;
	object->rel = NULL;
	object->nodest = 0;

	return object;
}

void destroyMhspObject(MhspObject *object)
{
	if (!object) {
		return;
	}

	if (object->rel) {
		destroyMhspObject(object->rel);
	}

	if (object->nodest == 0) {
		switch (object->type) {
			case T_SWFShape:
				destroySWFShape((SWFShape)object->realObject);
				break;
			case T_SWFMovie:
				destroySWFMovie((SWFMovie)object->realObject);
				break;
			case T_SWFMovieClip:
				destroySWFMovieClip((SWFMovieClip)object->realObject);
				break;
			case T_SWFGradient:
				destroySWFGradient((SWFGradient)object->realObject);
				break;
			case T_SWFBitmap:
				destroySWFBitmap((SWFBitmap)object->realObject);
				break;
			case T_SWFFill:
				destroySWFFill((SWFFill)object->realObject);
				break;
			case T_SWFMorph:
				destroySWFMorph((SWFMorph)object->realObject);
				break;
			case T_SWFText:
				destroySWFText((SWFText)object->realObject);
				break;
			case T_SWFFont:
				destroySWFFont((SWFFont)object->realObject);
				break;
			case T_SWFBrowserFont:
				destroySWFBrowserFont((SWFBrowserFont)object->realObject);
				break;
			case T_SWFTextField:
				destroySWFTextField((SWFTextField)object->realObject);
				break;
			case T_SWFAction:
				destroySWFAction(MINGOBJ(object, SWFAction));
				break;
			case T_SWFButton:
				destroySWFButton((SWFButton)object->realObject);
				break;
			case T_SWFVideoStream:
				destroySWFVideoStream((SWFVideoStream)object->realObject);
				break;
			case T_SWFSound:
				destroySWFSound((SWFSound)object->realObject);
				break;
			case T_SWFPrebuiltClip:
				destroySWFPrebuiltClip((SWFPrebuiltClip)object->realObject);
				break;
			case T_SWFInput:
				destroySWFInput(MINGOBJ(object, SWFInput));
				break;
			case T_SWFCharacter:
				destroySWFCharacter(MINGOBJ(object, SWFCharacter));
				break;
			case T_FILE:
				fclose(MINGOBJ(object, FILE *));
				break;
			case T_DATA:
				free(MINGOBJ(object, void *));
				break;
			default:
				break;
		}
	}
	free(object);
}

MhspObject* addMhspObject(void *realObject, enum mhspObjectType type)
{
	MhspObject *object;

	if ( !(object = allocMhspObject(realObject, type)) ) {
		return NULL;
	}
	if (mhsp_list_append(&mhspObjects, object)) {
		destroyMhspObject(object);
		return NULL;
	}

	return object;
}

void appendRelative(MhspObject *object, MhspObject *rel)
{
	MhspObject *next = object;

	while (next->rel) {
		next = next->rel;
	}
	next->rel = rel;
}
/*
void removeMhspObject(MhspObject *object)
{
	mhsp_list_remove(&mhspObjects, object);
}
*/
