/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2007 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#define MINGOBJ(mhspobj, type) (type)((mhspobj)->realObject)
#define OBJTYPE(mhspobj)       ((mhspobj)->type)
#define SETNODESTFLAG(mhspobj) ((mhspobj)->nodest) = 1

enum mhspObjectType {
	T_SWFShape,
	T_SWFMovie,
	T_SWFDisplayItem,
	T_SWFMovieClip,
	T_SWFGradient,
	T_SWFBitmap,
	T_SWFFill,
	T_SWFMorph,
	T_SWFText,
	T_SWFFont,
	T_SWFBrowserFont,
	T_SWFTextField,
	T_SWFAction,
	T_SWFButton,
	T_SWFVideoStream,
	T_SWFFontCharacter,
	T_SWFSound,
	T_SWFSoundStream,
	T_SWFSoundInstance,
	T_SWFPrebuiltClip,
	T_SWFInput,
	T_SWFCharacter,
	T_FILE,
	T_DATA
};

typedef struct _mhspObject {
	enum mhspObjectType type;
	void *realObject;
	struct _mhspObject *rel;
	int nodest;
} MhspObject;

typedef MhspObject* PMhspObject;

MhspObject* allocMhspObject(void *realObject, enum mhspObjectType type);
void destroyMhspObject(MhspObject *object);
MhspObject* addMhspObject(void *realObject, enum mhspObjectType type);
void appendRelative(MhspObject *object, MhspObject *rel);
/*void removeMhspObject(MhspObject *object);
*/
